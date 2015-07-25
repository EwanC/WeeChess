#include "ocl.hpp"
#include "types.hpp"
#include <iostream>
#include <cstring>

OCL* OCL::m_instance = 0; // Singleton instance
uint OCL::possible_pawn_moves = 12;
uint OCL::possible_piece_moves = 30;


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define KERNEL_DIR STR(__DIR__)

#define EVALKERNEL "evalKernel"
#define PIECEMOVEKERNEL "moveKernel"
#define WPAWNMOVEKERNEL "WhitePawnKernel"
#define BPAWNMOVEKERNEL "BlackPawnKernel"
#define PIECECAPTUREKERNEL "moveCapture"
#define WPAWNCAPTUREKERNEL "WhitePawnCapture"
#define BPAWNCAPTUREKERNEL "BlackPawnCapture"
#define EVAL_PROGRAM_FILE "/EvalKernel.cl"
#define MOVE_PROGRAM_FILE "/moveKernel.cl"


// Most Valuable attacker, least valuable victim
static int32_t  MvvLvaScores[13][13] = {
  { 141743112, 32767, -37664, 32767, -141682440, 32767, 54012236, 0,   -37680, 32767, -136429924, 32767, 0},
  { 0,         105,   104,    103,   102,        101,   100,      105, 104,    103,   102,        101,   100},
  { -37824,    205,   204,    203,   202,        201,   200,      205, 204,    203,   202,        201,   200},
  { 32767,     305,   304,    303,   302,        301,   300,      305, 304,    303,   302,        301,   300},
  { 4301065,   405,   404,    403,   402,        401,   400,      405, 404,    403,   402,        401,   400},
  { 32767,     505,   504,    503,   502,        501,   500,      505, 504,    503,   502,        501,   500},
  { 0,         605,   604,    603,   602,        601,   600,      605, 604,    603,   602,        601,   600},
  { 0,         105,   104,    103,   102,        101,   100,      105, 104,    103,   102,        101,   100},
  { -1,        205,   204,    203,   202,        201,   200,      205, 204,    203,   202,        201,   200},
  { 0,         305,   304,    303,   302,        301,   300,      305, 304,    303,   302,        301,   300},
  { 16,        405,   404,    403,   402,        401,   400,      405, 404,    403,   402,        401,   400},
  { 0,         505,   504,    503,   502,        501,   500,      505, 504,    503,   502,        501,   500},
  { 1,         605,   604,    603,   602,        601,   600,      605, 604,    603,   602,        601,   600}
};



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

    m_pieceMoveKernel = clCreateKernel(m_moveProgram,PIECEMOVEKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<PIECEMOVEKERNEL << " "<<err<<std::endl;
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

    m_pieceCaptureKernel = clCreateKernel(m_moveProgram,PIECECAPTUREKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<PIECECAPTUREKERNEL<< " "<<err<<std::endl;
        exit(1);
    }

    m_wPawnCaptureKernel = clCreateKernel(m_moveProgram,WPAWNCAPTUREKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<WPAWNCAPTUREKERNEL << " "<<err<<std::endl;
        exit(1);
    }

    m_bPawnCaptureKernel = clCreateKernel(m_moveProgram,BPAWNCAPTUREKERNEL,&err);
    if (err < 0) {
        std::cout << "Couldn't create OCL kernel "<<BPAWNCAPTUREKERNEL << " "<<err<<std::endl;
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
    char macro_str[128];
    sprintf(macro_str, "-DMAX_PAWN_MOVES=%u -DMAX_PIECE_MOVES=%u",OCL::possible_pawn_moves, OCL::possible_piece_moves);

    err = clBuildProgram(m_moveProgram, 0, NULL,macro_str, NULL, NULL);
    if (err < 0) {
        std::cout << "Couldn't build program" << MOVE_PROGRAM_FILE <<"\n";
        char buffer[2048];
        size_t length;
        clGetProgramBuildInfo(m_moveProgram, m_device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
        std::cout<<"--- Build log ---\n "<<buffer<<std::endl;
        exit(1);
    }
}


std::vector<Move> OCL::RunPawnMoveKernel(const Board& b, const bool capture)
{
    
    std::vector<Move> pawn_move_vec;

    bitboard Bitboard = b.m_side == Colour::WHITE ? b.m_pList[Piece::wP] : b.m_pList[Piece::bP];

    int pawns = Bitboard::countBits(Bitboard);
    if(pawns == 0)
        return pawn_move_vec;

    unsigned int* pawn_squares = new unsigned int[pawns];
  
    for (int pceNum = 0; pceNum < pawns; ++pceNum) {
            int sq64 = (Bitboard::popBit(&Bitboard));
            pawn_squares[pceNum] = SQ120(sq64);
    }

    int err = CL_SUCCESS;
    cl_mem square_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pawns * sizeof(unsigned int),(void *)pawn_squares,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn square buffer " << err <<"\n";
        exit(1);
    }

    cl_mem pieces_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE| CL_MEM_COPY_HOST_PTR, TOTAL_SQUARES * sizeof(int),(void *)b.m_board,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn peices buffer\n";
        exit(1);
    }

    cl_mem Enpass_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int),(void *)&b.m_enPas,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn enpass buffer\n";
        exit(1);
    }

    const uint move_buffer_size = pawns * OCL::possible_pawn_moves;
    unsigned long moves[move_buffer_size];
    for(int i = 0; i < move_buffer_size; ++i)
        moves[i] = 0;

    cl_mem moves_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, move_buffer_size * sizeof(unsigned long),moves,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl wp moves buffer\n";
        exit(1);
    }
    

    const size_t global_size = pawns;
    const size_t local_size = pawns;
    
    cl_kernel kernel;
    if(capture)
        kernel = b.m_side == Colour::WHITE ? m_wPawnCaptureKernel : m_bPawnCaptureKernel;
    else
        kernel = b.m_side == Colour::WHITE ? m_wPawnMoveKernel : m_bPawnMoveKernel;
    

   


    /* Set kernel args */
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&square_buffer);
    err |= clSetKernelArg(kernel,1,sizeof(cl_mem),&pieces_buffer);
    err |= clSetKernelArg(kernel,2,sizeof(cl_mem),&Enpass_buffer);
    err |= clSetKernelArg(kernel,3,sizeof(cl_mem),&moves_buffer);

    if (err < 0) {
        std::cout << "Couldn't set pawn move kernel arg\n";
        exit(1);
    }

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,kernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel "<<err <<"\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,moves_buffer,CL_TRUE,0, move_buffer_size * sizeof(unsigned long),moves,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
        exit(1);
    }

    err = clFinish(m_queue);
     if (err < 0) {
        std::cout << "Couldn't wait for CL tae finish\n";
        exit(1);
    }


    for(int i = 0; i < move_buffer_size; ++i)
    {
        if (moves[i] == 0) continue;

        Move m;
        m.m_move = moves[i];


        if(m.m_move & MFLAGEP)
            m.m_score = 105 + 1000000;
        if(CAPTURED(m.m_move))
            m.m_score = MvvLvaScores[CAPTURED(m.m_move)][b.m_board[FROMSQ(m.m_move)]] + 1000000;
        else if (b.m_searchKillers[0][b.m_ply] ==  m.m_move)
            m.m_score = 900000;
        else if (b.m_searchKillers[1][b.m_ply] ==  m.m_move)
            m.m_score = 800000;
        else
            m.m_score = b.m_searchHistory[b.m_board[FROMSQ(m.m_move)]][TOSQ(m.m_move)];

        pawn_move_vec.push_back(m);
    }

    clReleaseMemObject(square_buffer);
    clReleaseMemObject(pieces_buffer);
    clReleaseMemObject(Enpass_buffer);
    clReleaseMemObject(moves_buffer);

    delete[] pawn_squares;

    return pawn_move_vec;
}


void OCL::SetPieceHostBuffer(unsigned int* pieces, bitboard bb, unsigned int& itr, const unsigned short piece_count)
{
    int count = Bitboard::countBits(bb);
    for (int pceNum = 0; pceNum < piece_count; ++pceNum) 
    {
        if (pceNum < count)
        {  
           int sq120 = SQ120(Bitboard::popBit(&bb));
           pieces[itr++] = sq120;
        }
        else
            itr++;
    }
}

std::vector<Move> OCL::RunPieceMoveKernel(const Board& b, const bool capture)
{

    static const Piece SlidePce[2][3] = {{wB, wR, wQ}, {bB, bR, bQ}};
    static const Piece NonSlidePce[2][2] = {{wN, wK}, {bN, bK}};


    std::vector<Move> piece_move_vec;

    ushort max_pieces = 0;

    bitboard Nbb = b.m_pList[static_cast<int>(NonSlidePce[b.m_side][0])];
    ushort bbCount = Bitboard::countBits(Nbb);
    max_pieces = bbCount > max_pieces ? bbCount : max_pieces;

    bitboard Bbb = b.m_pList[static_cast<int>(SlidePce[b.m_side][0])];
    bbCount = Bitboard::countBits(Bbb);
    max_pieces = bbCount > max_pieces ? bbCount : max_pieces;

    bitboard Rbb = b.m_pList[static_cast<int>(SlidePce[b.m_side][1])];
    bbCount = Bitboard::countBits(Rbb);
    max_pieces = bbCount > max_pieces ? bbCount : max_pieces;

    bitboard Qbb = b.m_pList[static_cast<int>(SlidePce[b.m_side][2])];
    bbCount = Bitboard::countBits(Qbb);
    max_pieces = bbCount > max_pieces ? bbCount : max_pieces;

    if (max_pieces == 0 ) max_pieces = 1; 


    const size_t global_size = 5 * max_pieces;
    const size_t local_size = max_pieces;
    

    // Piece Buffer
    // WB, WB, WR, WR, WQ, WN, WN, WK
    unsigned int* pieces = new unsigned int [global_size];
    unsigned int itr = 0;

    for(int i = 0; i < global_size;++i)
        pieces[i] = Square::NO_SQ;
    
    SetPieceHostBuffer(pieces, Nbb, itr, local_size);

    SetPieceHostBuffer(pieces, Bbb, itr, local_size);
   
    SetPieceHostBuffer(pieces, Rbb, itr, local_size);

    SetPieceHostBuffer(pieces, Qbb, itr, local_size);

    bitboard Kbb = b.m_pList[static_cast<int>(NonSlidePce[b.m_side][1])];
    SetPieceHostBuffer(pieces, Kbb, itr, local_size);


    int err;
    cl_mem square_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, global_size * sizeof(unsigned int),(void *)pieces,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl side square buffer\n";
        exit(1);
    }

    cl_mem side_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (void *)&b.m_side,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl piece side buffer\n";
        exit(1);
    }


    cl_mem pieces_buffer = clCreateBuffer(m_context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, TOTAL_SQUARES * sizeof(int),(void*)b.m_board,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl pawn peices buffer\n";
        exit(1);
    }

    const uint move_buffer_size = global_size * OCL::possible_piece_moves;
    unsigned long moves[move_buffer_size];
    for(int i = 0; i < move_buffer_size; ++i)
        moves[i] = 0;

    cl_mem moves_buffer = clCreateBuffer(m_context,CL_MEM_READ_WRITE  | CL_MEM_COPY_HOST_PTR , move_buffer_size * sizeof(unsigned long),moves,&err);
    if (err < 0) {
        std::cout << "Couldn't create cl side moves buffer\n";
        exit(1);
    }


    /* Set kernel args */
    err = clSetKernelArg(m_pieceMoveKernel,0,sizeof(cl_mem),&square_buffer);
     if (err < 0) {
        std::cout << "A Couldn't set Piece move kernel arg\n";
        exit(1);
    }
    err |= clSetKernelArg(m_pieceMoveKernel,1,sizeof(cl_mem),&side_buffer);
     if (err < 0) {
        std::cout << "B Couldn't set Piece move kernel arg\n";
        exit(1);
    }
    err |= clSetKernelArg(m_pieceMoveKernel,2,sizeof(cl_mem),&pieces_buffer);
     if (err < 0) {
        std::cout << "C Couldn't set Piece move kernel arg: "<< err<<"\n";
        exit(1);
    }
    err |= clSetKernelArg(m_pieceMoveKernel,3,sizeof(cl_mem),&moves_buffer);
    if (err < 0) {
        std::cout << "D Couldn't set Piece move kernel arg\n";
        exit(1);
    }

    cl_kernel kernel = capture ? m_pieceCaptureKernel : m_pieceMoveKernel;

    /* Enqueue Kernel*/
    err = clEnqueueNDRangeKernel(m_queue,m_pieceMoveKernel,1,NULL,&global_size,&local_size,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't enqueue the kernel "<<err <<"\n";
        exit(1);
    }

    /* Enqueue ReadBuffer */
    err = clEnqueueReadBuffer(m_queue,moves_buffer,CL_TRUE,0, move_buffer_size * sizeof(unsigned long),moves,0,NULL,NULL);
    if (err < 0) {
        std::cout << "Couldn't read cl buffer\n";
        exit(1);
    }

    err = clFinish(m_queue);
    if (err < 0) {
        std::cout << "Couldn't wait for CL tae finish\n";
        exit(1);
    }
   
    for(int i = 0; i < move_buffer_size; ++i)
    {
        if (moves[i] == 0) continue;

        Move m;
        m.m_move = moves[i];


        if(CAPTURED(m.m_move))
            m.m_score = MvvLvaScores[CAPTURED(m.m_move)][b.m_board[FROMSQ(m.m_move)]] + 1000000;
        else if (b.m_searchKillers[0][b.m_ply] ==  m.m_move)
            m.m_score = 900000;
        else if (b.m_searchKillers[1][b.m_ply] ==  m.m_move)
            m.m_score = 800000;
        else
            m.m_score = b.m_searchHistory[b.m_board[FROMSQ(m.m_move)]][TOSQ(m.m_move)];

        piece_move_vec.push_back(m);
    }

    clReleaseMemObject(square_buffer);
    clReleaseMemObject(pieces_buffer);
    clReleaseMemObject(side_buffer);
    clReleaseMemObject(moves_buffer);
    delete[] pieces;

    return piece_move_vec;
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
