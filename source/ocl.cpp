#include "ocl.hpp"
#include <iostream>
#include <cstring>

OCL* OCL::m_instance = 0; // Singleton instance

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define KERNEL_DIR STR(__DIR__)

#define EVALKERNEL "evalKernel"
#define PIECEMOVEKERNEL "moveKernel"
#define WPAWNMOVEKERNEL "WhitePawnKernel"
#define BPAWNMOVEKERNEL "BlackPawnKernel"
#define EVAL_PROGRAM_FILE "/EvalKernel.cl"
#define MOVE_PROGRAM_FILE "/moveKernel.cl"



std::string OCL::getSourceDir()
{
#ifdef KERNEL_DIR
    std::string file(KERNEL_DIR); 
#else
    std::string file(__FILE__); // assume .cl filesin same dir

    size_t last_slash_idx = file.rfind('\\');
    if ( std::string::npos != last_slash_idx)
    {
        return file.substr(0, last_slash_idx);
    }

    last_slash_idx = file.rfind('/');
    if ( std::string::npos != last_slash_idx)
    {
        return file.substr(0, last_slash_idx);
    }

#endif

    return file;
}


// Singleton class
OCL* OCL::getInstance()
{
    if (!m_instance)
        m_instance = new OCL;
    return m_instance;
}



OCL::OCL()
{

    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &m_platform, NULL);
    if (err < 0) {
        std::cout << "Couldn't locate an OCL platform " << err << "\n";
        exit(1);
    }

    /* Access a device */
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 1, &m_device, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_CPU, 1, &m_device, NULL);
    }
    if (err < 0) {
        std::cout << "Couldn't locate an OCL device\n";
        exit(1);
    }

    m_context = clCreateContext(NULL, 1, &m_device, NULL, NULL, &err);
    if (err < 0) {
        std::cout << "Couldn't create an OCL context\n";
        exit(1);
    }

    char buffer[1024];
    err = clGetDeviceInfo(m_device, CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
    std::cout << "OpenCL Device: " << buffer << std::endl;

    m_queue = clCreateCommandQueue(m_context, m_device, 0, &err);
    if (err < 0) {
        std::cout << "Couldn't create an OCL command queue\n";
        exit(1);
    }

    BuildProgram();

    m_evalKernel = clCreateKernel(m_evalProgram,EVALKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<EVALKERNEL << " "<<err<<std::endl;
        exit(1);
    }

    m_pieceMoveKernel = clCreateKernel(m_evalProgram,EVALKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<EVALKERNEL << " "<<err<<std::endl;
        exit(1);
    }

    m_wPawnMoveKernel = clCreateKernel(m_moveProgram,WPAWNMOVEKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<WPAWNMOVEKERNEL << " "<<err<<std::endl;
        exit(1);
    }

    m_bPawnMoveKernel = clCreateKernel(m_moveProgram,BPAWNMOVEKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<BPAWNMOVEKERNEL << " "<<err<<std::endl;
        exit(1);
    }
}

void OCL::BuildProgram()
{

    int err;
    
    std::string evalFile = OCL::getSourceDir().append(EVAL_PROGRAM_FILE);
    

    FILE *fp = fopen(evalFile.c_str(), "r");
    if (!fp) {
        std::cout << "Failed to read kernel file "<<evalFile<<std::endl;
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    size_t programSize = ftell(fp);
    rewind(fp);
    char* program_buffer = new char[programSize + 1];
    program_buffer[programSize] = '\0';
    fread(program_buffer, sizeof(char), programSize, fp);
    fclose(fp);


    /* Create program from file */
    m_evalProgram = clCreateProgramWithSource(m_context, 1, (const char**)&program_buffer, &programSize, &err);
    if (err < 0) {
        std::cout << "Couldn't create the program\n";
        exit(1);
    }

    /* Build program */
    err = clBuildProgram(m_evalProgram, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        std::cout << "Couldn't build program" << EVAL_PROGRAM_FILE <<"\n";
        char buffer[2048];
        size_t length;
        clGetProgramBuildInfo(m_evalProgram, m_device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
        std::cout<<"--- Build log ---\n "<<buffer<<std::endl;
        exit(1);
    }

    std::string moveFile = OCL::getSourceDir().append(MOVE_PROGRAM_FILE);
    fp = fopen(moveFile.c_str(), "r");
    if (!fp) {
        std::cout << "Failed to read kernel file "<<moveFile<<std::endl;
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    programSize = ftell(fp);
    rewind(fp);
    program_buffer = new char[programSize + 1];
    program_buffer[programSize] = '\0';
    fread(program_buffer, sizeof(char), programSize, fp);
    fclose(fp);


    /* Create program from file */
    m_moveProgram = clCreateProgramWithSource(m_context, 1, (const char**)&program_buffer, &programSize, &err);
    if (err < 0) {
        std::cout << "Couldn't create the program\n";
        exit(1);
    }

    /* Build program */
    err = clBuildProgram(m_moveProgram, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        std::cout << "Couldn't build program" << MOVE_PROGRAM_FILE <<"\n";
        char buffer[2048];
        size_t length;
        clGetProgramBuildInfo(m_moveProgram, m_device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
        std::cout<<"--- Build log ---\n "<<buffer<<std::endl;
        exit(1);
    }
}

int OCL::RunEvalKernel(const Board& board)
{

    /* local vars */
    const unsigned int bitboards_per_board = 13;
    const unsigned int num_boards = 1;
    const size_t global_size = bitboards_per_board * num_boards;
    const size_t local_size = bitboards_per_board;
    int scores[num_boards] = {0};

    /* Create Buffers */
    int err;
    cl_mem bitboard_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, bitboards_per_board * sizeof(bitboard),(void *)board.m_pList,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl buffer\n";
        exit(1);
    }

    cl_mem score_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, num_boards * sizeof(int),scores,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl buffer\n";
        exit(1);
    }

    /* Set kernel args */
    err = clSetKernelArg(m_evalKernel,0,sizeof(cl_mem),&bitboard_buffer);
    err |= clSetKernelArg(m_evalKernel,1,sizeof(cl_mem),&score_buffer);
    err |= clSetKernelArg(m_evalKernel,2,num_boards * sizeof(cl_int),NULL);

    if (err < 0) {
        std::cout << "Couldn't set kernel arg\n";
        exit(1);
    }

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,m_evalKernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel "<<err <<"\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,score_buffer,CL_TRUE,0, num_boards * sizeof(int),scores,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
        exit(1);
    }

    err = clFinish(m_queue);
     if (err < 0) {
        std::cout << "Couldn't wait for CL tae finish\n";
        exit(1);
    }

    clReleaseMemObject(bitboard_buffer);
    clReleaseMemObject(score_buffer);

    // Just one board for now
    return scores[0];
}

OCL::~OCL()
{
    /* Deallocate resources */
    clReleaseContext(m_context);
    clReleaseCommandQueue(m_queue);
    clReleaseKernel(m_evalKernel);
}
