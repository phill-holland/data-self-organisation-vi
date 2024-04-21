#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/parameters.hpp"
#include "parallel/scale.hpp"
#include "point.h"
#include <stdio.h>

#ifndef _PARALLEL_MAPPER_CONFIGURATION
#define _PARALLEL_MAPPER_CONFIGURATION

namespace parallel
{
	namespace mapper
	{
		class configuration
		{
            const static int MINIMUM = 8192;
            const static int MAXIMUM = 8192;

            static ::parallel::scale COARSE;
            static ::parallel::scale MEDIUM;
            static ::parallel::scale FINE;

            static ::parallel::scale DIAMETER;

            static organisation::point ORIGIN;

        public:
            scale coarse, medium, fine;
            scale diameter;
            organisation::point origin;
            int minimum, maximum;

        public:
            configuration();
        };
    };
};

#endif
