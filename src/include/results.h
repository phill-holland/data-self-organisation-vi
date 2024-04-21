#ifndef _ORGANISATION_POPULATION_RESULTS
#define _ORGANISATION_POPULATION_RESULTS

namespace organisation
{
    namespace populations
    {        
        class results
        {
        public:
            float average;
            float best;
            int index;

        public:
            results() { average = 0.0f; best = 0.0f; index = 0; }
        };
    };
};

#endif