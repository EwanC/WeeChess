#include "ocl.hpp"
#include <iostream>

OCL* OCL::m_instance = 0; // Singleton instance

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
        std::cout << "Couldn't locate an OCL platform\n";
        exit(1);
    }

    /* Access a device */
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 1, &m_device, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_CPU, 1, &m_device, NULL);
        if (err != CL_DEVICE_NOT_FOUND)
            std::cout << "OPENCL CPU Found\n";
    }
    else {
        std::cout << "OPENCL GPU Found\n";
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
}

OCL::~OCL()
{
    /* Deallocate resources */

    clReleaseContext(m_context);
    clReleaseDevice(m_device);
}
