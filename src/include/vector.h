#include "math.h"
#include <string>
#include <iostream>

#ifndef _ORGANISATION_VECTOR
#define _ORGANISATION_VECTOR

namespace organisation
{    
    class vector
    {  
    public:
        int x,y,z,w;

    public:
        vector() { clear(); }
        vector(int a, int b, int c) { x = a; y = b; z = c; w = 1; }
        vector(int a, int b, int c, int d) { x = a; y = b; z = c; w = d; }

        void clear() { x = y = z = 0; w = 1; }

        vector inverse()
        {
            return { x * -1, y * -1, z * -1, w };
        }

        vector normalise()
        {
            return { x / w, y / w, z / w, w };
        }

        bool isempty()
        {
            return x == 0 && y == 0 && z == 0;
        }

        bool decode(int index)
        {
            if ((index >= 0) && (index <= 26))
            {
                div_t r = div(index, 9);
                z = (float)r.quot - 1;

                div_t j = div(r.rem, 3);
                y = (float)j.quot - 1;
                x = (float)j.rem - 1;
                w = 0.0f;

                return true;
            }

            return false;
        }

        int encode()
        {
            int tx = (int)roundf(x) + 1;
            int ty = (int)roundf(y) + 1;
            int tz = (int)roundf(z) + 1;

            if ((tx < 0) || (tx > 2)) return 0;
            if ((ty < 0) || (ty > 2)) return 0;
            if ((tz < 0) || (tz > 2)) return 0;

            return (abs(tz) * (3 * 3)) + (abs(ty) * 3) + abs(tx);
        }

        std::string serialise();
        void deserialise(std::string source);

    public:
        bool operator==(const vector &src) const
        {
            return x == src.x && y == src.y && z == src.z;
        }

        bool operator!=(const vector &src) const
        {
            return x != src.x || y != src.y || z != src.z;
        }

        vector operator*(const vector &src) 
        { 
            return vector(y * src.z - z * src.y,z * src.x - x * src.z,x * src.y - y * src.x, 0); 
        }        
        
        vector operator+(const vector &src) 
        { 
            return vector(x + src.x,y + src.y,z + src.z, 0); 
        }

        vector operator-(const vector &src) 
        { 
            return vector(x - src.x,y - src.y,z - src.z, 0); 
        }

        vector operator*(int r)  
        { 
            return vector(x * r,y * r,z, 0); 
        }
        
        vector operator/(int r) 
        { 
            return vector(x / r,y / r,z / r, 0); 
        }
    };
};

#endif


/*
auto is_dominant = [](int *a, int *b) 
	{ 
		const int dimensions = 2;
		bool any = false;
		for (int i = 0; i < dimensions; ++i)
		{
			//Log << a[i];
			// modify kdtree for this condition, between left and right insert
			if(a[i] > b[i]) return false;
			any |= (a[i] < b[i]);
			//if((*epochs)[i]->results.score > (*source.epochs)[i]->results.score) return false;
			//any |= ((*epochs)[i]->results.score < (*source.epochs)[i]->results.score);
		}

		return any;
	};

*/