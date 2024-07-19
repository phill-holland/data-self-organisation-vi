#include "dictionary.h"
#include "general.h"
#include <algorithm>

std::mt19937_64 organisation::dictionary::generator(std::random_device{}());

organisation::dictionary::dictionary()
{    
}

std::vector<std::string> organisation::dictionary::get() const 
{ 
    return words; 
}

std::string organisation::dictionary::random(int length, std::vector<std::string> excluded) const
{
    int total = length;
    if(total == 0)
        total = (std::uniform_int_distribution<int>{ 2, 5 })(generator);

    std::string result;

    result = words[(std::uniform_int_distribution<int>{ 0, (int)(words.size() - 1) })(generator)];
    for(int i = 1; i < total; ++i)
    {
        result += " ";
        std::string temp;
        do
        {
            temp = words[(std::uniform_int_distribution<int>{ 0, (int)(words.size() - 1) })(generator)];
        }while(std::find(excluded.begin(),excluded.end(),temp)!=excluded.end());

        result += temp;
    }

    return result;
}

void organisation::dictionary::push_back(inputs::input &epochs)
{
    for(int i = 0; i < epochs.size(); ++i)
    {
        inputs::epoch temp;
        if(epochs.get(temp, i))
        {
            auto values = organisation::split(temp.input);
            for(auto &it: values)
            {
                if(std::find(words.begin(), words.end(), it) == words.end())
                    words.push_back(it);
            }   

            auto expected = organisation::split(temp.expected);
            for(auto &it: expected)
            {
                if(std::find(words.begin(), words.end(), it) == words.end())
                    words.push_back(it);
            }   
        }
    }    
}