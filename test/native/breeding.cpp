#include <gtest/gtest.h>
#include <string>
#include "program.h"
#include "data.h"
#include "vector.h"
#include "schema.h"
#include "general.h"
#include "point.h"
#include "parameters.h"
#include "genetic/cache.h"
#include "genetic/movement.h"
#include "genetic/collisions.h"
#include "genetic/insert.h"

TEST(BasicProgramGenerate, BasicAssertions)
{
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input("daisy daisy give me your answer do .");        

    std::vector<std::string> strings = organisation::split(input);
    organisation::data mappings(strings);

    organisation::parameters parameters(width, height, depth);    
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input, input);
    parameters.input.push_back(epoch1);

    organisation::program p1(parameters);
     
    for(int i = 0; i < 100000; ++i)
    {
        p1.generate(mappings, parameters.input);
   
        EXPECT_FALSE(p1.empty());
        EXPECT_TRUE(p1.validate(mappings));
    }
}

TEST(BasicProgramMutate, BasicAssertions)
{
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::string input("daisy daisy give me your answer do .");        

    std::vector<std::string> strings = organisation::split(input);
    organisation::data mappings(strings);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input, input);
    parameters.input.push_back(epoch1);

    organisation::program p1(parameters);
         
    for(int i = 0; i < 100000; ++i)
    {
        organisation::program p2(parameters);

        p1.generate(mappings, parameters.input);        
        
        p2.copy(p1);
        
        EXPECT_TRUE(p1.equals(p2));

        if(p1.mutate(mappings, parameters.input))
        {
            EXPECT_FALSE(p1.equals(p2));
            EXPECT_TRUE(p1.validate(mappings));
        }
    }
}

TEST(BasicProgramCross, BasicAssertions)
{
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);
     
    std::string input("daisy daisy give me your answer do .");        

    std::vector<std::string> strings = organisation::split(input);
    organisation::data mappings(strings);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;

    organisation::inputs::epoch epoch1(input, input);
    parameters.input.push_back(epoch1);

    for(int i = 0; i < 100000; ++i)
    {      
        organisation::program p1(parameters);
        organisation::program p2(parameters);
        organisation::program p3(parameters);

        p1.generate(mappings, parameters.input);        
        p2.generate(mappings, parameters.input);

        p3.cross(p1, p2);
        
        EXPECT_FALSE(p3.equals(p1));
        EXPECT_FALSE(p3.equals(p2));
        EXPECT_TRUE(p3.validate(mappings));
    }
}
