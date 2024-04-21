#include "history/templates/history.h"
#include "history/value.h"
#include <string>
#include <vector>

#ifndef _ORGANISATION_HISTORY_STREAM
#define _ORGANISATION_HISTORY_STREAM

namespace organisation
{
    namespace history
    {
        class stream : public templates::history
        {
            std::vector<value> data;

        public:
            void clear() { data.clear(); }
            size_t size() { return data.size(); }

            bool get(value &destination, int index);
            void push_back(value &source) { data.push_back(source); }

            bool save(std::string filename);
            bool load(std::string filename);
        };
    };
};

#endif
    