#include <gtest/gtest.h>
#include "parallel/program.hpp"
#include "parallel/map/configuration.hpp"
#include "parallel/value.hpp"
#include "general.h"
#include "data.h"
#include "schema.h"
#include "kdpoint.h"
#include "statistics.h"
#include "dictionary.h"
#include <unordered_map>

TEST(BasicProgramCollisionOutputOneParallel, BasicAssertions)
{    
    //GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy");
   
    std::vector<std::vector<std::string>> expected = {
        { 
            "give"
        }
    };

    std::vector<std::string> strings = organisation::split(input1 + " give");
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 40;
    parameters.clear_links = false;
    parameters.output_stationary_only = true;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);
  
    organisation::vector up(0,1,0);
    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { up };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 20);
    insert.values = { a };
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(organisation::genetic::cache::xyzw(0,-1,-1), organisation::genetic::cache::xyzw(starting.x,18,starting.z,-2));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector rebound(1,0,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    organisation::genetic::links link(parameters);
    link.set(organisation::point(1,-1,-1),0);
    std::vector<organisation::genetic::links*> links = { &link };

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);
    
    // ***

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.copy(links.data(), links.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<std::vector<std::string>> compare;
    std::vector<organisation::outputs::output> results = program.get(mappings);
    
    for(auto &epoch: results)
    {
        std::unordered_map<int,std::vector<std::string>> data;
        for(auto &output: epoch.values)
        {
            if(data.find(output.client) == data.end()) data[output.client] = { output.value };
            else data[output.client].push_back(output.value);
        }

        std::vector<std::string> temp(1);
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }
    
    EXPECT_EQ(compare, expected);
}

TEST(BasicProgramCollisionOutputNoneParallel, BasicAssertions)
{    
    //GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy");
   
    std::vector<std::string> strings = organisation::split(input1 + " give");
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 40;
    parameters.clear_links = false;
    parameters.output_stationary_only = true;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);
  
    organisation::vector up(0,1,0);
    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { up };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 20);
    insert.values = { a };
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(organisation::genetic::cache::xyzw(0,-1,-1), organisation::genetic::cache::xyzw(starting.x,18,starting.z,-3));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector rebound(1,0,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    organisation::genetic::links link(parameters);
    link.set(organisation::point(1,-1,-1),0);
    std::vector<organisation::genetic::links*> links = { &link };

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);
    
    // ***

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.copy(links.data(), links.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<std::vector<std::string>> compare;
    std::vector<organisation::outputs::output> results = program.get(mappings);
    
    EXPECT_EQ(results.size(), 0);
}
