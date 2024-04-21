#include "genetic/links.h"
#include "general.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::links::generator(std::random_device{}());

std::string organisation::genetic::links::serialise()
{
    std::string result;

    int index = 0;
    for(auto &it: values)
    {     
        result += "L " + std::to_string(index) + " " + it.serialise() + "\n";
        ++index;
    }

    return result;                    
}

void organisation::genetic::links::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;

    point value(-1,-1,-1);
    int index = 0;
    int offset = 0;

    while(std::getline(ss,str,' '))
    {
        if(index == 0)
        {
            if(str.compare("L")!=0) return;        
        }
        else if(index == 1)
        {          
            offset = std::atoi(str.c_str());  
        }
        else if(index == 2)
        {            
            value.deserialise(str);
            values[offset] = value;
        }

        ++index;
    };
}

bool organisation::genetic::links::validate(data &source)
{    
    if(values.size() != source.maximum() * _max_chain)
    { 
        std::cout << "links::validate(false): values.size(" << values.size() << ") != data.size(" << source.maximum() << ")\r\n"; 
        return false; 
    }

    for(auto &it: values)
    {
        point value = it;
        int coordinates[] = { value.x, value.y, value.z };

        for(int i = 0; i < 3; ++i)
        {
            if(coordinates[i] != -1)
            {
                if(source.map(coordinates[i]).empty()) 
                { 
                    std::cout << "links::validate(false): map empty [" << coordinates[i] << "]\r\n"; 
                    return false; 
                }
            }
        }     
    }
    
    return true;
}

void organisation::genetic::links::generate(data &source, inputs::input &epochs)
{
    clear();
    
    for(int i = 0; i < epochs.size(); ++i)
    {
        inputs::epoch epoch;
        if(epochs.get(epoch,i))
        {
            std::vector<std::string> expected_words = organisation::split(epoch.expected);

            for(auto &it: expected_words)
            {
                int idx = source.map(it);
                if(idx < _max_hash_value)
                {    
                    int coordinates[] = { 0,0,0 };                
                    int offset = 0;
                    int destination = 0;
                    for(auto &jt: expected_words)
                    {
                        coordinates[offset++] = source.map(jt);
                        if(offset >= 3)
                        {
                            if(destination < _max_chain)
                            {
                                values[(idx * _max_chain) + destination] = organisation::point(coordinates[0],coordinates[1],coordinates[2]);
                                offset = 0;
                                ++destination;
                            }
                        }
                    }
                    if(offset == 1) values[(idx * _max_chain) + destination] = organisation::point(coordinates[0],-1,-1);
                    else if(offset == 2) values[(idx * _max_chain) + destination] = organisation::point(coordinates[0],coordinates[1],-1);
                }            
            }
        }
    }
}

bool organisation::genetic::links::mutate(data &source, inputs::input &epochs)
{
    const int COUNTER = 15;

    if(values.empty()) return false;
    std::vector<int> all = source.outputs(epochs);

    int offset = (std::uniform_int_distribution<int>{0, (int)(values.size() - 1)})(generator);

    int counter = 0;

    point old = values[offset];
    point value = old;

    while((old==value)&&(counter++<COUNTER))
    {
        value = old;
        value.mutate(all,_max_cache_dimension);
    }

    if(value == old) return false;

    values[offset] = value;
    
    return true;
}

void organisation::genetic::links::append(genetic *source, int src_start, int src_end)
{
    links *s = dynamic_cast<links*>(source);

    for(int i = src_start; i < src_end; ++i)
    {
        values[i] = s->values[i];
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