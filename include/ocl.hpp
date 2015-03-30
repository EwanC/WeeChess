#ifndef OCL_H
#define OCL_H

#include <CL/cl.h>

// Singleton class used for holding OpenCL objects
class OCL {

 public:
    static OCL* getInstance();

    cl_platform_id m_platform;
    cl_device_id m_device;
    cl_context m_context;

    // cl_program;
    // cl_kernel;
    // cl_command_queue;


  private:


    static OCL* m_instance;

    OCL();
    OCL(OCL const&);
    void operator=(OCL const&);
    ~OCL();


};  // end class


#endif // OCL_H