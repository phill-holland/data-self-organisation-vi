#include "genetic/movement.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::movements::movement::generator(std::random_device{}());

std::string organisation::genetic::movements::movement::serialise()
{
    std::string result;

    for(auto &it: directions)
    {
        if(result.size() > 0) result += "|";
        result += it.serialise();
    }

    return result;
}

void organisation::genetic::movements::movement::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;
    
    while(std::getline(ss,str,'|'))
    {
        organisation::vector temp;
        temp.deserialise(str);
        directions.push_back(temp);
    };
}

bool organisation::genetic::movements::movement::validate(data &source)
{
    if(directions.empty()) 
    { 
        std::cout << "movement::validate(false): directions is empty\r\n"; 
        return false; 
    }

    for(auto &direction: directions)
    {
        if((direction.x < -1)||(direction.x > 1)
            ||(direction.y < -1)||(direction.y > 1)
            ||(direction.z < -1)||(direction.z > 1))
        {
            std::cout << "movement::validate(false): direction out of bounds (" << direction.x << "," << direction.y << "," << direction.z << ")\r\n"; 
            return false;
        }
    }

    return true;
}

void organisation::genetic::movements::movement::generate(data &source, inputs::input &epochs)
{
    int n = (std::uniform_int_distribution<int>{_min_movements, _max_movements})(generator);

    for(int i = 0; i < n; ++i)
    {
        int value = (std::uniform_int_distribution<int>{0, 26})(generator);
        vector v1;
        if(v1.decode(value))
        {            
            directions.push_back(v1);
        }
    }
}

bool organisation::genetic::movements::movement::mutate(data &source, inputs::input &epochs)
{
    if(directions.empty()) return false;

    const int COUNTER = 15;

    int n = 0; 
    int value = 0, old = 0;
    int counter = 0;

    do
    {        
        n = (std::uniform_int_distribution<int>{0, (int)(directions.size() - 1)})(generator);
        value = (std::uniform_int_distribution<int>{0, 26})(generator);

        old = directions[n].encode();
        vector v1;
        v1.decode(value);
        directions[n] = v1;   
    } while((old == value)&&(counter++<COUNTER));    
    
    if(old==value) return false;

    return true;
}

void organisation::genetic::movements::movement::append(genetic *source, int src_start, int src_end)
{
    movement *s = dynamic_cast<movement*>(source);
    int length = src_end - src_start;
    
    for(int i = 0; i < length; ++i)
    {
        vector current_direction = s->directions[src_start + i];
     
        directions.push_back(current_direction);
    }   
}

void organisation::genetic::movements::movement::copy(const movement &source)
{
    directions.assign(source.directions.begin(), source.directions.end());
}