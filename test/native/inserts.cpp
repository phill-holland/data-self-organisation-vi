#include <gtest/gtest.h>
#include <vector>
#include "point.h"
#include "parameters.h"
#include "genetic/insert.h"
#include "vector.h"

TEST(BasicProgramInsertMovementsGetFunctionReturnsCorrectData, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    const organisation::point starting(width / 2, height / 2, depth / 2);

    organisation::parameters parameters(width, height, depth);

    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement1.directions = { 
        { 0,1,0 },
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
        {  0,-1,0 },  
        { -1, 0,0 }, 
        { -1, 0,0 }, 
        { -1, 0,0 }, 
        { -1, 0,0 },
        { -1, 0,0 }
    };
      
    std::vector<std::tuple<int,organisation::vector>> expected;
    for(auto &it:movement1.directions)
    {
        expected.push_back(std::tuple<int,organisation::vector>(0,it));
    }

    for(auto &it:movement2.directions)
    {
        expected.push_back(std::tuple<int,organisation::vector>(1,it));
    }

    organisation::genetic::inserts::insert insert(parameters);
    organisation::genetic::inserts::value i1(1, organisation::point(starting.x - 3,starting.y - 3,starting.z), movement1);
    organisation::genetic::inserts::value i2(1, organisation::point(starting.x + 3,starting.y + 3,starting.z), movement2);
    insert.values = { i1, i2 };
        
    EXPECT_EQ(insert.size(), 16);

    std::vector<std::tuple<int,organisation::vector>> results;

    for(int i = 0; i < insert.size() + 2; ++i)
    {
        std::tuple<int,organisation::vector> temp;
        if(insert.get(temp, i)) results.push_back(temp);
    }

    EXPECT_EQ(results.size(), insert.size());
    EXPECT_EQ(results, expected);   
}