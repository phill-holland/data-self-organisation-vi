#include "data.h"
#include "general.h"
#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>

std::mt19937_64 organisation::data::generator(std::random_device{}());

void organisation::data::add(std::vector<std::string> &source)
{
    for(std::vector<std::string>::iterator it = source.begin(); it != source.end(); ++it) 
	{
        if(forward.find(*it) == forward.end())
        {
            forward[*it] = max;
            reverse[max] = *it;
            ++max;
        }
    }
}

std::string organisation::data::map(int value)
{
    std::string result;

    if(reverse.find(value) != reverse.end())
    {
        result = reverse[value];
    }

    return result;
}
        
int organisation::data::map(std::string value)
{
    int result = 0;

    if(forward.find(value) != forward.end())
    {
        result = forward[value];
    }

    return result;
}

std::string organisation::data::get(std::vector<int> &source)
{
    std::string result;

    if(source.size() == 0) return result;
    
    if(source.begin() != source.end()) 
        result = map(*source.begin());

    for(std::vector<int>::iterator it = source.begin() + 1; it < source.end(); ++it)
    {   
        std::string temp = map(*it);
        if(temp.size() == 0)
        {
            std::cout << "HMM " << (*it) << "\r\n";
        }     
        else result += std::string(" ") + temp;//map(*it);
    }

    return result;
}

std::vector<int> organisation::data::get(std::string source)
{
    std::vector<int> result;
    if(source.size() <= 0) return result;

    auto strings = organisation::split(source);
    for(auto &it: strings)
    {
        result.push_back(map(it));   
    }

    return result;
}

std::vector<int> organisation::data::all()
{
    std::vector<int> result;

    for(auto &r: reverse)
    {
        result.push_back(r.first);
    }

    return result;
}

std::vector<int> organisation::data::outputs(inputs::input &epochs)
{
    std::vector<int> result;

    for(int i = 0; i < epochs.size(); ++i)
    {
        inputs::epoch temp;
        if(epochs.get(temp, i))
        {
            auto strings = organisation::split(temp.expected);
            for(auto &it: strings)
            {
                result.push_back(map(it));
            }   
        }
    }    

    return result;
}

organisation::point organisation::data::generate(int dimensions)
{
    point result(-1,-1,-1);

    int _dimensions = dimensions;
    if(_dimensions < 1) _dimensions = 1;
    if(_dimensions > 3) _dimensions = 3;
    
    std::vector<int> raw = all();

    int count = (std::uniform_int_distribution<int>{1, _dimensions})(generator);

    std::vector<int> results;
    for(int j = 0; j < count; ++j)
    {
        int value = 0;
        do
        {
            int idx = (std::uniform_int_distribution<int>{0, (int)(raw.size() - 1)})(generator);
            value = raw[idx];
        }while(std::find(results.begin(),results.end(),value) != results.end());

        results.push_back(value);
    }

    int *coordinates[] = { &result.x, &result.y, &result.z };

    for(int j = 0; j < results.size(); ++j)
    {
        *coordinates[j] = results[j];
    }

    return result;
}

std::string organisation::data::serialise()
{
    std::string result;
    std::vector<int> raw = all();

    std::sort(raw.begin(),raw.end());

    for(auto &it:raw)
    {
        result += std::to_string(it) + " " + map(it) + "\n";
    }

    return result;
}

void organisation::data::save(std::string filename)
{
    std::fstream output(filename, std::fstream::out | std::fstream::binary);

    if(output.is_open())
    {
        std::string data = serialise();
        output.write(data.c_str(), data.size());
    }

    output.close();
}

void organisation::data::copy(const data &source)
{
    clear();

    for(auto &f: source.forward)
    {
        forward[f.first] = f.second;
    }

    for(auto &r: source.reverse)
    {
        reverse[r.first] = r.second;
    }

    max = source.max;
}