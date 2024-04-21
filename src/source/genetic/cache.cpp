#include "genetic/cache.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::cache::generator(std::random_device{}());

bool organisation::genetic::cache::set(point value, point position)
{
    if(values.size() < _max_values)
    {
        int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);
        if(points.find(index) == points.end())
        {
            points[index] = position;
            values.push_back(std::tuple<point,point>(value,position));

            return true;
        }
    }
    
    return false;
}

std::string organisation::genetic::cache::serialise()
{
    std::string result;

    for(auto &it: values)
    {     
        result += "D " + std::get<0>(it).serialise();
        result += " " + std::get<1>(it).serialise() + "\n";            
    }

    return result;                    
}

void organisation::genetic::cache::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;

    point value(-1,-1,-1);
    int index = 0;

    while(std::getline(ss,str,' '))
    {
        if(index == 0)
        {
            if(str.compare("D")!=0) return;
        }
        else if(index == 1)
        {            
            value.deserialise(str);            
        }
        else if(index == 2)
        {
            organisation::point position;

            position.deserialise(str);
            int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);

            if(points.find(index) == points.end())
            {
                points[index] = position;
                values.push_back(std::tuple<point,point>(value,position));
            }
        }

        ++index;
    };
}

bool organisation::genetic::cache::validate(data &source)
{
    if(values.size() != points.size()) { std::cout << "cache::validate(false): values.size(" << values.size() << ") != points.size(" << points.size() << ")\r\n"; return false; }

    std::unordered_map<int, point> duplicates;

    for(auto &it: values)
    {
        point value = std::get<0>(it);
        int coordinates[] = { value.x, value.y, value.z };

        for(int i = 0; i < 3; ++i)
        {
            if(coordinates[i] != -1)
            {
                if(source.map(coordinates[i]).empty()) { std::cout << "cache::validate(false): map empty [" << coordinates[i] << "]\r\n"; 
                    return false; }
            }
        }

        point position = std::get<1>(it);

        if(!position.inside(_width, _height, _depth))
        {
            std::cout << "cache::validate(false): invalid position\r\n"; 
            return false; 
        }

        int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);
        if(duplicates.find(index) == duplicates.end())
            duplicates[index] = position;
        else
        {
            std::cout << "cache::validate(false): duplicate position\r\n"; 
            return false;  
        }

        if(points.find(index) == points.end()) { std::cout << "cache::validate(false): point(index)\r\n"; return false; }
        if(points[index] != position) { std::cout << "cache::validate(false): invalid position\r\n"; return false; }
    }

    return true;
}

void organisation::genetic::cache::generate(data &source, inputs::input &epochs)
{
    clear();
                
    std::vector<int> raw = source.outputs(epochs);

    int count = (std::uniform_int_distribution<int>{0, _max_cache})(generator);
 
    for(int i = 0; i < count; ++i)    
    {
        point position;
        position.generate(_width, _height, _depth);
        int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);
        if(points.find(index) == points.end())
        {
            point value;
            value.generate2(raw,_max_cache_dimension);

            points[index] = position;
            values.push_back(std::tuple<point,point>(value,position));
            if(values.size() >= _max_values) return;
        }
    }    
}

bool organisation::genetic::cache::mutate(data &source, inputs::input &epochs)
{
    const int COUNTER = 15;

    if(values.empty()) return false;
    std::vector<int> all = source.outputs(epochs);

    int offset = (std::uniform_int_distribution<int>{0, (int)(values.size() - 1)})(generator);

    int mode = (std::uniform_int_distribution<int>{0, 1})(generator);

    if(mode == 0)
    {
        int counter = 0;

        point old = std::get<0>(values[offset]);
        point value = old;

        while((old==value)&&(counter++<COUNTER))
        {
            value = old;
            value.mutate(all,_max_cache_dimension);
        }

        if(value == old) return false;

        std::get<0>(values[offset]) = value;
    }
    else
    {
        int counter = 0;
        point position;
        int new_index = 0;
        do
        {
            position.generate(_width, _height, _depth);
            new_index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);    
        }while((points.find(new_index) != points.end())&&(counter++ < COUNTER));

        point old = std::get<1>(values[offset]);
        int old_index = ((_width * _height) * old.z) + ((old.y * _width) + old.x);    

        points.erase(old_index);
        points[new_index] = position;
        std::get<1>(values[offset]) = position;
    }

    return true;
}

void organisation::genetic::cache::append(genetic *source, int src_start, int src_end)
{
    cache *s = dynamic_cast<cache*>(source);
    int length = src_end - src_start;

    for(int i = 0; i < length; ++i)
    {
        std::tuple<point,point> temp = s->values[src_start + i];        

        point p1 = std::get<1>(temp);
        int index = ((_width * _height) * p1.z) + ((p1.y * _width) + p1.x);
        if(points.find(index) == points.end())
        {
            if(size() < _max_cache)
            {
                values.push_back(temp);
                points[index] = p1;            
            }
        }
    }
}

void organisation::genetic::cache::copy(const cache &source)
{
    _width = source._width;
    _height = source._height;
    _depth = source._depth;

    values.assign(source.values.begin(), source.values.end());
    points = source.points;

}

bool organisation::genetic::cache::equals(const cache &source)
{
    if(values.size() != source.values.size()) 
        return false;
    if(points.size() != source.points.size())
        return false;

    for(int i = 0; i < values.size(); ++i)
    {
        std::tuple<point,point> a = values[i];
        std::tuple<point,point> b = source.values[i];

        if(a != b) 
            return false;
    }

    for(auto &it: points)
    {
        if(source.points.find(it.first) == source.points.end()) 
            return false;
        if(!(source.points.at(it.first) == it.second)) 
            return false;        
    }

    return true;
}