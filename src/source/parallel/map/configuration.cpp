#include "parallel/map/configuration.hpp"

::parallel::scale parallel::mapper::configuration::COARSE(4, 4, 4);
::parallel::scale parallel::mapper::configuration::MEDIUM(16, 16, 16);
::parallel::scale parallel::mapper::configuration::FINE(4, 4, 4);

::parallel::scale parallel::mapper::configuration::DIAMETER(2, 2, 2);

organisation::point parallel::mapper::configuration::ORIGIN(50.0f, 50.0f, 50.0f);

parallel::mapper::configuration::configuration()
{
    coarse = COARSE;
    medium = MEDIUM;
    fine = FINE;
    diameter = DIAMETER;
    origin = ORIGIN;
    
    minimum = MINIMUM;
    maximum = MAXIMUM;
}