#include "templates/programs.h"
#include "schema.h"
#include "data.h"
#include "fifo.h"
#include "schemas.h"
#include "parameters.h"
#include "region.h"
#include "input.h"
#include "results.h"
#include <random>
#include <atomic>

#ifndef _ORGANISATION_POPULATION
#define _ORGANISATION_POPULATION

namespace organisation
{
    namespace populations
    {        
        class population
        {
            static const int minimum = 100, maximum = 10000;

            static std::mt19937_64 generator;

            organisation::schemas *schemas;            
            organisation::schema **intermediateA, **intermediateB, **intermediateC;

            templates::programs *programs;

            int dimensions;

            parameters settings;

            bool init;

        public:
            population(templates::programs *programs, parameters &params) { makeNull(); reset(programs, params); }
            ~population() { cleanup(); }

            bool initalised() { return init; }
            void reset(templates::programs *programs, parameters &params);

            void clear();
                    
            organisation::schema go(int &count, int iterations = 0);

            void generate();

            bool set(organisation::schema &source, int index)
            {
                return schemas->set(source, index);
            }
                        
            bool get(organisation::schema &destination, int index)
            {
                organisation::schema *temp = schemas->get(index);
                if(temp == NULL) return false;
                
                destination.copy(*temp);

                return true;
            }

        protected:
            bool get(schema &destination, region r);
            bool set(schema &source, region r);
            
        protected:
            schema *best(region r);
            schema *worst(region r);

        protected:
            void pull(organisation::schema **buffer, region r);
            void push(organisation::schema **buffer, region r);
            void fill(organisation::schema **destination, region r);

        protected:
            results execute(organisation::schema **buffer);
            
            void validate(organisation::schema **buffer);
            void validate2(organisation::schema **buffer);
            
            void save(organisation::schema **buffer);
            void load(organisation::schema **buffer);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif