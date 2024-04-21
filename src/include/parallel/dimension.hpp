#include <CL/sycl.hpp>
#include <stdio.h>

#ifndef _PARALLEL_DIMENSION
#define _PARALLEL_DIMENSION

namespace parallel
{
	using namespace sycl;

	class dimension
	{
	public:
		int x;
		int y;
		int z;
		int w;

	public:
		dimension() { reset(); }
		dimension(int x, int y, int z) { reset(x, y, z); }
		dimension(const int4 &source) { copy(source); }

		void reset(int x = 0, int y = 0, int z = 0, int w = 0)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

	public:
		void copy(const dimension &source)
		{
			x = source.x;
			y = source.y;
			z = source.z;
			w = source.w;
		}

		void copy(const sycl::int4 &source)
		{
			x = source.x();
			y = source.y();
			z = source.z();
			w = source.w();			
		}

	public:
		bool operator==(const dimension& rhs) const
		{
			return std::tie(x,y,z,w) == 
				   std::tie(rhs.x,rhs.y,rhs.z,rhs.w);
		}

	public:
		dimension& operator=(const dimension& source)
		{
			this->copy((dimension&)source);
			return *this;
		}

		dimension& operator=(const sycl::int4& source)
		{
			this->copy((int4&)source);
			return *this;
		}

		operator sycl::int4() const { return { x, y, z, w }; }
	};
};

#endif