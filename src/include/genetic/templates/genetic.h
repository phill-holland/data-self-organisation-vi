#include "data.h"
#include "input.h"
#include <stddef.h>

#ifndef _ORGANISATION_GENETIC_TEMPLATES_GENETIC
#define _ORGANISATION_GENETIC_TEMPLATES_GENETIC

namespace organisation
{
    namespace templates
    {
        class genetic
        {
        public:
            virtual ~genetic() { }

        public:
            virtual size_t size() = 0;     
            virtual void clear() = 0;
            virtual bool empty() = 0;
            virtual void generate(data &source, inputs::input &epochs) = 0;
            virtual bool mutate(data &source, inputs::input &epochs) = 0;
            virtual void append(genetic *source, int src_start, int src_end) = 0;            
            virtual bool validate(data &source) = 0;

        };
    };
};

#endif