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

TEST(BasicProgramStopsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy give me");
   
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

    
	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 40;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::vector direction(0,1,0);
    organisation::vector rebound(1,0,0);             

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { direction };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 5);
    insert.values = { a };
    
    organisation::genetic::cache cache(parameters);
    organisation::genetic::collisions collisions(parameters);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + direction.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    // ***

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
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

    std::vector<organisation::parallel::value> values = {
        { organisation::point(10,16,10), organisation::point(0,-1,-1),  2, 0 },
        { organisation::point(11,19,10), organisation::point(1,-1,-1),  10, 0 }    
    };
    
    std::vector<organisation::parallel::value> data = program.get(true);

    EXPECT_EQ(compare.size(), 0);
    EXPECT_EQ(values, data);
}

TEST(BasicProgramStopsAndLinksParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy give me your");
   
    std::vector<std::vector<std::string>> expected = {
        { 
            "givedaisydaisygive"
        }
    };

    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.output_stationary_only = true;
    parameters.iterations = 40;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::vector direction1(1,0,0);
    organisation::vector direction2(-1,0,0);

    organisation::vector rebound1(1,1,0);                 
    organisation::vector rebound2(0,-1,1);             

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { direction1 };

    organisation::genetic::movements::movement movement2(parameters.min_movements, parameters.max_movements);
    movement2.directions = { direction2 };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x - 3,starting.y,starting.z), movement1, 1, 8);
    organisation::genetic::inserts::value b(2, organisation::point(starting.x + 3,starting.y,starting.z), movement2, 1, 8);
    insert.values = { a, b };
    
    organisation::genetic::cache cache(parameters);
    organisation::genetic::collisions collisions(parameters);

    std::vector<organisation::vector> directions = { direction1, direction2, direction1, direction2 };
    std::vector<organisation::vector> rebounds = { rebound1, rebound2, rebound1, rebound2 };

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebounds[i].encode(), offset + directions[i].encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    // ***

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
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

    std::vector<organisation::parallel::value> values = {
        { organisation::point(17,9,9), organisation::point(0,-1,-1),  4, 0 },
        { organisation::point(3,11,11), organisation::point(1,-1,-1), 4, 0 }    
    };
    
    std::vector<organisation::parallel::value> data = program.get(true);

    EXPECT_EQ(compare, expected);
    EXPECT_EQ(values, data);
}


TEST(BasicProgramStopsMultiplySchemasParallel, BasicAssertions)
{    
    //GTEST_SKIP();

    const int width = 40, height = 40, depth = 40;
    organisation::point starting(width / 2, 0, depth / 2);

    std::string input1("daisy give me");
   
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(2,2,1);
    parameters.iterations = 40;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);
    organisation::schema s2(parameters);
    organisation::schema s3(parameters);
    organisation::schema s4(parameters);

    organisation::vector direction(0,1,0);
    organisation::vector rebound(1,0,0);             

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { direction };

// need multiple inserts per schema to be tested as well ...??
    organisation::genetic::inserts::insert insert1(parameters);
    organisation::genetic::inserts::value a(0, organisation::point(starting.x,starting.y,starting.z), movement, 3, 5);
    insert1.values = { a };
    
    organisation::genetic::inserts::insert insert2(parameters);
    organisation::genetic::inserts::value b(0, organisation::point(starting.x,starting.y,starting.z), movement, 3, 10);
    insert2.values = { b };

    organisation::genetic::inserts::insert insert3(parameters);
    organisation::genetic::inserts::value c(0, organisation::point(starting.x,starting.y,starting.z), movement, 3, 15);
    insert3.values = { c };

    organisation::genetic::inserts::insert insert4(parameters);
    organisation::genetic::inserts::value d(0, organisation::point(starting.x,starting.y,starting.z), movement, 3, 20);
    insert4.values = { d };

    organisation::genetic::cache cache(parameters);
    organisation::genetic::collisions collisions(parameters);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + direction.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert1);
    s1.prog.set(collisions);

    s2.prog.set(cache);
    s2.prog.set(insert2);
    s2.prog.set(collisions);

    s3.prog.set(cache);
    s3.prog.set(insert3);
    s3.prog.set(collisions);

    s4.prog.set(cache);
    s4.prog.set(insert4);
    s4.prog.set(collisions);

    // ***

    std::vector<organisation::schema*> source = { &s1, &s2, &s3, &s4 };
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> values = {
        { organisation::point(20, 6,20), organisation::point(0,1,2),  1, 0 },
        { organisation::point(20,11,20), organisation::point(0,1,2),  1, 1 },
        { organisation::point(20,16,20), organisation::point(0,1,2),  1, 2 },
        { organisation::point(20,21,20), organisation::point(0,1,2),  1, 3 }        
    };
    
    std::vector<organisation::parallel::value> data = program.get(true);

    EXPECT_EQ(values, data);
}
