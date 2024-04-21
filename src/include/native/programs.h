#include "templates/programs.h"
#include "parameters.h"
#include "program.h"
#include "schema.h"
#include "data.h"
#include "output.h"
#include "input.h"

#ifndef _ORGANISATION_NATIVE_PROGRAM
#define _ORGANISATION_NATIVE_PROGRAM

namespace organisation
{    
    namespace native
    {
        class programs : public templates::programs
        {
            parameters params;

            schema **schemas;
            outputs::output *results;
            inputs::input input;

            int clients;
            
            bool init;

        private:
            const static int ITERATIONS = 20;
            const static int HOST_BUFFER = 20;
            
        public:
            programs(parameters settings, int clients) { makeNull(); reset(settings, clients); }
            ~programs() { cleanup(); }

            bool initalised() { return init; }
            void reset(parameters settings, int clients);

            void clear();

            void run(organisation::data &mappings);        

            void set(organisation::data &mappings, inputs::input &source);
            std::vector<outputs::output> get(organisation::data &mappings);

        public:
            void copy(::organisation::schema **source, int source_size);
            
        protected:
            void makeNull();
            void cleanup();
        };
    };
};

#endif