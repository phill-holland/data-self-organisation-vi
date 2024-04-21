#include <CL/sycl.hpp>
#include "parallel/dimension.hpp"
#include <string>
#include <stdio.h>
#include <vector>

#ifndef _PARALLEL_PARAMETERS
#define _PARALLEL_PARAMETERS

namespace parallel
{
	class parameters
	{
	public:
		int length;

		::parallel::dimension dimensions;

	public:
		parameters(int _W = 100, int _H = 100, int _D = 100, int _N = 0)
		{ 			
			reset(_W, _H, _D, _N);
		}

		parameters(const parameters &source) { copy(source); }

	public:
		void reset(int _W = 100, int _H = 100, int _D = 100, int _N = 0)
		{
			length = _N;

			dimensions.x = _W;
			dimensions.y = _H;
			dimensions.z = _D;
			
			dimensions.w = 0;
		}
		        
	public:
		void copy(const parameters &source)
		{
			length = source.length;

			dimensions = { source.dimensions.x, source.dimensions.y, source.dimensions.z, 0 };
		}

	public:
		bool operator==(const parameters& rhs) const
		{
			return std::tie(dimensions,length) == 
				   std::tie(rhs.dimensions,rhs.length);
		}

	public:
		parameters& operator=(const parameters& source)
		{
			this->copy((parameters&)source);
			return *this;
		}
	};
};

#endif