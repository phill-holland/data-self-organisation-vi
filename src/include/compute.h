#include "statistics.h"
#include "output.h"
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
        
        //std::vector<std::tuple<int,std::string>> values;
        std::vector<outputs::data> values;

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
                /*
                std::sort(values.begin(), values.end(), [](const std::tuple<int,std::string> &left, const std::tuple<int,std::string> &right) 
                {
                    return std::get<0>(left) < std::get<0>(right);
                });
                */

                std::sort(values.begin(), values.end(), [](const outputs::data &left, const outputs::data &right) 
                {
                    //return std::get<0>(left) < std::get<0>(right);
                    if(left.index == right.index)
                        return left.insertOrder < right.insertOrder;
                    else return left.index < right.index;
                });

                for(auto &it:values)
                {
                    if(value.size() > 0) value += " ";
                    value += it.value;
                    //value += std::get<1>(it);
                }
            }
        }
    };
};

#endif