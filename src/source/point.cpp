#include "point.h"
#include <sstream>
#include <functional>

std::mt19937_64 organisation::point::generator(std::random_device{}());

void organisation::point::generate(int max_x, int max_y, int max_z, int min_x, int min_y, int min_z)
{
    x = (std::uniform_int_distribution<int>{min_x, max_x - 1})(generator);
    y = (std::uniform_int_distribution<int>{min_y, max_y - 1})(generator);
    z = (std::uniform_int_distribution<int>{min_z, max_z - 1})(generator);
}

void organisation::point::generate(std::vector<int> &data, int dimensions)
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

void organisation::point::generate2(std::vector<int> &data, int dimensions)
{
    x = -1; y = -1; z = -1;

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
    
/*
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
    */
}

void organisation::point::mutate(std::vector<int> &data, int dimensions)
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
/*
    do
    {
        int idx = (std::uniform_int_distribution<int>{-1, (int)(data.size() - 1)})(generator);
        if(idx >= 0) *coordinates[count] = data[idx];      
        else *coordinates[count] = -1;
    }while(!validate(*this,_dimensions));    
*/
}

void organisation::point::mutate2(std::vector<int> &data, int dimensions)
{
    int _dimensions = dimensions;
    if(_dimensions < 1) _dimensions = 1;
    if(_dimensions > 3) _dimensions = 3;
    
    int *coordinates[] = { &x, &y, &z };

    int count = 0;

    do
    {
        count = (std::uniform_int_distribution<int>{0, _dimensions - 1})(generator);          
    }while(!((count == 0)||(count > 0 && *coordinates[count - 1] != -1)));

    int idx = (std::uniform_int_distribution<int>{0, (int)(data.size())})(generator);
    if(idx == data.size()) *coordinates[count] = -1;
    else *coordinates[count] = data[idx];      

    int x1 = -1, y1 = -1, z1 = -1;
    int *output[] = { &x1, &y1, &z1 };

    int j = 0;
    for(int i = 0; i < _dimensions; ++i)
    {
        if(*coordinates[i] != -1) *output[j++] = *coordinates[i];
    }

    for(int i = 0; i < _dimensions; ++i)
    {
        *coordinates[i] = *output[i];
    }
}

std::string organisation::point::serialise()
{
    std::string result("(");

    result += std::to_string(x);
    result += ",";
    result += std::to_string(y);
    result += ",";
    result += std::to_string(z);
    result += ")";

    return result;
}

void organisation::point::deserialise(std::string source)
{
    if(source.size() < 2) return;
    if(source.front() != '(') return;
    if(source.back() != ')') return;

    clear();

    int *position[] = { &x, &y, &z };

    std::string temp = source.substr(1, source.size() - 2);
    std::stringstream ss(temp);

    int index = 0;
    std::string str;

    while(std::getline(ss, str, ','))
    {
        if(index < 3)
        {
            int temp = std::atoi(str.c_str());
            *position[index++] = temp;  
        }
    }
}