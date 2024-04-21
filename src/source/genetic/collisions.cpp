#include "genetic/collisions.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::collisions::generator(std::random_device{}());

void organisation::genetic::collisions::generate(data &source, inputs::input &epochs)
{
    for(int i = 0; i < size(); ++i)
    {
        int value;
        do
        {
            value = (std::uniform_int_distribution<int>{0, 26})(generator);
        }while((value == 13)||(value == i % 26));
        values[i] = value;
    }
}

bool organisation::genetic::collisions::mutate(data &source, inputs::input &epochs)
{
    const int COUNTER = 15;

    int offset = 0;
    int value = 0, old = 0;
    int counter = 0;   

    do
    {
        offset = (std::uniform_int_distribution<int>{0, (int)(values.size() - 1)})(generator);
        do
        {
            value = (std::uniform_int_distribution<int>{0, 26})(generator);
        }while((value == 13)||(value == offset % 26));

        old = values[offset];
        values[offset] = value;

    }while((old == value)&&(counter++<COUNTER));
 
    if(old==value) return false;

    return true;
}

void organisation::genetic::collisions::append(genetic *source, int src_start, int src_end)
{
    collisions *s = dynamic_cast<collisions*>(source);

    for(int i = src_start; i < src_end; ++i)
    {
        values[i] = s->values[i];
    }
}

std::string organisation::genetic::collisions::serialise()
{
    std::string result;

    int index = 0;
    for(auto &it: values)
    {
        result += "C " + std::to_string(index) + " " + std::to_string(it) + "\n";
        ++index;
    }

    return result;
}

void organisation::genetic::collisions::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;

    int value = 0;
    int index = 0;
    int offset = 0;

    while(std::getline(ss,str,' '))
    {
        if(index == 0)
        {
            if(str.compare("C")!=0) return;
        }
        else if(index == 1)
        {
            offset = std::atoi(str.c_str());
        }
        else if(index == 2)
        {
            int value = std::atoi(str.c_str());
            values[offset] = value;
        }

        ++index;
    };
}

bool organisation::genetic::collisions::validate(data &source)
{
    if(values.size() != length) { std::cout << "collisions::validate(false): values.size() != " << length << " (" << values.size() << ")\r\n"; return false; }
    
    for(auto &it: values)
    {
        if((it < 0)||(it >= 27)) 
        { 
            std::cout << "collisions::validate(false): it < 0 || it > 27\r\n"; 
            return false; 
        }
    }

    return true;
}

bool organisation::genetic::collisions::get(int &result, int idx)
{    
    if((idx < 0)||(idx >= size())) return false;
    result = values[idx];
    return true;
}

bool organisation::genetic::collisions::set(int source, int idx)
{
    if((idx < 0)||(idx >= size())) return false;
    values[idx] = source;
    return true;
}

std::vector<organisation::vector> organisation::genetic::collisions::get(organisation::point value)
{
    std::vector<vector> results;

    int index = value.x * _max_collisions;
    int temp = 0;
    for(int i = 0; i < _max_collisions; ++i)
    {
        if(get(temp, i + index))
        {
            vector v;
            if(v.decode(temp)) results.push_back(v);
        }
    }

    return results;
}

std::vector<organisation::vector> organisation::genetic::collisions::get(organisation::vector direction)
{
    std::vector<vector> results;
    int value = direction.encode();

    for(int i = 0; i < _max_mappings; ++i)
    {
        int temp = 0;        
        if(get(temp,(i * _max_collisions) + value))
        {
            vector v;
            if(v.decode(temp))
            {
                if(!v.isempty()) results.push_back(v);
            }        
        }
    }

    return results;
}

void organisation::genetic::collisions::copy(const collisions &source)
{
    clear();

    for(int i = 0; i < size(); ++i)
    {
        values[i] = source.values[i];
    }
}

bool organisation::genetic::collisions::equals(const collisions &source)
{
    if(values.size() != source.values.size()) 
        return false;

    for(int i = 0; i < size(); ++i)
    {
        if(values[i] != source.values[i])
            return false;
    }

    return true;
}