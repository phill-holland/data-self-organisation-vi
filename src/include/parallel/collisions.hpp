#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/value.hpp"
#include "parameters.h"
#include "schema.h"

#ifndef _PARALLEL_COLLISIONS
#define _PARALLEL_COLLISIONS

namespace organisation
{    
    namespace parallel
    {        
        class program;

        class collisions
        {            
            friend class program;

            ::parallel::device *dev;
            ::parallel::queue *queue;

            sycl::float4 *deviceCollisions;
            sycl::float4 *hostCollisions;
            
            parameters settings;

            int length;

            bool init;

        public:
            collisions(::parallel::device &dev, 
                       ::parallel::queue *q,
                       parameters &settings) 
            { 
                makeNull(); 
                reset(dev, q, settings); 
            }
            ~collisions() { cleanup(); }

            bool initalised() { return init; }
            void reset(::parallel::device &dev, 
                       ::parallel::queue *q,
                       parameters &settings);

            void clear();

        public:
            void copy(::organisation::schema **source, int source_size);
            void into(::organisation::schema **destination, int destination_size);

        protected:
            void outputarb(int *source, int length);
            void outputarb(sycl::float4 *source, int length);

        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif