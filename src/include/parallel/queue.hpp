#include <CL/sycl.hpp>
#include <vector>
#include "device.hpp"

#ifndef _PARALLEL_QUEUE
#define _PARALLEL_QUEUE

namespace parallel
{
    class queue
    {
        ::cl::sycl::queue q;

    public:
        queue(device &d);

    public:
        ::cl::sycl::queue& get() { return q; }

    public:
        static ::cl::sycl::queue& get_queue(device &d, queue *q);
    };
};

#endif