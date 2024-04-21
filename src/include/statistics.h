#include <vector>
#include <string>
#include <unordered_map>

#ifndef _ORGANISATION_STATISTICS
#define _ORGANISATION_STATISTICS

namespace organisation
{
    namespace statistics
    {
        class data
        {
        public:            
            int collisions;

        public:
            data(int _collisions = 0)
            {
                collisions = _collisions;
            }
        
        public:
            bool operator==(const ::organisation::statistics::data &src) const
            {
                return src.collisions == collisions;
            }
        };

        class statistic
        {
        public:                        
            std::unordered_map<int, data> epochs;            

        public:
            statistic() { }

        public:
            bool operator==(const ::organisation::statistics::statistic &src) const
            {
                return src.epochs == epochs;
            }
        };
    }; 
};


#endif