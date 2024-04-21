#include "genetic/templates/genetic.h"
#include "genetic/templates/serialiser.h"
#include "point.h"
#include "parameters.h"
#include "input.h"
#include <vector>
#include <unordered_map>
#include <random>

#ifndef _ORGANISATION_GENETIC_LINKS
#define _ORGANISATION_GENETIC_LINKS

namespace organisation
{
    namespace genetic
    {
        class links : public templates::genetic, public templates::serialiser
        {
            static std::mt19937_64 generator;
            
            std::vector<point> values;            

            int _max_cache_dimension;
            int _max_chain;
            int _max_hash_value;

        public:
            links(parameters &settings) 
            { 
                _max_cache_dimension = settings.max_cache_dimension;
                _max_hash_value = settings.mappings.maximum();
                _max_chain = settings.max_chain;
                int length = _max_chain * _max_hash_value;
                values.resize(length);
                clear();
            }

        public:
            size_t size() { return values.size(); }

            void clear() 
            { 
                for(auto &it:values)
                {
                    it = organisation::point(-1,-1,-1);
                }
            }

            bool empty()
            {
                return false;
            }

            std::string serialise();
            void deserialise(std::string source);

            bool validate(data &source);

        public:
            void generate(data &source, inputs::input &epochs);
            bool mutate(data &source, inputs::input &epochs);
            void append(genetic *source, int src_start, int src_end);

        public:
            bool get(organisation::point &result, int idx);
            bool set(organisation::point source, int idx);

        public:
            void copy(const links &source);
            bool equals(const links &source);
        };
    };
};

#endif