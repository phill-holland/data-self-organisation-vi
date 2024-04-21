#include "vector.h"
#include <sstream>
#include <functional>

std::string organisation::vector::serialise()
{
    std::string result("(");

    result += std::to_string(x);
    result += ",";
    result += std::to_string(y);
    result += ",";
    result += std::to_string(z);
    result += ",";
    result += std::to_string(w);
    result += ")";

    return result;
}

void organisation::vector::deserialise(std::string source)
{
    if(source.size() < 2) return;
    if(source.front() != '(') return;
    if(source.back() != ')') return;

    clear();

    int *position[] = { &x, &y, &z, &w };

    std::string temp = source.substr(1, source.size() - 2);
    std::stringstream ss(temp);

    int index = 0;
    std::string str;

    while(std::getline(ss, str, ','))
    {
        if(index < 4)
        {
            int temp = std::atoi(str.c_str());
            *position[index++] = temp;  
        }
    }
}