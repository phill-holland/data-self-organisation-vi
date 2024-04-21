#include "program.h"
#include "data.h"
#include "score.h"
#include "kdpoint.h"
#include "statistics.h"
#include "compute.h"
#include "parameters.h"
#include "input.h"
#include <string>
#include <random>
#include <vector>
#include <tuple>
#include <unordered_map>

#ifndef _ORGANISATION_SCHEMA
#define _ORGANISATION_SCHEMA

namespace organisation
{
    class schema
    {
        static std::mt19937_64 generator;
        
        bool init;

    public:
        program prog;
        std::unordered_map<int, scores::score> scores;

    public:
        schema(parameters &settings) : prog(settings) { makeNull(); reset(); }        
        ~schema() { cleanup(); }

    public:
        void clear();
        
        bool initalised() { return init; }
        void reset();
        
        void generate(data &source, inputs::input &epochs);
    
        bool get(kdpoint &destination, int minimum, int maximum);

        std::vector<float> get();
        float get(int dimension);

        float sum();
            
        void compute(std::vector<organisation::compute> values, scores::settings settings);

    public:        
        void cross(schema *destination, schema *value);
        void mutate(data &source, inputs::input &epochs);
        bool validate(data &source) { return prog.validate(source); }

    public:
        std::string run(int epoch, std::string input, std::string expected, data &source);

    public:
        void copy(const schema &source);
        bool equals(const schema &source)
        {
            return prog.equals(source.prog) && (scores == source.scores);
        }

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif