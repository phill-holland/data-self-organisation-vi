#include "genetic/insert.h"
#include <iostream>
#include <sstream>
#include <functional>

std::mt19937_64 organisation::genetic::inserts::insert::generator(std::random_device{}());

bool organisation::genetic::inserts::insert::get(std::tuple<int,vector> &result, int idx)
{
    int offset = idx;
    int pattern = 0;
    for(auto &it:values)
    {
        int length = it.movement.size();
        if(offset >= length) offset -= length;
        else
        {
            vector direction = it.movement.directions[offset];
            result = std::tuple<int,vector>(pattern, direction);
            return true;
        }

        ++pattern;
    }

    return false;
}

void organisation::genetic::inserts::insert::generate(data &source, inputs::input &epochs)
{
    clear();

    int length = (std::uniform_int_distribution<int>{_min_movement_patterns, _max_movement_patterns})(generator);

    for(int i = 0; i < length; ++i)
    {
        value temp;

        temp.delay = (std::uniform_int_distribution<int>{_min_insert_delay, _max_insert_delay})(generator);
        temp.words = (std::uniform_int_distribution<int>{_min_insert_words, _max_insert_words})(generator);
        temp.starting.generate(_width,_height,_depth);
        temp.loops = (std::uniform_int_distribution<int>{1, _iterations})(generator);
        
        movements::movement movement(_min_movements, _max_movements);
        movement.generate(source, epochs);

        temp.movement = movement;

        values.push_back(temp);
    }    
}

/*
void organisation::genetic::inserts::insert::generate(data &source)
{
    clear();

    //int length = (std::uniform_int_distribution<int>{_min_movement_patterns, _max_movement_patterns})(generator);

    //for(int i = 0; i < length; ++i)
    //{
        value temp;

        temp.delay = (std::uniform_int_distribution<int>{_min_insert_delay, _max_insert_delay})(generator);
        temp.words = (std::uniform_int_distribution<int>{_min_insert_words, _max_insert_words})(generator);
        temp.starting.generate(_width / 2,_height,_depth);
        
        movements::movement movement(_min_movements, _max_movements);
        movement.generate(source);

        temp.movement = movement;

        values.push_back(temp);

        // ***

        value temp2 = temp;
        temp2.starting.x += (_width / 2);

        for(auto &it: temp2.movement.directions)
        {
            it.x *= -1;
        }

        values.push_back(temp2);
    //}    
}
*/
bool organisation::genetic::inserts::insert::mutate(data &source, inputs::input &epochs)
{    
    if(values.empty()) return false;

    const int COUNTER = 15;

    int offset = 0;
    value val, old;
    int counter = 0;
    
    do
    {
        offset = (std::uniform_int_distribution<int>{0, (int)(values.size() - 1)})(generator);        
        int mode = (std::uniform_int_distribution<int>{0, 4})(generator);

        val = values[offset];

        if(mode == 0)
        {            
            val.delay = (std::uniform_int_distribution<int>{_min_insert_delay, _max_insert_delay})(generator);
        }
        else if(mode == 1)
        {
            val.movement.mutate(source, epochs);
        }
        else if(mode == 2)
        {      
            val.starting.generate(_width,_height,_depth);                     
        }
        else if(mode == 3)
        {
            val.words = (std::uniform_int_distribution<int>{_min_insert_words, _max_insert_words})(generator);
        }
        else if(mode == 4)
        {
            val.loops = (std::uniform_int_distribution<int>{1, _iterations})(generator);
        }
            
        old = values[offset];        
        values[offset] = val;

    }while((old==val)&&(counter++<COUNTER));

    if(old==val) return false;

    return true;   
}

void organisation::genetic::inserts::insert::append(genetic *source, int src_start, int src_end)
{
    insert *s = dynamic_cast<insert*>(source);

    int length = src_end - src_start;  

    std::tuple<int,vector> first;
    if(s->get(first, src_start))
    {
        int previous = std::get<0>(first);
        const value src = s->values[previous];
        value dest(src.delay, src.starting, movements::movement(_min_movements,_max_movements), src.words, src.loops);

        for(int i = 0; i < length; ++i)
        {
            std::tuple<int,vector> data;
            
            if(s->get(data, src_start + i))
            {
                int pattern = std::get<0>(data);
                vector direction = std::get<1>(data);

                if(pattern != previous) 
                {
                    // ***
                    if(values.size() + 1 >= _max_movement_patterns) return;
                    // ***

                    values.push_back(dest);

                    const value src = s->values[pattern];
                    dest = value(src.delay, src.starting, movements::movement(_min_movements,_max_movements), src.words, src.loops);

                    previous = pattern;
                }
            
                dest.movement.directions.push_back(direction);
            }          
        }

        if(dest.movement.size() > 0) 
        {
            if(values.size() + 1 >= _max_movement_patterns) return;
            values.push_back(dest);
        }
    }   
}

std::string organisation::genetic::inserts::insert::serialise()
{
    std::string result;

    for(auto &it: values)
    {
        result += "I " + std::to_string(it.delay);
        result += " " + std::to_string(it.words);
        result += " " + std::to_string(it.loops);
        result += " " + it.starting.serialise();
        result += " " + it.movement.serialise();        
        result += "\n";
    }

    return result;
}

void organisation::genetic::inserts::insert::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;

    value value;
    int index = 0;

    while(std::getline(ss,str,' '))
    {
        
        if(index == 0)
        {
            if(str.compare("I")!=0) return;    
            value.clear();
        }
        else if(index == 1)
        {
            value.delay = std::atoi(str.c_str());         
        }
        else if(index == 2)        
        {
            value.words = std::atoi(str.c_str());
        }
        else if(index == 3)
        {
            value.loops = std::atoi(str.c_str());
        }
        else if(index == 4)
        {
            value.starting.deserialise(str);
        }
        else if(index == 5)
        {
            value.movement.deserialise(str);
            values.push_back(value);            
        }        

        ++index;
    };
}

bool organisation::genetic::inserts::insert::validate(data &source)
{
    if(values.empty()) { std::cout << "insert::validate(false): values is empty\r\n"; return false; }

    /*
    if(values.size() < _min_movement_patterns)
    {
        std::cout << "insert::validate(false): movement out of bounds\r\n"; 
        return false; 
    }
    */

    for(auto &it: values)
    {
        if((it.delay < _min_insert_delay)||(it.delay > _max_insert_delay))
        { 
            std::cout << "insert::validate(false): delay out of bounds (" << it.delay << ")\r\n"; 
            return false; 
        }

        if((it.words < _min_insert_words)||(it.words > _max_insert_words))
        { 
            std::cout << "insert::validate(false): words out of bounds (" << it.words << ")\r\n"; 
            return false; 
        }

        if((it.loops < 1)||(it.words > _iterations))
        { 
            std::cout << "insert::validate(false): loops out of bounds (" << it.loops << ")\r\n"; 
            return false; 
        }

        if(!it.starting.inside(_width,_height,_depth))
        {
            std::cout << "insert::validate(false): invalid starting position\r\n"; 
            return false; 
        }

        if(!it.movement.validate(source)) return false;
    }

    return true;
}

void organisation::genetic::inserts::insert::copy(const insert &source)
{
    values.assign(source.values.begin(), source.values.end());
}

bool organisation::genetic::inserts::insert::equals(const insert &source)
{
    if(values.size() != source.values.size()) 
        return false;

    for(int i = 0; i < values.size(); ++i)
    {
        if(values[i] != source.values[i]) 
            return false;
    }

    return true;
}