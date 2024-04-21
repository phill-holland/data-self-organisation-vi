#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>

#ifndef _ORGANISATION_GENERAL
#define _ORGANISATION_GENERAL

namespace organisation
{
    std::vector<std::string> split(std::string source);
        
    float compare_bow(std::string a, std::string b, int max_len = 0);
};

#endif