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
            int position;
            int insertOrder;

        public:
            data(std::string _value = "", int _client = 0, int _index = 0, int _position = 0, int _insertOrder = 0)
            {
                value = _value;
                client = _client;
                index = _index;                
                position = _position;
                insertOrder = _insertOrder;
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