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

organisation::schema getSchema1(organisation::parameters &parameters, organisation::point value)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { { 1,0,0 }, { 1,0,0 } };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
    insert.values = { a };    

    organisation::genetic::cache::cache cache(parameters);
    cache.set(value, organisation::point(18,10,10));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector up(1,0,0);
    organisation::vector rebound(0,1,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    return s1;
}

organisation::schema getSchema2(organisation::parameters &parameters, organisation::point value)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { { 0,1,0 }, { 0,1,0 } };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
    insert.values = { a };   
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(value, organisation::point(10,18,10));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector up(0,1,0);
    organisation::vector rebound(1,0,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    return s1;
}

organisation::schema getSchema4(organisation::parameters &parameters,
                                organisation::vector direction,
                                organisation::vector rebound,
                                organisation::point wall,
                                int value,
                                int delay)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { direction };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(delay, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
    insert.values = { a };   

    organisation::genetic::cache::cache cache(parameters);    
    cache.set(organisation::point(value,-1,-1), wall);

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

    return s1;
}        

TEST(BasicProgramMovementWithCollisionParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy daisy give me your answer do .");
   
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisydaisydaisydaisydaisydaisy"
        }
    };

    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

    std::vector<std::tuple<organisation::point,organisation::vector,organisation::vector>> directions = {
        { 
            organisation::point(starting.x,18,starting.z), 
            organisation::vector(0,1,0), 
            organisation::vector(1,0,0)             
        },
        { 
            organisation::point(starting.x,2,starting.z), 
            organisation::vector(0,-1,0), 
            organisation::vector(1,0,0)             
        },
        { 
            organisation::point(18,starting.y,starting.z), 
            organisation::vector(1,0,0), 
            organisation::vector(0,1,0)             
        },
        { 
            organisation::point(2,starting.y,starting.z), 
            organisation::vector(-1,0,0), 
            organisation::vector(0,1,0)             
        },         
        { 
            organisation::point(starting.x,starting.y,18), 
            organisation::vector(0,0,1), 
            organisation::vector(0,1,0)             
        },       
        { 
            organisation::point(starting.x,starting.y,2), 
            organisation::vector(0,0,-1), 
            organisation::vector(0,1,0)             
        }
    };

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

    for(auto &it: directions)    
    {        
        organisation::parallel::program program(device, &queue, mapper, parameters);
        
        EXPECT_TRUE(program.initalised());
        
        organisation::schema s1(parameters);

        organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
        movement.directions = { std::get<1>(it) };

        organisation::genetic::inserts::insert insert(parameters);
        organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
        insert.values = { a };
        
        organisation::genetic::cache::cache cache(parameters);
        cache.set(organisation::point(0,-1,-1), std::get<0>(it));

        organisation::genetic::collisions collisions(parameters);

        organisation::vector up = std::get<1>(it);
        organisation::vector rebound = std::get<2>(it);

        int offset = 0;
        for(int i = 0; i < parameters.mappings.maximum(); ++i)
        {        
            collisions.set(rebound.encode(), offset + up.encode());
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
        
        EXPECT_EQ(compare, expected);
    }
}

TEST(BasicProgramMovementWithCollisionForDifferentWordsParallel, BasicAssertions)
{   
    GTEST_SKIP();

    const int width = 60, height = 60, depth = 60;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy daisy give me your answer do .");
    
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisydaisydaisydaisydaisydaisy", 
            "daisydaisydaisydaisydaisydaisydaisydaisy" 
        }
    };

    std::vector<organisation::vector> rebounds1 = { 
        organisation::vector(1,0,0),
        organisation::vector(-1,0,0),
        organisation::vector(0,0,1),
        organisation::vector(0,0,-1),
        organisation::vector(1,0,1),
        organisation::vector(-1,0,-1),
        organisation::vector(1,0,0)
    };

    std::vector<organisation::vector> rebounds2 = { 
        organisation::vector(0,0,1),
        organisation::vector(0,0,-1),
        organisation::vector(1,0,0),
        organisation::vector(-1,0,0),
        organisation::vector(1,0,1),
        organisation::vector(-1,0,-1),
        organisation::vector(1,0,0)
    };

    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);
	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(2,1,1);
    parameters.iterations = 33;
    // ***
    // max_cache + data.unique()
    parameters.max_word_count = 10;
    // ***

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);
    organisation::schema s2(parameters);

    organisation::vector up(0,1,0);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { up };
    
    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);

    insert.values = { a };
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(organisation::point(0, -1, -1), organisation::point(starting.x,starting.y + 8,starting.z));

    organisation::genetic::collisions collisions1(parameters);
    organisation::genetic::collisions collisions2(parameters);

    for(int i = 0; i < rebounds1.size(); ++i)
    {
        int offset = parameters.max_collisions * i;
        collisions1.set(rebounds1[i].encode(), offset + up.encode());
    }

    for(int i = 0; i < rebounds2.size(); ++i)
    {
        int offset = parameters.max_collisions * i;
        collisions2.set(rebounds2[i].encode(), offset + up.encode());
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions1);

    s2.prog.set(cache);
    s2.prog.set(insert);
    s2.prog.set(collisions2);
    // ***

    std::vector<organisation::schema*> source = { &s1, &s2 };
    
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

        std::vector<std::string> temp(2);
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }
    
    EXPECT_EQ(compare, expected);

    std::vector<organisation::parallel::value> values1 = {
        { organisation::point(31,57,30), organisation::point(0,-1,-1), 12, 0 },
        { organisation::point(29,54,30), organisation::point(1,-1,-1), 15, 0 },
        { organisation::point(30,51,31), organisation::point(2,-1,-1), 18, 0 },
        { organisation::point(30,48,29), organisation::point(3,-1,-1), 21, 0 },        
        { organisation::point(31,45,31), organisation::point(4,-1,-1), 24, 0 },        
        { organisation::point(29,42,29), organisation::point(5,-1,-1), 27, 0 },        
        { organisation::point(31,39,30), organisation::point(6,-1,-1), 30, 0 },        
    };
    
    std::vector<organisation::parallel::value> values2 = {
        { organisation::point(30,57,31), organisation::point(0,-1,-1), 12, 1 },
        { organisation::point(30,54,29), organisation::point(1,-1,-1), 15, 1 },
        { organisation::point(31,51,30), organisation::point(2,-1,-1), 18, 1 },
        { organisation::point(29,48,30), organisation::point(3,-1,-1), 21, 1 },        
        { organisation::point(31,45,31), organisation::point(4,-1,-1), 24, 1 },        
        { organisation::point(29,42,29), organisation::point(5,-1,-1), 27, 1 },        
        { organisation::point(31,39,30), organisation::point(6,-1,-1), 30, 1 },        
    };

    std::vector<organisation::parallel::value> data = program.get();

    std::unordered_map<int,std::vector<organisation::parallel::value>> lookup;
    lookup[0] = { }; lookup[1] = { };

    for(auto &it: data) 
    { 
        lookup[it.client].push_back(it);
    }

    EXPECT_EQ(lookup[0], values1);
    EXPECT_EQ(lookup[1], values2);
}

TEST(BasicProgramMovementWithTwoClientsAndTwoEpochsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");    
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisydaisydaisydaisydaisydaisy",
            "youryouryouryouryouryouryouryour"
        },
        { 
            "daisydaisydaisydaisydaisydaisy",
            "youryouryouryouryouryour"
        }
    };

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    
    parameters.dim_clients = organisation::point(2,1,1);
    parameters.iterations = 40;
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);

    EXPECT_TRUE(program.initalised());

    organisation::schema s1 = getSchema1(parameters,organisation::point(0,-1,-1));
    organisation::schema s2 = getSchema2(parameters,organisation::point(3,-1,-1));

    std::vector<organisation::schema*> source = { &s1, &s2 };
    
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

        std::vector<std::string> temp(2);
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }
    
    EXPECT_EQ(compare, expected);
}

TEST(BasicProgramMovementMultiCellOutputParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");    
    std::vector<std::vector<std::string>> expected =
    { 
        { 
            "daisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygive", 
            "youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo" 
        }, 
        { 
            "daisygivedaisygivedaisygivedaisygivedaisygivedaisygive", 
            "youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo" 
        } 
    };
    /* {
        { 
            //"daisygivedaisygivedaisygivedaisygivedaisygivedaisygive",
            //"youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo"   
          "daisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygive",          
          "youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo"         
        },
        {     
            "daisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygivedaisygive",          
          "youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo"                 
            //"daisygivedaisygivedaisygivedaisygivedaisygivedaisygive",
            //"youranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdoyouranswerdo"
        }
    };*/

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    
    parameters.dim_clients = organisation::point(2,1,1);
    parameters.iterations = 40;
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);

    EXPECT_TRUE(program.initalised());

    organisation::schema s1 = getSchema1(parameters,organisation::point(0,1,-1));
    organisation::schema s2 = getSchema2(parameters,organisation::point(3,4,5));

    std::vector<organisation::schema*> source = { &s1, &s2 };
    
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

        std::vector<std::string> temp(2);
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }
    
    EXPECT_EQ(compare, expected);
}

TEST(BasicProgramMovementReboundDirectionSameAsMovementDirectionParallel, BasicAssertions)
{    
    // FIX_ME
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    const organisation::point starting(width / 2, height / 2, depth / 2);

    std::string data1("daisy daisy give me your answer do please do .");
    std::string input1("give me your .");
    
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisygivedaisygivedaisygivemedaisygive" 
        }
    };

    std::vector<std::string> strings = organisation::split(data1);    
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 30;
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = starting;

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { { 1,0,0 }, { 1,0,0 } };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
    insert.values = { a };    
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(organisation::point(0,-1,-1), organisation::point(18,10,10));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector up(1,0,0);
    organisation::vector rebound(1,0,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

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

    std::vector<organisation::parallel::value> values = {
        { organisation::point(17,10,10), organisation::point(1,-1,-1), 2, 0 },
        { organisation::point(16,10,10), organisation::point(2,-1,-1), 5, 0 },
        { organisation::point(15,10,10), organisation::point(3,-1,-1), 8, 0 },
        { organisation::point(14,10,10), organisation::point(7,-1,-1),11, 0 },        
    };
    
    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_EQ(data, values);
}

TEST(BasicProgramMovementReboundDirectionSameAsMovementDirectionOutputStationaryOnlyParallel, BasicAssertions)
{    
    // FIX_ME
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    const organisation::point starting(width / 2, height / 2, depth / 2);

    std::string data1("daisy daisy give me your answer do please do .");
    std::string input1("give me your .");
    
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisydaisydaisydaisydaisydaisydaisydaisy"
        }
    };

    std::vector<std::string> strings = organisation::split(data1);    
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 30;
    parameters.mappings = mappings;
    parameters.output_stationary_only = true;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { { 1,0,0 }, { 1,0,0 } };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement, 1, 40);
    insert.values = { a };
    
    organisation::genetic::cache::cache cache(parameters);
    cache.set(organisation::point(0,-1,-1), organisation::point(18,10,10));

    organisation::genetic::collisions collisions(parameters);

    organisation::vector up(1,0,0);
    organisation::vector rebound(1,0,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + up.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

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

    std::vector<organisation::parallel::value> values = {
        { organisation::point(17,10,10), organisation::point(1,-1,-1), 2, 0 },
        { organisation::point(16,10,10), organisation::point(2,-1,-1), 5, 0 },
        { organisation::point(15,10,10), organisation::point(3,-1,-1), 8, 0 },
        { organisation::point(14,10,10), organisation::point(7,-1,-1),11, 0 },        
    };
    
    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_EQ(data, values);
}

TEST(BasicProgramDataSwapWithDualMovementPatternParallel, BasicAssertions)
{    
    // FIX_ME
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    const organisation::point starting(width / 2, height / 2, depth / 2);

    std::string data1("daisy daisy give me your answer do please do .");
    std::string input1("daisy give");
    
    std::vector<std::vector<std::string>> expected = {
        { "givedaisy" },
        { "givedaisy" }
    };

    std::vector<std::string> strings = organisation::split(data1);    
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.iterations = 7;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = starting;

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());

    organisation::schema s1(parameters);

organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { 
        { 0,1,0 },    
        { 0,1,0 }, 
        { 1,0,0 }, 
        { 1,0,0 }, 
        { 1,0,0 }, 
        { 1,0,0 },
        { 1,0,0 }
    };

    organisation::genetic::movements::movement movement2(parameters.min_movements, parameters.max_movements);
    movement2.directions = { 
        {  0,-1,0 },    
        {  0,-1,0 },  
        { -1, 0,0 }, 
        { -1, 0,0 }, 
        { -1, 0,0 }, 
        { -1, 0,0 },
        { -1, 0,0 }
    };
      
    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value i1(1, organisation::point(starting.x - 3,starting.y - 3,starting.z), movement1, 1, 40);
    organisation::genetic::inserts::value i2(1, organisation::point(starting.x + 3,starting.y + 3,starting.z), movement2, 1, 40);
    insert.values = { i1, i2 };
        
    organisation::genetic::cache::cache cache(parameters);

    organisation::genetic::collisions collisions(parameters);
    
    organisation::vector left(-1,0,0);
    organisation::vector right(1,0,0);
    organisation::vector rebound(0,1,0);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound.encode(), offset + left.encode());
        collisions.set(rebound.encode(), offset + right.encode());
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

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

    std::vector<organisation::parallel::value> values = {
        { organisation::point( 9,10,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point(11,10,10), organisation::point(1,-1,-1), 1, 0 }
    };
    
    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_EQ(data, values);

    std::vector<organisation::statistics::statistic> statistics = program.statistics();

    EXPECT_EQ(statistics.size(), 1);

    organisation::statistics::statistic a;

    a.epochs[0] = organisation::statistics::data(2);
    a.epochs[1] = organisation::statistics::data(2);

    EXPECT_EQ(statistics, std::vector<organisation::statistics::statistic> { a });
}


TEST(BasicProgramMultiMovementPatternsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    const organisation::point starting(width / 2, height / 2, depth / 2);

    std::string data1("daisy daisy give me your answer do please do .");
    std::string input1("daisy give");
    
    std::vector<std::string> strings = organisation::split(data1);    
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(2,1,1);
    parameters.iterations = 6;//8;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = starting;

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());

    organisation::schema s1(parameters), s2(parameters);

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { 
        { 0,1,0 }
    };

    organisation::genetic::movements::movement movement2(parameters.min_movements, parameters.max_movements);
    movement2.directions = { 
        {  0,-1,0 }        
    };

    organisation::genetic::inserts::insert insert1(parameters);
    organisation::genetic::inserts::value i1(1, organisation::point(starting.x - 1,starting.y,starting.z), movement1, 1, 40);
    organisation::genetic::inserts::value i2(1, organisation::point(starting.x + 1,starting.y,starting.z), movement2, 1, 40);
    insert1.values = { i1, i2 };
          
    // ***

    organisation::genetic::movements::movement movement3(parameters.min_movements, parameters.max_movements);
    movement3.directions = { 
        { 1,0,0 }
    };

    organisation::genetic::movements::movement movement4(parameters.min_movements, parameters.max_movements);
    movement4.directions = { 
        { -1,0,0 }        
    };

    organisation::genetic::inserts::insert insert2(parameters);
    organisation::genetic::inserts::value i3(1, organisation::point(starting.x,starting.y - 1,starting.z), movement3, 1, 40);
    organisation::genetic::inserts::value i4(1, organisation::point(starting.x,starting.y + 1,starting.z), movement4, 1, 40);
    insert2.values = { i3, i4 };
      
    organisation::genetic::cache::cache cache(parameters);
    organisation::genetic::collisions collisions(parameters);
        
    s1.prog.set(cache);
    s1.prog.set(insert1);
    s1.prog.set(collisions);

    s2.prog.set(cache);
    s2.prog.set(insert2);
    s2.prog.set(collisions);

    std::vector<organisation::schema*> source = { &s1, &s2 };
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> values0 = {
        { organisation::point( 9,16,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point(11, 4,10), organisation::point(1,-1,-1), 1, 0 }
    };

    std::vector<organisation::parallel::value> values1 = {
        { organisation::point(16, 9,10), organisation::point(0,-1,-1), 1, 1 },
        { organisation::point( 4,11,10), organisation::point(1,-1,-1), 1, 1 }
    };
    
    std::vector<organisation::parallel::value> data = program.get();

    std::unordered_map<int,std::vector<organisation::parallel::value>> clients;

    for(auto &it: data)
    {
        clients[it.client].push_back(it);
    }

    EXPECT_EQ(clients[0], values0);
    EXPECT_EQ(clients[1], values1);
}

/*
TEST(BasicProgramMovementWithCollisionSaveAndLoadAndRunParallel, BasicAssertions)
{    
    //GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy daisy give me your answer do .");
   
    std::vector<std::vector<std::string>> expected = {
        { 
            "daisydaisydaisydaisydaisydaisydaisydaisy"
        }
    };

    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

    std::vector<std::tuple<organisation::point,organisation::vector,organisation::vector>> directions = {
        { 
            organisation::point(starting.x,18,starting.z), 
            organisation::vector(0,1,0), 
            organisation::vector(1,0,0)             
        },
        { 
            organisation::point(starting.x,2,starting.z), 
            organisation::vector(0,-1,0), 
            organisation::vector(1,0,0)             
        },
        { 
            organisation::point(18,starting.y,starting.z), 
            organisation::vector(1,0,0), 
            organisation::vector(0,1,0)             
        },
        { 
            organisation::point(2,starting.y,starting.z), 
            organisation::vector(-1,0,0), 
            organisation::vector(0,1,0)             
        },         
        { 
            organisation::point(starting.x,starting.y,18), 
            organisation::vector(0,0,1), 
            organisation::vector(0,1,0)             
        },       
        { 
            organisation::point(starting.x,starting.y,2), 
            organisation::vector(0,0,-1), 
            organisation::vector(0,1,0)             
        }
    };

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

    for(auto &it: directions)    
    {        
        organisation::parallel::program program(device, &queue, mapper, parameters);
        
        EXPECT_TRUE(program.initalised());
        
        organisation::schema s1(parameters);

        organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
        movement.directions = { std::get<1>(it) };

        organisation::genetic::inserts::insert insert(parameters);
        organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z), movement);
        insert.values = { a };
        
        organisation::genetic::cache::cache cache(parameters);
        cache.set(organisation::point(0,-1,-1), std::get<0>(it));

        organisation::genetic::collisions collisions(parameters);

        organisation::vector up = std::get<1>(it);
        organisation::vector rebound = std::get<2>(it);

        int offset = 0;
        for(int i = 0; i < parameters.mappings.maximum(); ++i)
        {        
            collisions.set(rebound.encode(), offset + up.encode());
            offset += parameters.max_collisions;
        }

        s1.prog.set(cache);
        s1.prog.set(insert);
        s1.prog.set(collisions);

        // ***
        
        std::string stream = s1.prog.serialise();
        s1.prog.deserialise(stream);

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
}
*/