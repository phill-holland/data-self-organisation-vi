#include "statistics.h"
#include <string>
#include <tuple>
#include <algorithm>

#ifndef _ORGANISATION_COMPUTE
#define _ORGANISATION_COMPUTE

namespace organisation
{
    class compute
    {
    public:
        int max_words;
        int max_collisions;
        
        std::vector<std::tuple<int,std::string>> values;

        std::string expected, value;
        statistics::data stats;
        
    public:
        compute(std::string _expected = "", std::string _value = "", statistics::data _statistics = { }) 
        { 
            expected = _expected;
            value = _value;
            stats = _statistics;
        }

    public:
        void compile()
        {
            value.clear();

            if(values.size() > 0)
            {
                std::sort(values.begin(), values.end(), [](const std::tuple<int,std::string> &left, const std::tuple<int,std::string> &right) 
                {
                    return std::get<0>(left) < std::get<0>(right);
                });

                for(auto &it:values)
                {
                    if(value.size() > 0) value += " ";
                    value += std::get<1>(it);
                }
            }
        }
    };
};

#endif