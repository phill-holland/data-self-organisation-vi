#include "genetic/links.h"
#include "general.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::links::generator(std::random_device{}());

void organisation::genetic::links::seed(std::vector<int> source)
{
    for(auto &it:source)
    {
        int index = (_max_chain * it);        
        set(organisation::point(it,-1,-1), index);
    }
}

bool organisation::genetic::links::get(organisation::point &result, int idx)
{    
    if((idx < 0)||(idx >= size())) return false;
    result = values[idx];
    return true;
}

bool organisation::genetic::links::set(organisation::point source, int idx)
{
    if((idx < 0)||(idx >= size())) return false;
    values[idx] = source;
    return true;
}

void organisation::genetic::links::copy(const links &source)
{
    clear();

    for(int i = 0; i < size(); ++i)
    {
        values[i] = source.values[i];
    }
}

bool organisation::genetic::links::equals(const links &source)
{
    if(values.size() != source.values.size()) 
        return false;

    for(int i = 0; i < values.size(); ++i)
    {
        point a = values[i];
        point b = source.values[i];

        if(a != b) 
            return false;
    }

    return true;
}