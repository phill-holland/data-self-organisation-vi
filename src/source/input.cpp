#include "input.h"
#include "general.h"

std::vector<std::tuple<std::string,std::string>> organisation::inputs::input::combine(std::vector<outputs::data> output)
{
    std::vector<std::tuple<std::string,std::string>> result(values.size());

        int i = 0;
        for(auto &it: values)
        {            
            std::tuple<std::string,std::string> temp(it.expected,output.at(i).value);
            result[i] = temp;
            ++i;
        }

        return result;
}

int organisation::inputs::input::dimensions()
{
    int result = 0;
    for(auto &it: values)
    {
        std::vector<std::string> t = split(it.expected);        
        result += (t.size() * 2) + 1;
    }

    return result;
}