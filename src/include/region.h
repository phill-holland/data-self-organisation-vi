#ifndef _ORGANISATION_POPULATION_REGION
#define _ORGANISATION_POPULATION_REGION

namespace organisation
{
    namespace populations
    {        
        class region
        {
        public:
            int start, end;

        public:
            region(int _start = 0, int _end = 0)
            {
                start = _start;
                end = _end;
            }
        };
    };
};

#endif