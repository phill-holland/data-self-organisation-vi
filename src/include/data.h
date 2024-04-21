#include "point.h"
#include "input.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#ifndef _ORGANISATION_DATA
#define _ORGANISATION_DATA

namespace organisation
{    
    class data
    {
        static std::mt19937_64 generator;

        std::unordered_map<std::string, int> forward;
        std::unordered_map<int, std::string> reverse;

        int max;

    public:
        data() { max = 0; } 
        data(std::vector<std::string> &source) { reset(source); }
        data(const data &source) { copy(source); }

    public:
        void reset(std::vector<std::string> &source) { clear(); add(source); }

        void add(std::vector<std::string> &source);

        void clear()
        {
            forward.clear();
            reverse.clear();
            max = 0;
        }

        int maximum() const { return max; }
        
        std::string map(int value);
        int map(std::string value);
        
        std::string get(std::vector<int> &source);  
        std::vector<int> get(std::string source);

        std::vector<int> all();
        std::vector<int> outputs(inputs::input &epochs);

        point generate(int dimensions);

        std::string serialise();

        void save(std::string filename);
        
    public:
        void copy(const data &source);      
    };
};

#endif