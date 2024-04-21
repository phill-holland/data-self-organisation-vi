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
        class cache : public templates::genetic, public templates::serialiser
        {
            static std::mt19937_64 generator;

            int _width, _height, _depth;

            int _max_cache;
            int _max_values;
            int _max_cache_dimension;

        public:
            std::vector<std::tuple<point,point>> values;
            std::unordered_map<int,point> points;

        public:
            cache(parameters &settings) 
            { 
                _width = settings.width;
                _height = settings.height;
                _depth = settings.depth;

                _max_cache = settings.max_cache;
                _max_values = settings.max_values;
                _max_cache_dimension = settings.max_cache_dimension;
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

#endif