#include "ocl.hpp"
#include <iostream>
#include <cstring>
OCL* OCL::m_instance = 0; // Singleton instance


#define EVALKERNEL "evalKernel"

const char programString[] = "__kernel void evalKernel(\
                         __global int* bitboards,\
                         __global int* score\
                        )\
{\
    int i = get_group_id(0);\
    int j = get_local_id(0);\
    //int score = bitboards[i*13 + j];\
    if(j == 0)\
        score[i] = 5;\
}";

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

    m_evalKernel = clCreateKernel(m_program,EVALKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<EVALKERNEL<<std::endl;
        exit(1);
    }
}

void OCL::BuildProgram()
{

    int err;

    const size_t programSize = strlen(programString);
    const char* srcptr[] = {programString};

    /* Create program from file */
    m_program = clCreateProgramWithSource(m_context, 1, srcptr, &programSize, &err);
    if (err < 0) {
        std::cout << "Couldn't create the program\n";
        exit(1);
    }

    /* Build program */
    err = clBuildProgram(m_program, 0, NULL, NULL, NULL, NULL);
    if (err < 0) {
        std::cout << "Couldn't build program\n";
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
    cl_int scores[num_boards] = {0};

    /* Create Buffers */
    int err;
    cl_mem bitboard_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY,bitboards_per_board * sizeof(bitboard),(void *)board.m_pList,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl buffer\n";
        exit(1);
    }

    cl_mem score_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, num_boards * sizeof(cl_int),scores,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl buffer\n";
        exit(1);
    }

    /* Set kernel args */
    err = clSetKernelArg(m_evalKernel,0,sizeof(cl_mem),&bitboard_buffer);
    err |= clSetKernelArg(m_evalKernel,1,sizeof(cl_mem),&score_buffer);
    if (err < 0) {
        std::cout << "Couldn't set kernel arg\n";
        exit(1);
    }

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,m_evalKernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,score_buffer,CL_TRUE,0, num_boards * sizeof(cl_int),scores,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
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
