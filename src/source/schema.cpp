#include "schema.h"
#include "general.h"
#include <iostream>
#include <tuple>

std::mt19937_64 organisation::schema::generator(std::random_device{}());

void organisation::schema::reset()
{
    init = false; cleanup();

    init = true;
}

void organisation::schema::clear()
{
    prog.clear();
    scores.clear();
}

void organisation::schema::generate(data &source, inputs::input &epochs)
{
    prog.clear();
    prog.generate(source, epochs);
}

bool organisation::schema::get(kdpoint &destination, int minimum, int maximum)
{
    int counter = 0;

    for(auto &it: scores)
    {     
        for(int j = 0; j < it.second.size(); ++j)
        {
            float temp = it.second.get(j);
            temp = (temp * ((float)maximum - minimum)) + minimum;
            if(!destination.set((long)temp, counter)) return false;

            ++counter;
        }        
    }

    return true;
}

std::vector<float> organisation::schema::get()
{
    std::vector<float> result;

    for(auto &it: scores)
    {     
        for(int j = 0; j < it.second.size(); ++j)
        {
            float temp = it.second.get(j);
            result.push_back(temp);
        }        
    }

    return result;
}

float organisation::schema::get(int dimension)
{
    int temp = dimension;
    int index = 0;

    while(temp >= scores[index].size())
    {
        int length = scores[index].size();
        if(length == 0) return 0.0f;
        
        temp -= length;        
        ++index;
    }

    return scores[index].get(temp);
}

float organisation::schema::sum()
{
    float result = 0.0f;
    if(scores.size() <= 0) 
        return 0.0f;

    for(auto &it: scores)
    {
        result += it.second.sum();
    }

    return result / ((float)scores.size());
}

void organisation::schema::compute(std::vector<organisation::compute> values, scores::settings settings)
{
    
    bool penalty = false;
    
    /*
    int i = 0;
    std::string first = "";
    if(values.size() > 0) first = values[0].value;

    for(std::vector<organisation::compute>::iterator it = values.begin(); it != values.end(); ++it)
    {        
        scores[i].compute(*it, settings);
        ++i;

        if(it->value.size() <= 0) penalty = true;
        else if(i > 1)
        {
            if(compare_bow(it->value, first, 4) > 0.6f) 
                penalty = true;
        }        
    }
    */

   bool clear = false;
    for(int i = 0; i < values.size(); ++i)
    {
        scores[i].compute(values[i], settings);

        if(values[i].value.size() <= 0) clear = true;
        else
        {
            for(int j = 0; j < values.size(); ++j)
            {
                if(i != j)
                {
                    if(compare_bow(values[i].value, values[j].value, 4) > 0.8f)//0.6f) 
                    {
                        penalty = true;
                        break;
                    }
                }
            }
        }
    }

    if(penalty)
    {
        int i = 0;
        for(std::vector<organisation::compute>::iterator it = values.begin(); it != values.end(); ++it)
        {
            scores[i].penalty(0.2f);
            ++i;
        }
    }

    if(clear)
    {
        int i = 0;
        for(std::vector<organisation::compute>::iterator it = values.begin(); it != values.end(); ++it)
        {
            scores[i].clear();
            ++i;
        }
    }

}

void organisation::schema::mutate(data &source, inputs::input &epochs)
{
	prog.mutate(source, epochs);
}

void organisation::schema::cross(schema *destination, schema *value)
{   
    destination->prog.cross(prog, value->prog);
}

std::string organisation::schema::run(int epoch, std::string input, std::string expected, data &source)
{		    
    std::string output = prog.run(input, source);
    scores[epoch].compute(organisation::compute(expected, output));
    
    return output;
}

void organisation::schema::copy(const schema &source)
{    
    prog.copy(source.prog);
    scores = source.scores;
}

void organisation::schema::makeNull()
{

}

void organisation::schema::cleanup()
{
    
}