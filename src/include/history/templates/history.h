#include "history/value.h"
#include <vector>

#ifndef _ORGANISATION_TEMPLATES_HISTORY
#define _ORGANISATION_TEMPLATES_HISTORY

namespace organisation
{
    namespace history
    {
        namespace templates
        {
            class history
            {
            public:
                virtual ~history() { }

            public:
                virtual void clear() = 0;
                virtual size_t size() = 0;

                virtual bool get(value &destination, int index) = 0;
                virtual void push_back(value &source) = 0;

                virtual bool save(std::string filename) = 0;
            };
        };
    };
};

#endif