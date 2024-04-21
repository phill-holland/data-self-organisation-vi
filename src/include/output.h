#include <vector>
#include <string>

#ifndef _ORGANISATION_OUTPUT
#define _ORGANISATION_OUTPUT

namespace organisation
{
    namespace outputs
    {
        class data
        {
        public:
            std::string value;
            int client;
            int index;

        public:
            data(std::string _value = "", int _client = 0, int _index = 0)
            {
                value = _value;
                client = _client;
                index = _index;
            }
        };

        class output
        {
        public:
            std::vector<data> values;

        public:
            void clear()
            {
                values.clear();
            }
        };
    };
};


#endif