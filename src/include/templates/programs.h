#include "data.h"
#include "output.h"
#include "schema.h"
#include "input.h"
#include "statistics.h"
#include <stddef.h>
#include <vector>

#ifndef _ORGANISATION_TEMPLATES_PROGRAMS
#define _ORGANISATION_TEMPLATES_PROGRAMS

namespace organisation
{
    namespace templates
    {
        class programs
        {
        public:
            virtual ~programs() { }

        public:
            virtual void clear() = 0;
            virtual void run(organisation::data &mappings) = 0;

            virtual void set(organisation::data &mappings, inputs::input &source) = 0;
            virtual std::vector<outputs::output> get(organisation::data &mappings) = 0;
            virtual std::vector<organisation::statistics::statistic> statistics() = 0;

            virtual void copy(::organisation::schema **source, int source_size) = 0;
            virtual void into(::organisation::schema **destination, int destination_size) = 0;
        };
    };
};

#endif