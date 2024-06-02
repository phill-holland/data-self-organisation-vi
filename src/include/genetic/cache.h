#include "genetic/templates/genetic.h"
#include "genetic/templates/serialiser.h"
#include "point.h"
#include "parameters.h"
#include "input.h"
#include <vector>
#include <unordered_map>
#include <random>

#ifndef _ORGANISATION_GENETIC_CACHE
#define _ORGANISATION_GENETIC_CACHE

namespace organisation
{
    namespace genetic
    {
        namespace cache
        {
            class xyzw
            {
                static std::mt19937_64 generator;

            public:
                int x,y,z,w;

            public:
                xyzw(int _x = 0, int _y = 0, int _z = 0, int _w = 0)        
                {
                    x = _x;
                    y = _y;
                    z = _z;
                    w = _w;
                }    

                void clear()
                {
                    x = y = z = w = 0;
                }

                void generate(int max_x, int max_y, int max_z, int min_x = 0, int min_y = 0, int min_z = 0);

                //void generate(std::vector<int> &data, int dimensions = 3);
                void generate2(std::vector<int> &data, int dimensions = 3);
                void mutate(std::vector<int> &data, int dimensions = 3);

                std::string serialise();
                void deserialise(std::string source);
            
                bool inside(int w, int h, int d)
                {
                    if((x < 0)||(x >= w)) return false;
                    if((y < 0)||(y >= h)) return false;
                    if((z < 0)||(z >= d)) return false;

                    return true;
                }

            public:
                bool operator==(const xyzw &src) const
                {
                    return x == src.x && y == src.y && z == src.z && w == src.w;
                }

                bool operator!=(const xyzw &src) const
                {
                    return x != src.x || y != src.y || z != src.z;
                }
            };

            class cache : public templates::genetic, public templates::serialiser
            {
                static std::mt19937_64 generator;

                int _width, _height, _depth;

                int _max_cache;
                int _max_values;
                int _max_cache_dimension;
                bool _blanks_only;

            //public:
                std::vector<std::tuple<xyzw,xyzw>> values;
                std::unordered_map<int,xyzw> points;

            public:
                cache(parameters &settings) 
                { 
                    _width = settings.width;
                    _height = settings.height;
                    _depth = settings.depth;

                    _max_cache = settings.max_cache;
                    _max_values = settings.max_values;
                    _max_cache_dimension = settings.max_cache_dimension;

                    _blanks_only = settings.cache_blanks_only;
                }

            public:
                size_t size() { return values.size(); }
                void clear() 
                { 
                    values.clear(); 
                    points.clear();
                }

                bool empty()
                {
                    return values.empty() || points.empty();
                }

                bool set(point value, point position);
                bool set(xyzw value, xyzw position);
                bool get(int index, xyzw &value, xyzw &position);

                std::string serialise();
                void deserialise(std::string source);

                bool validate(data &source);

            public:
                void generate(data &source, inputs::input &epochs);
                bool mutate(data &source, inputs::input &epochs);
                void append(genetic *source, int src_start, int src_end);

            public:
                void copy(const cache &source);
                bool equals(const cache &source);
            };
        };
    };
};

#endif