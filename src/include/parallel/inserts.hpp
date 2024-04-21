#include <CL/sycl.hpp>
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/value.hpp"
#include "parameters.h"
#include "schema.h"

#ifndef _PARALLEL_INSERTS
#define _PARALLEL_INSERTS

namespace organisation
{    
    namespace parallel
    {        
        class program;

        class inserts
        {            
            friend class program;

            ::parallel::device *dev;
            ::parallel::queue *queue;

            sycl::float4 *deviceNewPositions;
            sycl::int4 *deviceNewValues;
            int *deviceNewMovementPatternIdx;            
            sycl::int4 *deviceNewClient;

            int *deviceInputData;
            int *deviceInsertsDelay;
            int *deviceInsertsLoops;
            sycl::float4 *deviceInsertsStartingPosition;
            int *deviceInsertsMovementPatternIdx;
            int *deviceInsertsWords;

            sycl::float4 *deviceMovements;
            int *deviceMovementsCounts;

            int *deviceInputIdx;

            int *deviceTotalNewInserts;
            int *hostTotalNewInserts;

            int *hostInputData;

            int *hostInsertsDelay;
            int *hostInsertsLoops;
            sycl::float4 *hostInsertsStartingPosition;
            int *hostInsertsMovementPatternIdx;
            int *hostInsertsWords;

            sycl::float4 *hostMovements;
            int *hostMovementsCounts;

            parameters settings;

            int length;

            bool init;

        public:
            inserts(::parallel::device &dev, 
                    ::parallel::queue *q,
                    parameters &settings) 
            { 
                makeNull(); 
                reset(dev, q, settings); 
            }
            ~inserts() { cleanup(); }

            bool initalised() { return init; }
            void reset(::parallel::device &dev, 
                       ::parallel::queue *q,
                       parameters &settings);

            void restart();
            void clear();
            
            int insert(int epoch, int iteration);

            void set(organisation::data &mappings, inputs::input &source);
            std::vector<value> get();

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