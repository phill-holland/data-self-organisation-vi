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

organisation::schema getSchema3(organisation::parameters &parameters,
                                std::vector<organisation::vector> direction,
                                int delay)
{
    organisation::point starting(parameters.width / 2, parameters.height / 2, parameters.depth / 2);

    organisation::schema s1(parameters);

    organisation::genetic::movements::movement movement(parameters.min_movements, parameters.max_movements);
    movement.directions = direction;

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value a(delay, organisation::point(starting.x,starting.y,starting.z), movement);
    insert.values = { a };   

    organisation::genetic::cache cache(parameters);    
    organisation::genetic::collisions collisions(parameters);

    s1.prog.set(cache);
    s1.prog.set(insert);
    s1.prog.set(collisions);

    return s1;
}

TEST(BasicProgramMovementAllDirectionsBoundaryTestParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;

    std::string input1("daisy");
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);    
    parameters.dim_clients = organisation::point(2,3,1);
    parameters.iterations = 17;
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input1);

    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;    
    organisation::parallel::program program(device, &queue, mapper, parameters);
    
    EXPECT_TRUE(program.initalised());

    organisation::schema s1 = getSchema3(parameters, { { 1, 0, 0 } }, 1);
    organisation::schema s2 = getSchema3(parameters, { {-1, 0, 0 } }, 2);
    organisation::schema s3 = getSchema3(parameters, { { 0, 1, 0 } }, 3);
    organisation::schema s4 = getSchema3(parameters, { { 0,-1, 0 } }, 4);
    organisation::schema s5 = getSchema3(parameters, { { 0, 0, 1 } }, 5);
    organisation::schema s6 = getSchema3(parameters, { { 0, 0,-1 } }, 6);

    std::vector<organisation::schema*> source = { &s1,&s2,&s3,&s4,&s5,&s6 };

    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> data = program.get();

    EXPECT_TRUE(data.size() == 0);
}

TEST(BasicProgramMovementAllDirectionsPartialBoundaryParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;

    std::string input1("daisy");
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(2,3,1);
    parameters.iterations = 12;

    organisation::inputs::epoch epoch1(input1);

    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;    
    organisation::parallel::program program(device, &queue, mapper, parameters);

    EXPECT_TRUE(program.initalised());

    organisation::schema s1 = getSchema3(parameters, { { 1, 0, 0 } }, 1);
    organisation::schema s2 = getSchema3(parameters, { {-1, 0, 0 } }, 2);
    organisation::schema s3 = getSchema3(parameters, { { 0, 1, 0 } }, 3);
    organisation::schema s4 = getSchema3(parameters, { { 0,-1, 0 } }, 4);
    organisation::schema s5 = getSchema3(parameters, { { 0, 0, 1 } }, 5);
    organisation::schema s6 = getSchema3(parameters, { { 0, 0,-1 } }, 6);

    std::vector<organisation::schema*> source = { &s1,&s2,&s3,&s4,&s5,&s6 };

    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> data = program.get();

    std::vector<organisation::parallel::value> values = {
        { organisation::point(10, 1,10), organisation::point(0,-1,-1), 1, 3 },
        { organisation::point(10,10,18), organisation::point(0,-1,-1), 2, 4 },
        { organisation::point(10,10, 3), organisation::point(0,-1,-1), 3, 5 }        
    };

    EXPECT_EQ(data,values);
}

TEST(BasicProgramMovementAllDirectionsBoundaryDeleteSuccessfulAndMovementSequenceMaintainedParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;

    std::string input1("daisy");
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    parameters.dim_clients = organisation::point(2,3,1);
    parameters.iterations = 13;

    organisation::inputs::epoch epoch1(input1);

    parameters.input.push_back(epoch1);

    parallel::mapper::configuration mapper;    
    organisation::parallel::program program(device, &queue, mapper, parameters);

    EXPECT_TRUE(program.initalised());

    organisation::schema s1 = getSchema3(parameters, { { 1, 0, 0 } }, 1);
    organisation::schema s2 = getSchema3(parameters, { {-1, 0, 0 } }, 2);
    organisation::schema s3 = getSchema3(parameters, { { 0, 1, 0 }, { 1, 0, 0 } }, 3);
    organisation::schema s4 = getSchema3(parameters, { { 0,-1, 0 } }, 4);
    organisation::schema s5 = getSchema3(parameters, { { 0, 0, 1 }, { 1, 1, 0 } }, 5);
    organisation::schema s6 = getSchema3(parameters, { { 0, 0,-1 } }, 6);

    std::vector<organisation::schema*> source = { &s1,&s2,&s3,&s4,&s5,&s6 };

    program.copy(source.data(), source.size());
    program.set(mappings, parameters.input);

    program.run(mappings);

    std::vector<organisation::parallel::value> data = program.get();

    std::vector<organisation::parallel::value> values = {
        { organisation::point(15,16,10), organisation::point(0,-1,-1), 1, 2 },
        { organisation::point(10, 0,10), organisation::point(0,-1,-1), 2, 3 },
        { organisation::point(14,14,15), organisation::point(0,-1,-1), 3, 4 },
        { organisation::point(10,10, 2), organisation::point(0,-1,-1), 4, 5 }        
    };

    EXPECT_EQ(data,values);
}

