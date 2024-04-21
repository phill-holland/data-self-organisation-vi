#include "genetic/templates/genetic.h"
#include "genetic/templates/serialiser.h"
#include "data.h"
#include "point.h"
#include "parameters.h"
#include "movement.h"
#include <vector>
#include <random>
#include <string>

#ifndef _ORGANISATION_GENETIC_INSERT
#define _ORGANISATION_GENETIC_INSERT

namespace organisation
{
    namespace genetic
    {
        namespace inserts
        {
            class value
            {
            public:
                int delay;
                point starting;
                movements::movement movement;
                int words;
                int loops;

            public:
                value(int _delay = 0, point _starting = point(0,0,0), movements::movement _movement = movements::movement(), int _words = 1, int _loops = 1)
                {
                    delay = _delay;
                    starting = _starting;
                    movement = _movement;
                    words = _words;
                    loops = _loops;
                }

                void clear()
                {
                    delay = 0;
                    starting = point(0,0,0);
                    movement.clear();
                    words = 1;
                    loops = 1;
                }

            public:
                bool operator==(const value &src) const
                {
                    return delay == src.delay &&
                           starting == src.starting &&
                           movement == src.movement &&
                           words == src.words && 
                           loops == src.loops;
                }

                bool operator!=(const value &src) const
                {
                    return delay != src.delay ||
                           starting != src.starting ||
                           movement != src.movement || 
                           words != src.words || 
                           loops != src.loops;

                }
            };

            class insert : public templates::genetic, public templates::serialiser
            {
                static std::mt19937_64 generator;
                                     
                int _width, _height, _depth;                
                
                int _min_insert_delay, _max_insert_delay;
                int _min_movements, _max_movements;
                int _min_movement_patterns, _max_movement_patterns;
                int _min_insert_words, _max_insert_words;
                int _iterations;

            public:
                std::vector<value> values;

            public:
                insert(parameters &settings) 
                { 
                    _width = settings.width;
                    _height = settings.height;
                    _depth = settings.depth;

                    _min_insert_delay = settings.min_insert_delay;
                    _max_insert_delay = settings.max_insert_delay;

                    _min_movements = settings.min_movements;
                    _max_movements = settings.max_movements;

                    _min_movement_patterns = settings.min_movement_patterns;
                    _max_movement_patterns = settings.max_movement_patterns;

                    _min_insert_words = settings.min_insert_words;
                    _max_insert_words = settings.max_insert_words;

                    _iterations = settings.iterations;
                }

            public:
                size_t size() 
                { 
                    size_t result = 0;

                    for(auto&it:values) 
                    {
                        result += it.movement.size();
                    }

                    return result;
                }

                void clear() 
                {
                    values.clear();
                }

                bool empty() { return values.empty(); }
                
                bool get(std::tuple<int,vector> &result, int idx);

                void generate(data &source, inputs::input &epochs);
                bool mutate(data &source, inputs::input &epochs);
                void append(genetic *source, int src_start, int src_end);

                std::string serialise();
                void deserialise(std::string source);

                bool validate(data &source);
                                
            public:
                void copy(const insert &source);
                bool equals(const insert &source);
            };
        };
    };
};

#endif