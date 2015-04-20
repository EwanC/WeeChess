#ifndef OCL_H
#define OCL_H

#include <CL/cl.h>
#include "board.hpp"

// Singleton class used for holding OpenCL objects
class OCL {

 public:
    static OCL* getInstance();

    cl_platform_id m_platform;
    cl_device_id m_device;
    cl_context m_context;

    cl_program m_program;
    cl_command_queue m_queue;
    cl_kernel m_evalKernel;

    int RunEvalKernel(const Board& board);


  private:


    static OCL* m_instance;

    void BuildProgram();

    OCL();
    OCL(OCL const&);
    void operator=(OCL const&);
    ~OCL();


};  // end class


#endif // OCL_H