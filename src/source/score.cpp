#include "score.h"
#include <iostream>
#include <tuple>
#include <sstream>

void organisation::scores::score::clear()
{
	scores.clear();
}

// first half relative distance from word in front
// second half, relative distance away from words "primary place"
// last value, scoare rating on sentence length

bool organisation::scores::score::compute(organisation::compute value, settings params)
{
	auto _words = [](std::string source)
    {
        std::stringstream stream(source);  
        std::string word;

        int index = 0;
        while (stream >> word) 
        {
			++index;
        };

        return index;
    };

    auto _split = [](std::string source)
    {
        std::vector<std::tuple<std::string,int>> result;

        std::stringstream stream(source);  
        std::string word;

        int index = 0;
        while (stream >> word) 
        {
            result.push_back(std::tuple<std::string,int>(word,index));
			++index;
        };

        return result;
    };

	auto _occurances = [](std::string to_find, std::string value) 
	{ 
		std::stringstream stream(value);
		std::string word;

		int result = 0;
	
		while(stream >> word)
		{
			if(to_find.compare(word) == 0) ++result;
		}

		return result;
	};

	auto _closest = [](std::tuple<std::string,int> to_find, std::string value, int alphabet_length)
	{		
		std::stringstream stream(value);
		std::string word;

		std::string find = std::get<0>(to_find);
		int position = std::get<1>(to_find);

		int distance = alphabet_length + 1;
		int result = -1;

		int count = 0;

		while(stream >> word)
		{
			if(find.compare(word) == 0)
			{
				int temp = abs(count - position);
				if(temp < distance) 
				{
					distance = temp;
					result = count;
				}
			}
			++count;
		}

		return result;
	};

	auto _distance = [](float f1, float f2, float offset, float max)
	{
		float distance = (float)std::abs(f1 - f2) + offset;

		if(distance > max) distance = max;
		if((f1 == -1)||(f2 == -1)) distance = max;
		float result = ((distance / max) * -1.0f) + 1.0f;
		
		if(result < 0.0f) result = 0.0f;
		if(result > 1.0f) result = 1.0f;

		return result;
	};

	clear();

	std::vector<std::tuple<std::string,int>> alphabet = _split(value.expected);
		
	int score_len = (alphabet.size() * 2) + 1;
	for(int i = 0; i < score_len + 1; ++i) set(0.0f, i);

    if(value.value.size() == 0) return true;
	
	const int MAX_WORDS = params.max_words;
	const int MAX_COLLISIONS = params.max_collisions;

	bool valid = true;
	
	if(MAX_COLLISIONS == 0) 
	{
		if(!set(1.0f, score_len)) valid = false;
	}
	else
	{
		int collisions = value.stats.collisions;
		if(collisions > MAX_COLLISIONS) collisions = MAX_COLLISIONS;
		if(!set(((float)collisions) / ((float)MAX_COLLISIONS), score_len)) valid = false;

		if((params.optimise_for_collisions==true)&&(collisions == 0)) return true;
	}
 	
	const int alphabet_len = alphabet.size();		
	float max = (float)MAX_WORDS;
	if(((float)(alphabet_len - 1))>max) max = (float)(alphabet_len - 1);

	float len_score = compute_comparative_length_score(value.expected, value.value);
	if(!set(len_score, score_len - 1)) valid = false;
		
	int index = 0;
	for(auto &it : alphabet)
	{            
		int f1 = _closest(it, value.value, alphabet_len);
		int f3 = std::get<1>(it);
					
		if(f1 >= f3) 
		{
			float distance = _distance(f1, f3, 0.0f, max); 
			if(!set(distance, index)) valid = false;
		}
		else if(f1 < 0)
		{
			if(!set(0.0f, index)) valid = false;
		}
		else if(!set(0.1f, index)) valid = false;

		++index;
	}

	index = 0;
	for(std::vector<std::tuple<std::string,int>>::iterator it = alphabet.begin(); it != (alphabet.end() - 1); ++it)
	{			
		std::tuple<std::string,int> a1 = *it;
		std::tuple<std::string,int> a2 = *(it + 1);

		int f1 = _closest(a1, value.value, alphabet_len);
		int f2 = _closest(a2, value.value, alphabet_len);			
		
		if(f1 < f2) 
		{
			if(!set(_distance(f1, f2, -1.0f, max),index + alphabet_len)) valid = false;		
		}
		else set(0.0f, index+alphabet_len);

		++index;
	}

	std::tuple<std::string,int> last = *(alphabet.end() - 1);
	int f1 = _closest(last, value.value, alphabet_len);
	int f2 = _words(value.value);

	if(f1 < f2) 
	{
		if(!set(_distance(f1, f2, -1.0f, max),alphabet_len + alphabet_len - 1)) valid = false;
	}
	else set(0.0f,alphabet_len + alphabet_len - 1);
	
	
	return valid;
}

float organisation::scores::score::sum()
{    
	if(scores.size() <= 0) return 0.0f;
	float result = 0.0f;

	for(auto &b :scores)
    {
		result += b.second;
    }
    
// THIS IS WRONG, if scores not populated 
    return result / ((float)scores.size());
}

bool organisation::scores::score::set(float value, int index)
{
	if(index < 0) return false;

	scores[index] = value;

	return true;
}

float organisation::scores::score::get(int index)
{
	if((index < 0)||(index >= scores.size())) return 0.0f;

	return scores[index];
}

void organisation::scores::score::penalty(float multiple)
{
	for(auto &it: scores)
	{
		it.second *= multiple;
	}
}

void organisation::scores::score::copy(const score &source)
{
	clear();

	for(auto &a: source.scores)
    {
        scores[a.first] = a.second;
    }
}

float organisation::scores::score::compute_comparative_length_score(std::string expected, std::string value)
{
	auto _words = [](std::string source)
    {
        std::stringstream stream(source);  
        std::string word;

        int index = 0;
        while (stream >> word) 
        {
			++index;
        };

        return index;
    };

	const int MAX_WORD_COUNT_DELTA = 10;

	int l1 = _words(expected);
	int l2 = _words(value);

	const float fib[] = { 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 89.0f, 144.0f };
	
	int delta = abs(l2 - l1);
	if(delta > MAX_WORD_COUNT_DELTA) delta = MAX_WORD_COUNT_DELTA;

	return (1.0f / fib[delta]);
}