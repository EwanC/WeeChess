#include "ocl.hpp"
#include "types.hpp"
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


void OCL::RunPawnMoveKernel(const Board& b)
{
    
    bitboard Bitboard = b.m_side == Colour::WHITE ? b.m_pList[Piece::wP] : b.m_pList[Piece::bP];

    int pawns = Bitboard::countBits(Bitboard);

    unsigned int* pawn_squares = new unsigned int[pawns];
  
    for (int pceNum = 0; pceNum < pawns; ++pceNum) {
            int sq64 = (Bitboard::popBit(&Bitboard));
            pawn_squares[pceNum] = SQ120(sq64);
    }

    int err;
    cl_mem square_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pawns * sizeof(unsigned int),(void *)pawn_squares,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn square buffer\n";
        exit(1);
    }

    cl_mem pieces_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, TOTAL_SQUARES * sizeof(int),(void *)b.m_board,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn  peices buffer\n";
        exit(1);
    }

    cl_mem Enpass_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, sizeof(unsigned int),(void *)b.m_enPas,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn  enpass buffer\n";
        exit(1);
    }

    #define MAX_MOVES_PER_POS 218
    unsigned long moves[MAX_MOVES_PER_POS] = {0};
    cl_mem moves_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, MAX_MOVES_PER_POS * sizeof(unsigned long),moves,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl wp moves buffer\n";
        exit(1);
    }

  

    const size_t global_size = pawns;
    const size_t local_size = pawns;
    

    cl_kernel kernel = b.m_side == Colour::WHITE ? m_wPawnMoveKernel : m_bPawnMoveKernel;


    /* Set kernel args */
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&square_buffer);
    err |= clSetKernelArg(kernel,1,sizeof(cl_mem),&pieces_buffer);
    err |= clSetKernelArg(kernel,2,sizeof(cl_mem),&Enpass_buffer);
    err |= clSetKernelArg(kernel,3,sizeof(cl_mem),&moves_buffer);

    if (err < 0) {
        std::cout << "Couldn't set kernel arg\n";
        exit(1);
    }

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,kernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel "<<err <<"\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,moves_buffer,CL_TRUE,0, MAX_MOVES_PER_POS * sizeof(unsigned long),moves,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
        exit(1);
    }

    err = clFinish(m_queue);
     if (err < 0) {
        std::cout << "Couldn't wait for CL tae finish\n";
        exit(1);
    }

    clReleaseMemObject(square_buffer);
    clReleaseMemObject(pieces_buffer);
    clReleaseMemObject(Enpass_buffer);
    clReleaseMemObject(moves_buffer);

    delete[] pawn_squares;

}

void OCL::RunPieceMoveKernel(const Board& b)
{

    // static const Piece SlidePce[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
    // static const Piece NonSlidePce[2][2] = {{wN, wK}, {bN, bK}};
    // for (Piece pce : SlidePce[b.m_side]) {
    //        assert(pce >= wP && pce <= bK);
    //        bitboard bb = b.m_pList[static_cast<int>(pce)];
    //        int pieces = Bitboard::countBits(bb);
    // }


    //
    //
    // TODO ARRANGE PIECES IN BUFFER
    //
    //
    bitboard Bitboard = b.m_side == Colour::WHITE ? b.m_pList[Piece::wP] : b.m_pList[Piece::bP];

    int pawns = Bitboard::countBits(Bitboard);

    unsigned int* pawn_squares = new unsigned int[pawns];
  
    for (int pceNum = 0; pceNum < pawns; ++pceNum) {
            int sq64 = (Bitboard::popBit(&Bitboard));
            pawn_squares[pceNum] = SQ120(sq64);
    }

    int err;
    cl_mem square_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pawns * sizeof(unsigned int),(void *)pawn_squares,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl side square buffer\n";
        exit(1);
    }

    cl_mem side_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, sizeof(int),(void *)b.m_side,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl piece side buffer\n";
        exit(1);
    }


    cl_mem pieces_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, TOTAL_SQUARES * sizeof(int),(void *)b.m_board,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn  peices buffer\n";
        exit(1);
    }

    
    #define MAX_MOVES_PER_POS 218
    unsigned long moves[MAX_MOVES_PER_POS] = {0};
    cl_mem moves_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE, MAX_MOVES_PER_POS * sizeof(unsigned long),moves,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl side moves buffer\n";
        exit(1);
    }

  

    const size_t global_size = pawns;
    const size_t local_size = pawns;
    

    /* Set kernel args */
    err = clSetKernelArg(m_pieceMoveKernel,0,sizeof(cl_mem),&square_buffer);
    err |= clSetKernelArg(m_pieceMoveKernel,1,sizeof(cl_mem),&side_buffer);
    err |= clSetKernelArg(m_pieceMoveKernel,2,sizeof(cl_mem),&pieces_buffer);
    err |= clSetKernelArg(m_pieceMoveKernel,3,sizeof(cl_mem),&moves_buffer);

    if (err < 0) {
        std::cout << "Couldn't set kernel arg\n";
        exit(1);
    }

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,m_pieceMoveKernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel "<<err <<"\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,moves_buffer,CL_TRUE,0, MAX_MOVES_PER_POS * sizeof(unsigned long),moves,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
        exit(1);
    }

    err = clFinish(m_queue);
     if (err < 0) {
        std::cout << "Couldn't wait for CL tae finish\n";
        exit(1);
    }

    clReleaseMemObject(square_buffer);
    clReleaseMemObject(pieces_buffer);
    clReleaseMemObject(side_buffer);
    clReleaseMemObject(moves_buffer);

    delete[] pawn_squares;

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
