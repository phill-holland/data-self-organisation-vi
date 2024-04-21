#include <string>
#include <vector>
#include <random>
#include "input.h"

#ifndef _ORGANISATION_DICTIONARY
#define _ORGANISATION_DICTIONARY

namespace organisation
{    
    class dictionary
    {
        static std::mt19937_64 generator;

        std::vector<std::string> words;

    public:
        dictionary();

        std::vector<std::string> get() const;
        std::string random(int length = 0, std::vector<std::string> excluded= {}) const;
        void push_back(inputs::input &epochs);
    };
};

#endif