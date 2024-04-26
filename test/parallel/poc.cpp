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

TEST(BasicProgramDualEpochProofOfConceptTestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy give");
    std::string input2("daisy answer");

    std::string expected1 = "I'mhalf";
    std::string expected2 = "loveyou";
    
    std::vector<std::string> strings = organisation::split(input1 + " " + input2 + " I'm half love you");
    organisation::data mappings(strings);
	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.output_stationary_only = true;
    parameters.iterations = 10;
    
    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    organisation::inputs::epoch epoch2(input2);
    parameters.input.push_back(epoch2);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement0(parameters.min_movements, parameters.max_movements);
    movement0.directions = { { 1,0,0 } };

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { { -1,0,0 } };

    organisation::genetic::inserts::insert insert0(parameters);
    organisation::genetic::inserts::value a(1, organisation::point(starting.x - 4,starting.y,starting.z), movement0);
    organisation::genetic::inserts::value b(1, organisation::point(starting.x + 4,starting.y,starting.z), movement1);
    
    insert0.values = { a, b };

    organisation::genetic::cache cache(parameters);
    
    cache.set(organisation::point(mappings.map("I'm"),-1,-1), organisation::point(11,10,9));
    cache.set(organisation::point(mappings.map("half"),-1,-1), organisation::point(8,10,11));
    cache.set(organisation::point(mappings.map("love"),-1,-1), organisation::point(10,11,11));
    cache.set(organisation::point(mappings.map("you"),-1,-1), organisation::point(9,9,9));

    organisation::vector left(-1,0,0);
    organisation::vector right(1,0,0);
    organisation::vector up(0,1,0);
    organisation::vector down(0,-1,0);
    
    organisation::vector v1(1,1,0); 
    organisation::vector v2(1,0,0); 
    organisation::vector v3(-1,0,-1);

    organisation::genetic::collisions collisions(parameters);

    int value0 = mappings.map("daisy");
    int value1 = mappings.map("give");
    int value2 = mappings.map("answer");

    collisions.set(v1.encode(), (value0 * parameters.max_collisions) + right.encode());
    collisions.set(v2.encode(),(value1 * parameters.max_collisions) + left.encode());
    collisions.set(v3.encode(),(value2 * parameters.max_collisions) + left.encode());

    organisation::vector vz(0,0,-1);

    int value3 = mappings.map("I'm");
    int value4 = mappings.map("love");

    collisions.set(vz.encode(), (value3 * parameters.max_collisions) + right.encode());
    collisions.set(vz.encode(), (value4 * parameters.max_collisions) + right.encode());
    
    s1.prog.set(cache);
    s1.prog.set(insert0);
    s1.prog.set(collisions);

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<std::unordered_map<int,std::string>> compare;
    std::vector<organisation::outputs::output> results = program.get(mappings);
    
    for(auto &epoch: results)
    {
        std::unordered_map<int,std::vector<std::string>> data;
        for(auto &output: epoch.values)
        {
            if(data.find(output.client) == data.end()) data[output.client] = { output.value };
            else data[output.client].push_back(output.value);
        }

        std::unordered_map<int,std::string> temp;
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }

    EXPECT_EQ(expected1, compare[0][0]);
    EXPECT_EQ(expected2, compare[1][0]);
}

TEST(BasicProgramDualEpochProofOfConceptWithBulkValueInsertTestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input1("daisy give answer answer give daisy");    
    std::string expected1 = "I'mhalfloveyou";
    
    std::vector<std::string> strings = organisation::split(input1 + " I'm half love you");
    organisation::data mappings(strings);
	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.output_stationary_only = true;
    parameters.iterations = 10;
    
    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement0(parameters.min_movements, parameters.max_movements);
    movement0.directions = { { 1,0,0 } };

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { { -1,0,0 } };

    organisation::genetic::inserts::insert insert0(parameters);
    organisation::genetic::inserts::value a(1, organisation::point(starting.x - 4,starting.y,starting.z), movement0, 3);
    organisation::genetic::inserts::value b(1, organisation::point(starting.x + 4,starting.y,starting.z), movement1, 3);
    
    insert0.values = { a, b };

    organisation::genetic::cache cache(parameters);
    
    cache.set(organisation::point(mappings.map("I'm"),mappings.map("half"),-1), organisation::point(11,9,9));
    cache.set(organisation::point(mappings.map("love"),mappings.map("you"),-1), organisation::point(8,11,11));

    organisation::vector left(-1,0,0);
    organisation::vector right(1,0,0);
    organisation::vector up(0,1,0);
    organisation::vector down(0,-1,0);
    
    organisation::vector v1(1,1,0); 
    organisation::vector v2(1,0,0); 
    organisation::vector v3(-1,0,-1);

    organisation::genetic::collisions collisions(parameters);

    int value0 = mappings.map("daisy");
    int value1 = mappings.map("give");
    int value2 = mappings.map("answer");

    collisions.set(v1.encode(), (value0 * parameters.max_collisions) + right.encode());
    collisions.set(v2.encode(),(value1 * parameters.max_collisions) + left.encode());
    collisions.set(v3.encode(),(value2 * parameters.max_collisions) + left.encode());

    collisions.set(v1.encode(), (value0 * parameters.max_collisions) + left.encode());
    collisions.set(v2.encode(),(value1 * parameters.max_collisions) + right.encode());
    collisions.set(v3.encode(),(value2 * parameters.max_collisions) + right.encode());

    s1.prog.set(cache);
    s1.prog.set(insert0);
    s1.prog.set(collisions);

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<std::unordered_map<int,std::string>> compare;
    std::vector<organisation::outputs::output> results = program.get(mappings);
    
    for(auto &epoch: results)
    {
        std::unordered_map<int,std::vector<std::string>> data;
        for(auto &output: epoch.values)
        {
            if(data.find(output.client) == data.end()) data[output.client] = { output.value };
            else data[output.client].push_back(output.value);
        }

        std::unordered_map<int,std::string> temp;
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }

    EXPECT_EQ(expected1, compare[0][0]);
}


TEST(BasicProgramDualEpochProofOfConcept2TestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string dictionary("daisy give me your answer do I'm half crazy for the love of you");
    std::string input1("daisy daisy give me your answer do");
    
    std::string expected1 = "I'mhalfcrazyfortheloveofyou";
    
    std::vector<std::string> strings = organisation::split(dictionary);
    organisation::data mappings(strings);
	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(1,1,1);
    parameters.output_stationary_only = true;
    parameters.iterations = 13;
    parameters.max_insert_delay = 20;
    
    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;
    mapper.origin = organisation::point(width / 2, height / 2, depth / 2);

    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement0(parameters.min_movements, parameters.max_movements);
    movement0.directions = { { 1,0,0 } };

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { { -1,0,0 } };

    organisation::genetic::movements::movement movement3(parameters.min_movements, parameters.max_movements);
    movement3.directions = { { 0,0,-1 } };

    organisation::genetic::inserts::insert insert0(parameters);
    organisation::genetic::inserts::value a(1, organisation::point(starting.x - 4,starting.y,starting.z), movement0, 3);
    organisation::genetic::inserts::value b(1, organisation::point(starting.x + 4,starting.y,starting.z), movement1, 3);
    // ***
    organisation::genetic::inserts::value c(2, organisation::point(starting.x + 2,starting.y + 1,starting.z + 9), movement3, 3);
    // ***
    
    insert0.values = { a, b, c };

    organisation::genetic::cache cache(parameters);

    cache.set(organisation::point(mappings.map("I'm"),mappings.map("half"),mappings.map("crazy")), organisation::point(10,10,11));
    cache.set(organisation::point(mappings.map("for"),mappings.map("the"),mappings.map("love")), organisation::point(9,10,9));
    cache.set(organisation::point(mappings.map("of"),mappings.map("you"),-1), organisation::point(12,10,13));

    organisation::vector left(-1,0,0);
    organisation::vector right(1,0,0);
    organisation::vector up(0,1,0);
    organisation::vector down(0,-1,0);
    
    organisation::vector v1(1,1,0); 
    organisation::vector v2(1,0,0); 
    organisation::vector v3(-1,0,-1);

    organisation::genetic::collisions collisions(parameters);

    int value0 = mappings.map("daisy");
    int value1 = mappings.map("give");

    int value2 = mappings.map("me");
    int value3 = mappings.map("your");
    int value4 = mappings.map("answer");
    
    collisions.set(v1.encode(), (value0 * parameters.max_collisions) + right.encode());
    collisions.set(v2.encode(), (value1 * parameters.max_collisions) + right.encode());
    
    collisions.set(v3.encode(), (value2 * parameters.max_collisions) + left.encode());
    collisions.set(v3.encode(), (value3 * parameters.max_collisions) + left.encode());
    collisions.set(v1.encode(), (value4 * parameters.max_collisions) + left.encode());

    s1.prog.set(cache);
    s1.prog.set(insert0);
    s1.prog.set(collisions);

    std::vector<organisation::schema*> source = { &s1 };
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<std::unordered_map<int,std::string>> compare;
    std::vector<organisation::outputs::output> results = program.get(mappings);
    
    for(auto &epoch: results)
    {
        std::unordered_map<int,std::vector<std::string>> data;
        for(auto &output: epoch.values)
        {
            if(data.find(output.client) == data.end()) data[output.client] = { output.value };
            else data[output.client].push_back(output.value);
        }

        std::unordered_map<int,std::string> temp;
        for(auto &value: data)
        {
            temp[value.first] = std::reduce(value.second.begin(),value.second.end(),std::string(""));
        }

        compare.push_back(temp);
    }

    EXPECT_EQ(expected1, compare[0][0]);
}
