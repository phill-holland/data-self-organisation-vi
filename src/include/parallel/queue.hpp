#include <sycl/sycl.hpp>
#include <vector>
#include "device.hpp"

#ifndef _PARALLEL_QUEUE
#define _PARALLEL_QUEUE

namespace parallel
{
    class queue
    {
        ::sycl::queue q;

    public:
        queue(device &d);

    public:
        ::sycl::queue& get() { return q; }

    public:
        static ::sycl::queue& get_queue(device &d, queue *q);
    };
};

#endif