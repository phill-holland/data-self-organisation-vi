#include <CL/sycl.hpp>
#include "parallel/map/configuration.hpp"
#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/parameters.hpp"
#include "parallel/scale.hpp"
#include "point.h"
#include <stdio.h>

#ifndef _PARALLEL_MAPPER_MAP
#define _PARALLEL_MAPPER_MAP

namespace parallel
{
	namespace mapper
	{
		using namespace sycl;
		
		class map
		{
			::parallel::device *dev;

			float4 coarseScale;
			int4 coarseDimension;

			float4 mediumScale;
			int4 mediumDimension;

			float4 fineScale;
			int4 fineBuckets;
			int totalBuckets;

			int4 clientTotals;

			int *deviceCoarseScaleMap;
			int *deviceMediumScaleMap;

			int *hostPinnedBucketIndices;
			int *hostBucketLengths;

			int *deviceBucketIndices;
			int *deviceBucketLengths;

			float4 **devicePositions;
			int4 **deviceClients;

			float4 **deviceHostPositions;
			int4 **deviceHostClients;

			::parallel::parameters client, global;

			int value;

			bool init;

		public:
			map(::parallel::device &dev,
				::parallel::parameters c, ::parallel::parameters g,
				configuration &settings)
				{
					makeNull();
					reset(dev, c, g, settings);
				}
			~map() { cleanup(); }

			bool initalised() { return init; }

			void reset(::parallel::device &dev,
					::parallel::parameters c, ::parallel::parameters g,
					configuration &settings);

			void clear(::parallel::queue *q = NULL);

			void build(float4 *points, int4 *clients, const int length, ::parallel::queue *q = NULL);

			void search(float4 *search, int4 *clients, int *result, const int length, const bool self, int *collided, int index, ::parallel::queue *q = NULL);		
			void search(float4 *search, int4 *clients, int2 *result, const int length, const bool self = false, const bool symetrical = true, const bool inverse = false, int *collided = NULL, int index = 0, ::parallel::queue *q = NULL);

		protected:
			std::string outputarb(int *source, int length);

		protected:
			void makeNull();
			void cleanup();
		};
	};
};

#endif