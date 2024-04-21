#include <array>
#include <atomic>
#include <iostream>

#ifndef _CORE_QUEUE_FIFO
#define _CORE_QUEUE_FIFO

namespace core
{
	namespace queue
	{
		template <class X, long Y> class fifo
		{
			bool init;

			std::array<X, Y> items;
			std::atomic<int> lpwrite;
			std::atomic<int> lpread;
			
		public:
			std::atomic<int> elements;

		public:
			fifo() { makeNull(); reset(); }
			~fifo() { cleanup(); }

			bool initalised() { return init; }
			void reset();

			unsigned long entries()
			{
				int r = lpread.load(std::memory_order_relaxed);
				int w = lpwrite.load(std::memory_order_relaxed);

				if (w < r) return (Y - r) + w;

				return w - r;
			}

			bool isfull()
			{
				int r = lpread.load(std::memory_order_relaxed);
				int w = lpwrite.load(std::memory_order_relaxed);

				return (w - 1) == r;
			}

			bool isempty()
			{
				int r = lpread.load(std::memory_order_relaxed);
				int w = lpwrite.load(std::memory_order_relaxed);

				return (r == w);
			}

			long size() { return Y; }

			void empty()
			{
				do
				{
					lpwrite.store(0);
					lpread.store(0);
					elements.store(0);
				} while ((lpwrite != 0) || (lpread != 0));
			}

			bool get(X &destination);
			bool set(X &source);

		private:
			void cleanup() { }
			void makeNull() { }
		};

		template <class X, long Y> void fifo<X, Y>::reset()
		{
			init = false; cleanup();

			empty();

			init = true;
		}

		template <class X, long Y> inline bool fifo<X, Y>::get(X &destination)
		{
			if(lpread == lpwrite) return false;
			if(elements == 0) return false;

			int a = lpread.load();
			int b = a + 1;
			if (b >= Y) b = 0;

			if (lpread.compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
			{
				elements.fetch_sub(1, std::memory_order_relaxed);
				destination = X(items[a]);
				
				return true;
			}

			return false;
		}

		template <class X, long Y> inline bool fifo<X, Y>::set(X &source)
		{
			int a = lpwrite.load();			
			int b = a + 1;
			if (b >= Y) b = 0;
			if (b == lpread) return false;

			if(lpwrite.compare_exchange_weak(a, b, std::memory_order_release, std::memory_order_relaxed))
			{		
				items[a] = source;							
				elements.fetch_add(1, std::memory_order_release);
				
				return true;
			}

			return false;
		}
	};
};

#endif