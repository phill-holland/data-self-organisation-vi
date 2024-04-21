#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/value.hpp"
#include "parameters.h"
#include "schema.h"

#ifndef _PARALLEL_LINKS
#define _PARALLEL_LINKS

namespace organisation
{    
    namespace parallel
    {        
        class program;

        class links
        {            
            friend class program;

            ::parallel::device *dev;
            ::parallel::queue *queue;

            sycl::int4 *deviceLinks;
            sycl::int4 *hostLinks;
            
            parameters settings;

            int length;

            bool init;

        public:
            links(::parallel::device &dev, 
                  ::parallel::queue *q,
                  parameters &settings) 
            { 
                makeNull(); 
                reset(dev, q, settings); 
            }
            ~links() { cleanup(); }

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