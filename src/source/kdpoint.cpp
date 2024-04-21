#include "kdpoint.h"
#include <limits.h>
#include <string>

void organisation::kdpoint::reset(long dimensions)
{
    init = false; cleanup();

    this->dimensions = dimensions;

    identity = 0L;

    values.reserve(dimensions);
    min.reserve(dimensions);
    max.reserve(dimensions);

    for(long i=0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }

    init = true;
}

void organisation::kdpoint::clear()
{
    for(long i=0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }
}

void organisation::kdpoint::set(long *points, unsigned long length)
{
    long min = length;
    if(min == 0L) min = dimensions;
    if(min > dimensions) min = dimensions;

    for(long i = 0L; i < min; ++i)
    {
        values[i] = points[i];
    }
}

bool organisation::kdpoint::set(long value, unsigned long dimension)
{
    if(dimension >= dimensions) return false;
    if(dimension < 0L) return false;

    values[dimension] = value;

    return true;
}

void organisation::kdpoint::set(long value)
{
    for(long i = 0L; i < dimensions; ++i)
    {
        values[i] = 0L;
        min[i] = LONG_MIN;
        max[i] = LONG_MAX;
    }
}

long organisation::kdpoint::get(unsigned long dimension)
{
    if(dimension >= dimensions) return -1L;
    if(dimension < 0L) return -1L;

    return values[dimension];
}

bool organisation::kdpoint::dominates(const kdpoint &source)
{
    bool any = false;
    for (int i = 0; i < dimensions; ++i)
    {
        if(source.values[i] > values[i]) return false;
        any |= (source.values[i] < values[i]);
    }

    return any;
}

void organisation::kdpoint::copy(const kdpoint &source)
{
    if (dimensions <= source.dimensions)
    {
        long _min = dimensions;
        if(source.dimensions < _min) _min = source.dimensions;

        for(long i=0L; i < _min; ++i)
        {
            values[i] = source.values[i];
            min[i] = source.min[i];
            max[i] = source.max[i];
        }
    
        dimensions = source.dimensions;
        identity = source.identity;
    }
}

bool organisation::kdpoint::equals(const kdpoint &source)
{
    if (dimensions < source.dimensions) return false;
    
    long min = dimensions;
    if(source.dimensions < min) min = source.dimensions;

    for(long i=0L; i < min; ++i)
    {
        if(values[i] != source.values[i]) return false;
    }
    
    return true;
}

bool organisation::kdpoint::inside(const kdpoint &min, const kdpoint &max)
{
    if(min.dimensions != dimensions) return false;
    if(max.dimensions != dimensions) return false;

    for(long i=0L; i < dimensions; ++i)
    {
        if(values[i] < min.values[i]) return false;
        if(values[i] > max.values[i]) return false;
    }

    return true;
}

bool organisation::kdpoint::issame(long value)
{
    for(long i=0L; i < dimensions; ++i)
    {
        if(values[i] != value) return false;
    }

    return true;
}

void organisation::kdpoint::makeNull()
{
    
}

void organisation::kdpoint::cleanup()
{

}
