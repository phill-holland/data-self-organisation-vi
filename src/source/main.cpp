#include "population.h"
#include "data.h"
#include "general.h"
#include <iostream>
#include <string.h>

#include "fifo.h"
#include "schema.h"
#include "vector.h"
#include "input.h"
#include "output.h"
#include "dictionary.h"

#include "templates/programs.h"

#include "history/stream.h"

#include "parallel/device.hpp"
#include "parallel/queue.hpp"
#include "parallel/program.hpp"

using namespace std;

const int width = 6, height = 6, depth = 6;
const int device_idx = 0;
const int generations = 2000;

organisation::parameters get_parameters()
{
    organisation::parameters parameters(width, height, depth);

    parameters.dim_clients = organisation::point(10,10,10);
    parameters.iterations = 20;
    parameters.max_values = 100;
    parameters.max_cache = parameters.max_values;
        
    parameters.population = parameters.clients() * 4;

    parameters.output_stationary_only = true;
    
    parameters.width = width;
    parameters.height = height;
    parameters.depth = depth;

    parameters.min_movement_patterns = 2;
    parameters.max_movement_patterns = 6;
    parameters.max_insert_delay = 5;

    parameters.scores.max_collisions = 2;
    parameters.scores.optimise_for_collisions = true;

    parameters.max_cache_dimension = 3;

    parameters.max_chain = 3;

    parameters.min_insert_words = 1;
    parameters.max_insert_words = 3;

    parameters.max_movements = 5;

    // ***
    //parameters.save_population = true;
    //parameters.load_population = true;
    // ***

    std::string input1("daisy daisy give me your answer do");
    std::string expected1("I'm half crazy for the love of you");
    
    std::string input2("it won't be a stylish marriage");
    std::string expected2("I cannot afford a carriage");

    std::string input3("but you'll look sweet upon the seat");
    std::string expected3("of a bicycle built for two");

    organisation::inputs::epoch epoch1(input1, expected1);
    organisation::inputs::epoch epoch2(input2, expected2);
    organisation::inputs::epoch epoch3(input3, expected3);
    
    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);
    parameters.input.push_back(epoch3);
    
    organisation::dictionary words;
    words.push_back(parameters.input);
    auto strings = words.get();
    organisation::data mappings(strings);
    parameters.mappings = mappings;        

    for(int i = 0; i < parameters.input.size(); ++i)
    {        
        organisation::inputs::epoch temp;
        if(parameters.input.get(temp, i))
            std::cout << "input: \"" << temp.input << "\" expected: \"" << temp.expected << "\"\r\n";
    }

    return parameters;
}

bool run(organisation::templates::programs *program, organisation::parameters &parameters, organisation::schema &result)
{         	
    organisation::populations::population p(program, parameters);
    if(!p.initalised()) return false;
    
    int actual = 0;

    p.clear();
    p.generate();
    
    result.copy(p.go(actual, generations));

    std::string filename("data/run.txt");
    if(actual > generations) filename = std::string("data/failed.txt");    
    result.prog.save(filename);
    
    return true;
}

bool single(std::string filename)
{
    organisation::parameters parameters = get_parameters();
    organisation::history::stream stream;

    parameters.dim_clients = organisation::point(1,1,1);
    parameters.history = &stream;
    parameters.save_outputs = true;

	::parallel::device device(device_idx);
	::parallel::queue queue(device);

    parallel::mapper::configuration mapper;

    organisation::schema result(parameters);   
    organisation::parallel::program program(device, &queue, mapper, parameters);

    if(!program.initalised()) return false;
    
    organisation::schema s1(parameters);

    if(!s1.prog.load(filename)) return false;
        
    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.set(parameters.mappings, parameters.input);
    program.run(parameters.mappings);

    std::vector<organisation::outputs::output> results = program.get(parameters.mappings);

    int epoch = 0;
    for(auto &it: results)
    {
        std::string result;
        for(auto &jt: it.values)
        {
            result += jt.value + " ";
        }

        std::cout << "output" << std::to_string(epoch++) << ": " << result << "\r\n";
    }

    parameters.mappings.save("data/mapping.txt");
    
    return true;    
}

int main(int argc, char *argv[])
{  
    bool history = false;

    if(argc > 1)
    {
        std::string filename = "data/saved/run.txt";

        std::string argument1 = std::string(argv[1]);
        if(argc > 2) filename = std::string(argv[2]);
        
        if(argument1=="SINGLE")
        {
            single(filename);
            return 0;
        }
        else if(argument1 == "HISTORY") history = true;
    }
    
    organisation::parameters parameters = get_parameters();

    if(history)
    {
        organisation::history::stream stream;
        parameters.history = &stream;
    }

	::parallel::device device(device_idx);
	::parallel::queue queue(device);

    parallel::mapper::configuration mapper;

    organisation::schema result(parameters);   
    organisation::parallel::program program(device, &queue, mapper, parameters);

    if(program.initalised())
    {
        run(&program, parameters, result);
    }
       
    return 0;
}
