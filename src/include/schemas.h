#include "schema.h"
#include "data.h"
#include "parameters.h"
#include "input.h"
#include <random>
#include <atomic>
#include <vector>

#ifndef _ORGANISATION_SCHEMAS
#define _ORGANISATION_SCHEMAS

namespace organisation
{
    class schemas
    {
        std::vector<schema*> data;
    
    private:
        int length;
        bool init;
        
    public:
        schemas(parameters &settings) { makeNull(); reset(settings); }
        ~schemas() { cleanup(); }

        bool initalised() { return init; }
        void reset(parameters &settings);

        bool clear();

        schema *get(int index)
        {
            if((index < 0)||(index >= length)) return NULL;
            return data[index];
        }

        bool set(schema &source, int index)
        {
            if((index < 0)||(index >= length)) return false;
            data[index]->copy(source);

            return true;
        }

        bool generate(organisation::data &source, inputs::input &epochs);

    protected:
        void makeNull();
        void cleanup();
    };
};

#endif