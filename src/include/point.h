#include "vector.h"
#include <random>
#include <string>
#include <vector>

#ifndef _ORGANISATION_POINT
#define _ORGANISATION_POINT

namespace organisation
{
    class point 
    {
        static std::mt19937_64 generator;

    public:    
        int x,y,z;

    public:
        point(int _x = 0, int _y = 0, int _z = 0)        
        {
            x = _x;
            y = _y;
            z = _z;
        }    

        void clear()
        {
            x = y = z = 0;
        }

    public:
        void generate(int max_x, int max_y, int max_z, int min_x = 0, int min_y = 0, int min_z = 0);

        void generate(std::vector<int> &data, int dimensions = 3);
        void generate2(std::vector<int> &data, int dimensions = 3);

        void mutate(std::vector<int> &data, int dimensions = 3);
        void mutate2(std::vector<int> &data, int dimensions = 3);

        point min(const point &src)
        {
            point result;

            result.x = x > src.x ? src.x : x;
            result.y = y > src.y ? src.y : y;
            result.z = z > src.z ? src.z : z;

            return result;
        }

        bool inside(int w, int h, int d)
        {
            if((x < 0)||(x >= w)) return false;
            if((y < 0)||(y >= h)) return false;
            if((z < 0)||(z >= d)) return false;

            return true;
        }
        
        std::string serialise();
        void deserialise(std::string source);

    public:
        bool operator==(const point &src) const
        {
            return x == src.x && y == src.y && z == src.z;
        }

        bool operator!=(const point &src) const
        {
            return x != src.x || y != src.y || z != src.z;
        }

        point operator+(const point &src) 
        { 
            return point(x + src.x,y + src.y,z + src.z); 
        }   

        point operator+(const vector &src) 
        { 
            return point(x + src.x,y + src.y,z + src.z); 
        }   

        point operator-(const vector &src) 
        { 
            return point(x - src.x,y - src.y,z - src.z); 
        }   

        point operator/(int r) 
        { 
            return point(x / r,y / r,z / r); 
        }     
    };
};

#endif