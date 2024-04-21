#include "point.h"
#include "vector.h"

#ifndef _ORGANISATION_POSITION
#define _ORGANISATION_POSITION

namespace organisation
{
    class position
    {
    public:
        point current;
        vector direction;
        int value;
        int index;
        int time;
        bool output;

        std::vector<vector> collisions;
        
    public:
        position(int _value = -1, int _index = 0, int _time = 0)
        {
            value = _value;
            index = _index;
            time = _time;

            output = false;
        }

    public:
        bool isempty() { return value == -1; }
    };
};

#endif