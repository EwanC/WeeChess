#ifndef OCL_H
#define OCL_H

#include <CL/cl.h>
#include "board.hpp"
#include "move.hpp"
#include <string>
#include <vector>



// Singleton class used for holding OpenCL objects
class OCL {

 public:
    static OCL* getInstance();

    cl_platform_id m_platform;
    cl_device_id m_device;
    cl_context m_context;

    cl_program m_evalProgram;
    cl_program m_moveProgram;


    cl_command_queue m_queue;
    cl_kernel m_evalKernel;
    
    cl_kernel m_pieceMoveKernel;
    cl_kernel m_wPawnMoveKernel;
    cl_kernel m_bPawnMoveKernel;

    cl_kernel m_pieceCaptureKernel;
    cl_kernel m_wPawnCaptureKernel;
    cl_kernel m_bPawnCaptureKernel;

    int RunEvalKernel(const Board& board);

    std::vector<Move> RunPawnMoveKernel(const Board& board, const bool capture = false);
    std::vector<Move> RunPieceMoveKernel(const Board& b, const bool capture = false);

  private:


    static OCL* m_instance;
    static std::string getSourceDir();
    static uint possible_pawn_moves;
    static uint possible_piece_moves;

    void BuildProgram();
    void SetPieceHostBuffer(unsigned int* pieces, bitboard bb, unsigned int& itr, const unsigned short piece_count);

    OCL();
    OCL(OCL const&);
    void operator=(OCL const&);
    ~OCL();


};  // end class


#endif // OCL_H