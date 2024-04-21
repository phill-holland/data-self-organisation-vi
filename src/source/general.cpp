#include "general.h"

inline std::vector<std::string> organisation::split(std::string source)
{
    std::vector<std::string> result;
    std::string temp; 

    for(auto &ch: source)
    {
        if((ch != ' ')&&(ch != 10)&&(ch != 13))
        {
            temp += ch;
        }
        else
        {
            if(temp.size() > 0)
            {
                result.push_back(temp);
                temp.clear();
            }
        }
    }

    if(temp.size() > 0) result.push_back(temp);
    
    return result;    
}

float organisation::compare_bow(std::string a, std::string b, int max_len)
{
    std::unordered_map<std::string, int> mappings;

    std::vector<std::string> a1 = split(a);
    std::vector<std::string> b1 = split(b);

    if(a1.size() == 0) return 0.0f;
    if(b1.size() == 0) return 0.0f;

    if(max_len > 0)
    {
        if(a1.size() > max_len) a1.resize(max_len);
        if(b1.size() > max_len) b1.resize(max_len);
    }

    std::vector<int> a2;
    std::vector<int> b2;

    float len1 = 0.0f, len2 = 0.0f;
    int index = 0;

    std::unordered_map<int,int> a_counts, b_counts;

    for(auto &it: a1)
    {
        if(mappings.find(it) == mappings.end())
        {
            a_counts[index] = 1;
            mappings[it] = index++;                
        }
        else
        {
            int temp = mappings[it];
            a_counts[temp] += 1;
        }
    }

    a2.resize(index);
    for(auto &it: a_counts)
    {
        a2[it.first] = it.second;
        len1 += (float)(it.second * it.second);
    }

    len1 = sqrtf(len1);

    for(auto &it: b1)
    {
        if(mappings.find(it) == mappings.end())
        {
            b_counts[index] = 1;
            mappings[it] = index++;                
        }
        else
        {
            int temp = mappings[it];
            b_counts[temp] += 1;
        }
    }

    b2.resize(index);
    for(auto &it: b_counts)
    {
        b2[it.first] = it.second;
        len2 += (float)(it.second * it.second);
    }

    len2 = sqrtf(len2);

    int length = a2.size();
    if(b2.size() < length) length = b2.size();

    float dot = 0.0f;
    for(int i = 0; i < length; ++i)
    {
        dot += (a2[i] * b2[i]);
    }

    return dot / (len1 * len2);
}