#include <exception>
#include <string>

#ifndef _ORGANISATION_EXCEPTIONS
#define _ORGANISATION_EXCEPTIONS

namespace organisation
{
    namespace exceptions
    {
        class MaxInputDataExceededException : public std::exception 
        {
            static const char message[];

        public:
            char *what() 
            {
                return (char*)message;
            }
        };
    };
}

#endif