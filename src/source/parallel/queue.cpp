#include "parallel/queue.hpp"

parallel::queue::queue(device &d)
{
    q = ::cl::sycl::queue(d.get(), sycl::property::queue::in_order{});
}

::cl::sycl::queue& parallel::queue::get_queue(device &d, queue *q)
{
    return q != NULL ? q->get() : queue(d).get();
}