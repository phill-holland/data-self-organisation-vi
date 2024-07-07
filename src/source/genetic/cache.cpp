#include "genetic/cache.h"
#include <sstream>
#include <functional>
#include <iostream>

std::mt19937_64 organisation::genetic::cache::xyzw::generator(std::random_device{}());
std::mt19937_64 organisation::genetic::cache::cache::generator(std::random_device{}());

void organisation::genetic::cache::xyzw::generate(int max_x, int max_y, int max_z, int min_x, int min_y, int min_z)
{
    x = (std::uniform_int_distribution<int>{min_x, max_x - 1})(generator);
    y = (std::uniform_int_distribution<int>{min_y, max_y - 1})(generator);
    z = (std::uniform_int_distribution<int>{min_z, max_z - 1})(generator);
}

/*
void organisation::genetic::cache::xyzw::generate(std::vector<int> &data, int dimensions)
{
    x = -1; y = -1; z = -1;

    int _dimensions = dimensions;
    if(_dimensions < 1) _dimensions = 1;
    if(_dimensions > 3) _dimensions = 3;
    
    int count = (std::uniform_int_distribution<int>{1, _dimensions})(generator);

    std::vector<int> results;
    for(int j = 0; j < count; ++j)
    {
        int value = 0;
        do
        {
            int idx = (std::uniform_int_distribution<int>{0, (int)(data.size() - 1)})(generator);
            value = data[idx];      
        }while(std::find(results.begin(),results.end(),value) != results.end());

        results.push_back(value);
    }

    int *coordinates[] = { &x, &y, &z };

    for(int j = 0; j < results.size(); ++j)
    {
        *coordinates[j] = results[j];
    }
}
*/
void organisation::genetic::cache::xyzw::generate2(std::vector<int> &data, int dimensions)
{
    x = -1; y = -1; z = -1; w = -1;

    int _dimensions = dimensions;
    if(_dimensions < 1) _dimensions = 1;
    if(_dimensions > 3) _dimensions = 3;
    
    int index = (std::uniform_int_distribution<int>{0, (int)(data.size() - 1)})(generator);
    int count = (std::uniform_int_distribution<int>{1, _dimensions})(generator);

    int *coordinates[] = { &x, &y, &z };
    int dim = 0;

    while((index < data.size())&&(dim<count))
    {
        *coordinates[dim++] = data[index++];
    }    
}

void organisation::genetic::cache::xyzw::mutate(std::vector<int> &data, int dimensions)
{
    auto validate = [this](point &src, int dim)
    {
        const int coordinates[] = { src.x, src.y, src.z };    
        for(int i = 1; i < dim; ++i)
        {
            if((coordinates[i] != -1)&&(coordinates[i-1] == -1)) return false;            
        }

        return true;
    };

    int _dimensions = dimensions;
    if(_dimensions < 1) _dimensions = 1;
    if(_dimensions > 3) _dimensions = 3;
    
    int *coordinates[] = { &x, &y, &z };

    int count = 0;

    do
    {
        count = (std::uniform_int_distribution<int>{0, _dimensions - 1})(generator);          
    }while(!((count == 0)||(count > 0 && *coordinates[count - 1] != -1)));

    int idx = (std::uniform_int_distribution<int>{0, (int)(data.size() - 1)})(generator);
    *coordinates[count] = data[idx];      
}

std::string organisation::genetic::cache::xyzw::serialise()
{
    std::string result("(");

    result += std::to_string(x);
    result += ",";
    result += std::to_string(y);
    result += ",";
    result += std::to_string(z);
    result += ",";
    result += std::to_string(w);
    result += ")";

    return result;
}

void organisation::genetic::cache::xyzw::deserialise(std::string source)
{
    if(source.size() < 2) return;
    if(source.front() != '(') return;
    if(source.back() != ')') return;

    clear();

    int *position[] = { &x, &y, &z, &w };

    std::string temp = source.substr(1, source.size() - 2);
    std::stringstream ss(temp);

    int index = 0;
    std::string str;

    while(std::getline(ss, str, ','))
    {
        if(index < 4)
        {
            int temp = std::atoi(str.c_str());
            *position[index++] = temp;  
        }
    }
}

bool organisation::genetic::cache::cache::set(point value, point position)
{
    xyzw temp_value(value.x,value.y,value.z,-1), temp_position(position.x,position.y,position.z,-2);

    return set(temp_value,temp_position);
}

bool organisation::genetic::cache::cache::set(xyzw value, xyzw position)
{
    if(values.size() < _max_values)
    {
        int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);
        if(points.find(index) == points.end())
        {
            points[index] = position;
            values.push_back(std::tuple<xyzw,xyzw>(value,position));

            return true;
        }
    }
    
    return false;
}

bool organisation::genetic::cache::cache::get(int index, xyzw &value, xyzw &position)
{
    if((index < 0)||(index >= values.size())) return false;

    std::tuple<xyzw,xyzw> temp = values[index];
    
    value = std::get<0>(temp);
    position = std::get<1>(temp);

    return true;
}

std::string organisation::genetic::cache::cache::serialise()
{
    std::string result;

    for(auto &it: values)
    {     
        result += "D " + std::get<0>(it).serialise();
        result += " " + std::get<1>(it).serialise() + "\n";            
    }

    return result;                    
}

void organisation::genetic::cache::cache::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string str;

    xyzw value(-1,-1,-1,-1);
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
            xyzw position;

            position.deserialise(str);
            int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);

            if(points.find(index) == points.end())
            {
                points[index] = position;
                values.push_back(std::tuple<xyzw,xyzw>(value,position));
            }
        }

        ++index;
    };
}

bool organisation::genetic::cache::cache::validate(data &source)
{
    if(values.size() != points.size()) { std::cout << "cache::validate(false): values.size(" << values.size() << ") != points.size(" << points.size() << ")\r\n"; return false; }

    std::unordered_map<int, xyzw> duplicates;

    for(auto &it: values)
    {
        xyzw value = std::get<0>(it);
        int coordinates[] = { value.x, value.y, value.z };

        for(int i = 0; i < 3; ++i)
        {
            if(coordinates[i] != -1)
            {
                if(source.map(coordinates[i]).empty()) { std::cout << "cache::validate(false): map empty [" << coordinates[i] << "]\r\n"; 
                    return false; }
            }
        }

        xyzw position = std::get<1>(it);

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

void organisation::genetic::cache::cache::generate(data &source, inputs::input &epochs)
{
    clear();
                
    std::vector<int> raw = source.outputs(epochs);

    int count = 0;
    if(_max_cache > 0) (std::uniform_int_distribution<int>{0, _max_cache})(generator);
 
    for(int i = 0; i < count; ++i)    
    {
        xyzw position;
        position.generate(_width, _height, _depth);
        int key = (std::uniform_int_distribution<int>{0, 1})(generator);
        // ***
        if(key == 0) position.w = -3;
        else position.w = -4;

// ***
//position.w = -4;
// ***

        // ***
        int index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);
        if(points.find(index) == points.end())
        {
            xyzw value;

// remove blanks only
// generate data (for collision data still) with w == -3
// change program.cpp to not output on .w() == -3 cache collision 
// reduce count, number of cache valuesd generated should be less!!

            //if(_blanks_only) value = xyzw(-1,-1,-1);
            //else 
            value.generate2(raw,_max_cache_dimension);

            points[index] = position;
            values.push_back(std::tuple<xyzw,xyzw>(value,position));
            if(values.size() >= _max_values) return;
        }
    }    
}

bool organisation::genetic::cache::cache::mutate(data &source, inputs::input &epochs)
{
    const int COUNTER = 15;

    if(values.empty()) return false;
    std::vector<int> all = source.outputs(epochs);

    int offset = (std::uniform_int_distribution<int>{0, (int)(values.size() - 1)})(generator);

    int mode = (std::uniform_int_distribution<int>{0, 1})(generator);

    if((mode == 0)&&(!_blanks_only))
    {
        int counter = 0;

        xyzw old = std::get<0>(values[offset]);
        xyzw value = old;

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
        xyzw position;
        int new_index = 0;
        do
        {
            position.generate(_width, _height, _depth);
            new_index = ((_width * _height) * position.z) + ((position.y * _width) + position.x);    
        }while((points.find(new_index) != points.end())&&(counter++ < COUNTER));

        xyzw old = std::get<1>(values[offset]);
        int old_index = ((_width * _height) * old.z) + ((old.y * _width) + old.x);    

        points.erase(old_index);
        points[new_index] = position;
        std::get<1>(values[offset]) = position;
    }

    return true;
}

void organisation::genetic::cache::cache::append(genetic *source, int src_start, int src_end)
{
    cache *s = dynamic_cast<cache*>(source);
    int length = src_end - src_start;

    for(int i = 0; i < length; ++i)
    {
        std::tuple<xyzw,xyzw> temp = s->values[src_start + i];        

        xyzw p1 = std::get<1>(temp);
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

void organisation::genetic::cache::cache::copy(const cache &source)
{
    _width = source._width;
    _height = source._height;
    _depth = source._depth;

    values.assign(source.values.begin(), source.values.end());
    points = source.points;

}

bool organisation::genetic::cache::cache::equals(const cache &source)
{
    if(values.size() != source.values.size()) 
        return false;
    if(points.size() != source.points.size())
        return false;

    for(int i = 0; i < values.size(); ++i)
    {
        std::tuple<xyzw,xyzw> a = values[i];
        std::tuple<xyzw,xyzw> b = source.values[i];

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