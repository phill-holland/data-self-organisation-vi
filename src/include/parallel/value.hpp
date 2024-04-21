#include "point.h"

#ifndef _PARALLEL_VALUE
#define _PARALLEL_VALUE

namespace organisation
{    
    namespace parallel
    {        
        class value
        {
        public:
            point position;
            point data;
            int lifetime;
            int client;

        public:
            value(point _position = point(0,0,0), point _data = point(0,0,0), int _lifetime = 0, int _client = 0)
            {
                position = _position;
                data = _data;
                lifetime = _lifetime;
                client = _client;
            }

        public:
            bool operator==(const ::organisation::parallel::value &src) const
            {
                return src.position == position &&
                        src.data == data &&
                        src.lifetime == lifetime &&
                        src.client == client;
            }
        };
    };
};

#endif