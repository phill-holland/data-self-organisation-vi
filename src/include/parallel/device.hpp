#include <CL/sycl.hpp>
#include <vector>
#include <string>

#ifndef _PARALLEL_DEVICE
#define _PARALLEL_DEVICE

namespace parallel
{
    class device
    {
        sycl::device dev;

    public:
        device(int index);

    public:
        sycl::device& get() { return dev; }

    public:
		static std::vector<std::string> enumerate();
		static std::string capabilities();
        
    public:
        std::vector<int> get(int *source, int length);
        std::vector<sycl::float4> get(sycl::float4 *source, int length);
        std::vector<sycl::int4> get(sycl::int4 *source, int length);

    protected:
        sycl::device select(int index);
    };
};

#endif