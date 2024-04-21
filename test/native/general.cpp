#include <gtest/gtest.h>
#include <string>
#include "program.h"
#include "data.h"
#include "vector.h"
#include "schema.h"
#include "general.h"
#include "point.h"
#include "genetic/cache.h"
#include "genetic/movement.h"
#include "genetic/collisions.h"
#include "genetic/insert.h"

/*
TEST(BasicMovementAndCollisionDetection, BasicAssertions)
{
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

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

    for(auto &it: directions)
    {        
        organisation::program p(width, height, depth);

        std::string input("daisy daisy give me your answer do .");
        std::vector<std::string> expected = organisation::split("daisy daisy daisy daisy me daisy daisy do");

        std::vector<std::string> strings = organisation::split(input);
        organisation::data d(strings);
        
        organisation::genetic::cache cache(width, height, depth);
        cache.set(0, std::get<0>(it));
        
        organisation::genetic::inserts::insert insert;
        insert.values = { 1,2,3 };

        organisation::genetic::movement movement;
        movement.directions = { std::get<1>(it) };

        organisation::genetic::collisions collisions;

        collisions.values.resize(27);
        organisation::vector up = std::get<1>(it);
        organisation::vector rebound = std::get<2>(it);
        collisions.values[up.encode()] = rebound.encode();

        p.set(cache);
        p.set(insert);
        p.set(movement);
        p.set(collisions);

        std::string output = p.run(input, d);
        std::vector<std::string> outputs = organisation::split(output);
        
        EXPECT_EQ(outputs, expected);
    }
}
*/