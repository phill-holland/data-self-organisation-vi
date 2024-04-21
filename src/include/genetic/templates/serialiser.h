#include <stddef.h>
#include <string>

#ifndef _ORGANISATION_GENETIC_TEMPLATES_SERIALISER
#define _ORGANISATION_GENETIC_TEMPLATES_SERIALISER

namespace organisation
{
    namespace templates
    {
        class serialiser
        {
        public:
            virtual ~serialiser() { }

        public:
            virtual std::string serialise() = 0;
            virtual void deserialise(std::string source) = 0;
        };
    };
};

#endif