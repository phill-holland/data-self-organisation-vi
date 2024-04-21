#include <CL/sycl.hpp>
#include <stdio.h>

#ifndef _PARALLEL_SCALE
#define _PARALLEL_SCALE

namespace parallel
{
	using namespace sycl;
	
	class scale
	{
	public:
		int x;
		int y;
		int z;
		int w;

	public:
		scale() { reset(); }
		scale(int x, int y, int z) { reset(x, y, z); }
		scale(const int4 &source) { copy(source); }

		void reset(int x = 0, int y = 0, int z = 0, int w = 0)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

	public:
		void copy(const scale &source)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = source.w;
		}

		void copy(const int4 &source)
		{
			x = source.x();
			y = source.y();
			z = source.z();
			w = source.w();
		}

	public:
		bool operator==(const scale& rhs) const
		{
			return std::tie(x,y,z,w) == 
				   std::tie(rhs.x,rhs.y,rhs.z,rhs.w);
		}

	public:
		scale& operator=(const scale& source)
		{
			this->copy((scale&)source);
			return *this;
		}

		scale& operator=(const int4& source)
		{
			this->copy((int4&)source);
			return *this;
		}

		operator int4() const { return { x, y, z, w }; }
	};
};

#endif