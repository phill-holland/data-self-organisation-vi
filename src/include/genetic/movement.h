#include "genetic/templates/genetic.h"
#include "genetic/templates/serialiser.h"
#include "point.h"
#include "vector.h"
#include "data.h"
#include "input.h"
#include <vector>
#include <tuple>
#include <random>

#ifndef _ORGANISATION_GENETIC_MOVEMENT
#define _ORGANISATION_GENETIC_MOVEMENT

namespace organisation
{
    namespace genetic
    {
        namespace movements
        {
            class movement : public templates::genetic, public templates::serialiser
            {
                static std::mt19937_64 generator;

                int _min_movements, _max_movements;

            public:
                std::vector<organisation::vector> directions;                

            public:
                movement(int min_movements = 0, int max_movements = 0)
                {
                    _min_movements = min_movements; 
                    _max_movements = max_movements;                    
                }

            public:
                size_t size() { return directions.size(); }

                void clear() 
                {
                    directions.clear();
                }

                bool empty() 
                {
                    return directions.empty();
                }

                int next(int index)
                {
                    if(index + 1 < directions.size()) return index + 1;
                    return 0;
                }

                std::string serialise();
                void deserialise(std::string source);

                bool validate(data &source);
                
            public:
                void generate(data &source, inputs::input &epochs);
                bool mutate(data &source, inputs::input &epochs);
                void append(genetic *source, int src_start, int src_end);

            public:
                void copy(const movement &source);
                bool equals(const movement &source) const
                {
                    if(directions.size() != source.directions.size()) 
                        return false;

                    for(int i = 0; i < directions.size(); ++i)
                    {
                        if(directions[i] != source.directions[i]) 
                            return false;
                    }

                    return true;
                }

            public:
                bool operator==(const movement &src) const
                {
                    return equals(src);                    
                }

                bool operator!=(const movement &src) const
                {
                    return !equals(src);
                }
            };
        };
    };
};

#endif