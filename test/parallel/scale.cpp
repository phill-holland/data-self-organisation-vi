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

organisation::schema getScaleSchema1(organisation::parameters &parameters,
                                     int direction, int rebound,
                                     int iteration, int value,
                                     int delay)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    organisation::vector _direction;
    _direction.decode(direction);
    movement.directions = { _direction };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(delay, organisation::point(starting.x,starting.y,starting.z), movement);
    insert.values = { a };   
    
    organisation::point wall(parameters.width/2,parameters.height/2,parameters.depth/2);
    for(int i = 0; i < iteration; ++i)
    {
        wall.x += _direction.x;
        wall.y += _direction.y;
        wall.z += _direction.z;
    }

    organisation::genetic::cache cache(parameters);    
    if(wall != organisation::point(parameters.width/2,parameters.height/2,parameters.depth/2))
        cache.set(organisation::point(value,-1,-1), wall);

    organisation::genetic::collisions collisions(parameters);

    int offset = 0;
    for(int i = 0; i < parameters.mappings.maximum(); ++i)
    {        
        collisions.set(rebound, offset + direction);
        offset += parameters.max_collisions;
    }

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    return s1;
}

organisation::schema getScaleSchema2(organisation::parameters &parameters,
                                     organisation::vector direction,
                                     organisation::vector rebound,
                                     organisation::point wall,
                                     int value, int delay)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = { direction };

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(delay, organisation::point(starting.x,starting.y,starting.z), movement);
    insert.values = { a };   

    organisation::genetic::cache cache(parameters);    
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

TEST(BasicProgramScaleTestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const organisation::point clients(6,1,1);//(10,10,10);
    const int width = 20, height = 20, depth = 20;

    std::string values1("daisy give me your answer do .");
    std::string input1("daisy");
    std::vector<std::string> expected = 
    { 
        "daisygive", "give", "me", "your", "answer", "do"
    };
    
    std::vector<std::string> strings = organisation::split(values1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = clients;    
    parameters.iterations = 7;
    parameters.host_buffer = 100; 
    parameters.max_inserts = 30;   
    parameters.clear_links = false;

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;    
    organisation::parallel::program program(device, &queue, mapper, parameters);
        
    EXPECT_TRUE(program.initalised());
    
    organisation::schema s1 = getScaleSchema2(parameters, { 1, 0, 0 }, { 0, 1, 0 }, { 12,10,10 }, 0, 1);
    organisation::schema s2 = getScaleSchema2(parameters, {-1, 0, 0 }, { 0,-1, 0 }, {  7,10,10 }, 1, 1);
    organisation::schema s3 = getScaleSchema2(parameters, { 0, 1, 0 }, { 1, 0, 0 }, {10, 12,10 }, 2, 1);
    organisation::schema s4 = getScaleSchema2(parameters, { 0,-1, 0 }, {-1, 0, 0 }, {10,  7,10 }, 3, 1);
    organisation::schema s5 = getScaleSchema2(parameters, { 0, 0, 1 }, { 1, 0, 0 }, {10, 10,12 }, 4, 1);
    organisation::schema s6 = getScaleSchema2(parameters, { 0, 0,-1 }, {-1, 0, 0 }, {10, 10, 7 }, 5, 1);

    std::vector<organisation::schema*> schemas = { &s1,&s2,&s3,&s4,&s5,&s6 };
    std::vector<organisation::schema*> schemas_source;
    
    organisation::genetic::links link(parameters);
    std::vector<int> hashes = mappings.all();
    //link.set(organisation::point(0,-1,-1),0);
    link.set(organisation::point(1,-1,-1),0);
    link.set(organisation::point(0,-1,-1),hashes.size());

    link.set(organisation::point(3,-1,-1),hashes.size() * 2);
    link.set(organisation::point(2,-1,-1),hashes.size() * 3);

    link.set(organisation::point(5,-1,-1),hashes.size() * 4);
    link.set(organisation::point(4,-1,-1),hashes.size() * 5);

    //link.seed(mappings.all());

    std::vector<organisation::genetic::links*> links_source;
    
    int length = clients.x * clients.y * clients.z;
    int total = schemas.size();

    for(int i = 0; i < length; ++i)
    {
        schemas_source.push_back(schemas[i % total]);
        links_source.push_back(&link);
    }
    
    program.copy(schemas_source.data(), schemas_source.size());
    program.copy(links_source.data(), links_source.size());
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

    EXPECT_EQ(compare.size(), 1);
    EXPECT_EQ(schemas_source.size(), compare[0].size());

    std::unordered_map<int,std::string> first = compare.front();    
    for(int i = 0; i < length; ++i)
    {
        EXPECT_FALSE(first.find(i) == first.end());
        EXPECT_EQ(first[i],expected[i % total]);
    }
    
    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_EQ(schemas_source.size(), data.size());

    std::vector<organisation::parallel::value> values = {
        { organisation::point(15,11,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point( 5, 9,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point(11,15,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point( 9, 5,10), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point(11,10,15), organisation::point(0,-1,-1), 1, 0 },
        { organisation::point( 9,10, 5), organisation::point(0,-1,-1), 1, 0 },
    };

    std::unordered_map<int,organisation::parallel::value> lookup;

    for(auto &it: data) { lookup[it.client] = it; }

    for(int i = 0; i < length; ++i)
    {    
        values[i % total].client = i;
        EXPECT_EQ(lookup[i], values[i % total]);
    }
}

TEST(BasicProgramAllDirectionsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const organisation::point clients(27,1,1);
    const int width = 20, height = 20, depth = 20;

    std::string values1("a b c d e f g h i j k l m n o p q r s t u v w x y z .");
    std::string input1("a");
    
    std::vector<std::string> strings = organisation::split(values1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = clients;    
    parameters.iterations = 6;
    parameters.host_buffer = 100; 
    parameters.max_inserts = 30;   

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;    
    organisation::parallel::program program(device, &queue, mapper, parameters);
        
    EXPECT_TRUE(program.initalised());
    
    const int iterations = 5;
    std::vector<organisation::parallel::value> predictions;
    std::vector<organisation::schema*> source;

    for(int i = 0; i < parameters.max_collisions; ++i)
    {
        int rebound = i + 1;
        if(rebound >= parameters.max_collisions) rebound = 0;

        organisation::schema temp = getScaleSchema1(parameters, i, rebound, iterations, i, 1);
        organisation::schema *schema = new organisation::schema(parameters);
        
        EXPECT_TRUE(schema != NULL);
        EXPECT_TRUE(schema->initalised());

        schema->copy(temp);
        source.push_back(schema);

        organisation::vector _direction, _rebound;
        _direction.decode(i);
        _rebound.decode(rebound);

        organisation::point prediction(width/2,height/2,depth/2);
        for(int i = 0; i < iterations - 1; ++i)
        {
            prediction.x += _direction.x;
            prediction.y += _direction.y;
            prediction.z += _direction.z;
        }

        if(prediction != organisation::point(width/2,height/2,depth/2))
        {
            prediction.x += _rebound.x;
            prediction.y += _rebound.y;
            prediction.z += _rebound.z;
        }

        organisation::parallel::value value = { prediction, organisation::point(0,-1,-1), 1, i };
        predictions.push_back(value);
    }
    
    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_EQ(data, predictions);

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

    EXPECT_EQ(compare.size(), 1);
    EXPECT_EQ(strings.size() - 1, compare[0].size());

    std::unordered_map<int,std::string> first = compare.front();        

    for(int i = 0; i < strings.size(); ++i)
    {
        if(strings[i] != std::string("n"))
            EXPECT_EQ(first[i],strings[i]);
    }

   for(auto &it:source)
   {
        if(it != NULL) delete it;
   }
}
