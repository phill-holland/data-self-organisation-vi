#include "point.h"

#ifndef _ORGANISATION_HISTORY
#define _ORGANISATION_HISTORY

namespace organisation
{
    namespace history
    {
        class value
        {
        public:
            point position;            
            point data;
            point next;
            point collision;

            int sequence;
            int client;
            int epoch;
            int colType;
            int movementIdx;
            int movementPatternIdx;

            bool stationary;

        public:
            value(point _position = point(0,0,0), point _data = point(0,0,0))
            {
                position = _position;
                data = _data;
                next = point(0,0,0);
                collision = point(0,0,0);
                sequence = 0;
                client = 0;
                epoch = 0;
                colType = 0;
                
                movementIdx = 0;
                movementPatternIdx = 0;
                
                stationary = false;
            }

            void clear()
            {
                position = point(0,0,0);
                data = point(0,0,0);
                next = point(0,0,0);
                collision = point(0,0,0);

                sequence = 0;
                client = 0;
                epoch = 0;
                colType = 0;

                movementIdx = 0;
                movementPatternIdx = 0;


                stationary = false;
            }

            std::string serialise();
        };
    };
};

#endif
    