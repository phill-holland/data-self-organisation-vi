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

TEST(BasicProgramMovementWithCollisionBasicLinkTestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy daisy give me your answer do . banana");
   
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisygivemeyouranswerdo"
        }
    };
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 16;
    parameters.max_word_count = 10;
    parameters.output_stationary_only = true;
    parameters.full_stop_pause = true;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::point position = organisation::point(starting.x,18,starting.z);
    organisation::vector up(0,1,0);
    organisation::vector rebound(1,0,0);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { up, up, organisation::vector(0,0,0), organisation::vector(0,0,0), organisation::vector(0,0,0) };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 3, 5);//40);
    insert.values = { a };
    
    std::vector<organisation::vector> rebounds1 = { 
        organisation::vector(0,0,-1),
        organisation::vector(1,0,1),        
        organisation::vector(0,-1,0),

        organisation::vector(0,0,-1),
        organisation::vector(1,0,1)    
    };

    organisation::genetic::collisions collisions(parameters);

    for(int i = 0; i < rebounds1.size(); ++i)
    {
        int offset = parameters.max_collisions * i;
        collisions.set(rebounds1[i].encode(), offset + up.encode());
    }

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
    
    EXPECT_EQ(compare, expected);
}
