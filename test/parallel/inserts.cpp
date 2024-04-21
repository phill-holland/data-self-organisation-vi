#include <gtest/gtest.h>
#include "parallel/inserts.hpp"
#include "parallel/program.hpp"
#include "parallel/map/configuration.hpp"
#include "parallel/value.hpp"
#include "general.h"
#include "data.h"
#include "schema.h"
#include "kdpoint.h"
#include <unordered_map>
#include <tuple>
#include <vector>

TEST(BasicProgramInsertThreeInputsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(2, organisation::point(starting.x + 1,starting.y,starting.z));
    organisation::genetic::inserts::value c(2, organisation::point(starting.x + 2,starting.y,starting.z));

    insert.values = { a, b, c };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;

    expected0[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(0,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(0,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(1,-1,-1), 0, 0 }
    };

    expected0[5] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(2,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(3,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(4,-1,-1), 0, 0 }
    };

    expected0[8] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(5,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(6,-1,-1), 0, 0 }
    };

    std::unordered_map<int,std::vector<organisation::parallel::value>> expected1;

    expected1[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(7,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(7,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(8,-1,-1), 0, 0 }
    };

    expected1[5] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(9,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(10,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(6,-1,-1), 0, 0 }
    };

    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0, &expected1 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertThreeInputsOffsetDelayParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(3, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(2, organisation::point(starting.x + 1,starting.y,starting.z));
    organisation::genetic::inserts::value c(0, organisation::point(starting.x + 2,starting.y,starting.z));

    insert.values = { a, b, c };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;

    expected0[0] = std::vector<organisation::parallel::value> { 
        { organisation::point(7,5,5), organisation::point(0,-1,-1), 0, 0 }, 
    };

    expected0[1] = std::vector<organisation::parallel::value> { 
        { organisation::point(7,5,5), organisation::point(0,-1,-1), 0, 0 }
    };

    expected0[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(1,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(2,-1,-1), 0, 0 }
    };

    expected0[3] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(3,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(4,-1,-1), 0, 0 }
    };

    expected0[4] = std::vector<organisation::parallel::value> { 
        { organisation::point(7,5,5), organisation::point(5,-1,-1), 0, 0 }        
    };

    expected0[5] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(6,-1,-1), 0, 0 }        
    };
   
    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertThreeInputsOffsetDelayAndBulkWordsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(input1);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    parameters.input.push_back(epoch1);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(0, organisation::point(starting.x + 1,starting.y,starting.z));    

    a.words = 3;

    insert.values = { a, b };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;

    expected0[0] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(0,-1,-1), 0, 0 }, 
    };

    expected0[1] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(0,-1,-1), 0, 0 }
    };

    expected0[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(1,2,3), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(4,-1,-1), 0, 0 }
    };

    expected0[3] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(5,-1,-1), 0, 0 }        
    };

    expected0[4] = std::vector<organisation::parallel::value> { 
        { organisation::point(6,5,5), organisation::point(6,-1,-1), 0, 0 }        
    };
   
    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertThreeInputsTwoSchemasParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(2,1,1);

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(2, organisation::point(starting.x + 1,starting.y,starting.z));
    organisation::genetic::inserts::value c(2, organisation::point(starting.x + 2,starting.y,starting.z));

    insert.values = { a, b, c };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1, &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;

    expected0[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(0,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(0,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(1,-1,-1), 0, 0 },
        { organisation::point(5,5,5), organisation::point(0,-1,-1), 0, 1 }, 
        { organisation::point(6,5,5), organisation::point(0,-1,-1), 0, 1 }, 
        { organisation::point(7,5,5), organisation::point(1,-1,-1), 0, 1 }
    };

    expected0[5] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(2,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(3,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(4,-1,-1), 0, 0 },
        { organisation::point(5,5,5), organisation::point(2,-1,-1), 0, 1 }, 
        { organisation::point(6,5,5), organisation::point(3,-1,-1), 0, 1 }, 
        { organisation::point(7,5,5), organisation::point(4,-1,-1), 0, 1 }
    };

    expected0[8] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(5,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(6,-1,-1), 0, 0 },
        { organisation::point(5,5,5), organisation::point(5,-1,-1), 0, 1 }, 
        { organisation::point(6,5,5), organisation::point(6,-1,-1), 0, 1 }
    };

    std::unordered_map<int,std::vector<organisation::parallel::value>> expected1;

    expected1[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(7,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(7,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(8,-1,-1), 0, 0 },
        { organisation::point(5,5,5), organisation::point(7,-1,-1), 0, 1 }, 
        { organisation::point(6,5,5), organisation::point(7,-1,-1), 0, 1 }, 
        { organisation::point(7,5,5), organisation::point(8,-1,-1), 0, 1 }
    };

    expected1[5] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(9,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(10,-1,-1), 0, 0 }, 
        { organisation::point(7,5,5), organisation::point(6,-1,-1), 0, 0 },
        { organisation::point(5,5,5), organisation::point(9,-1,-1), 0, 1 }, 
        { organisation::point(6,5,5), organisation::point(10,-1,-1), 0, 1 }, 
        { organisation::point(7,5,5), organisation::point(6,-1,-1), 0, 1 }
    };

    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0, &expected1 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            std::unordered_map<int,std::vector<organisation::parallel::value>> output;
            for(auto &it: data)
            {
                output[it.client].push_back(it);
            }

            std::vector<organisation::parallel::value> sorted = output[0];
            sorted.insert(std::end(sorted), std::begin(output[1]), std::end(output[1]));

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), sorted);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertFourInputsTwoWordsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy answer");
    std::string input2("monkey face");

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(2, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(2, organisation::point(starting.x + 1,starting.y,starting.z));
    organisation::genetic::inserts::value c(2, organisation::point(starting.x + 2,starting.y,starting.z));
    organisation::genetic::inserts::value d(2, organisation::point(starting.x + 3,starting.y,starting.z));

    insert.values = { a, b, c, d };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;

    expected0[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(0,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(1,-1,-1), 0, 0 }        
    };    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected1;

    expected1[2] = std::vector<organisation::parallel::value> { 
        { organisation::point(5,5,5), organisation::point(2,-1,-1), 0, 0 }, 
        { organisation::point(6,5,5), organisation::point(3,-1,-1), 0, 0 }
    };

    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0, &expected1 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertSingleInputParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(1, organisation::point(starting.x,starting.y,starting.z));

    insert.values = { a };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;
    std::vector<int> data0 = { 0, 0, 1, 2, 3, 4, 5, 6 };

    int offset = 1;
    for(auto &it: data0)
    {
        expected0[offset] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(it,-1,-1), 0, 0 },
        };

        offset += 2;
    }
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected1;
    std::vector<int> data1 = { 7, 7, 8, 9, 10, 6 };

    offset = 1;
    for(auto &it: data1)
    {
        expected1[offset] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(it,-1,-1), 0, 0 },
        };

        offset += 2;
    }

    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0, &expected1 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}

TEST(BasicProgramInsertSingleInputManyWordsParallel, BasicAssertions)
{    
    GTEST_SKIP();

    const int width = 10, height = 10, depth = 10;

    std::string input1("daisy daisy give me your answer do .");
    std::string input2("monkey monkey eat my face .");

    std::vector<std::string> strings = organisation::split(input1 + " " + input2);
    organisation::data mappings(strings);

	::parallel::device device(0);
	::parallel::queue queue(device);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;
    
    parameters.dim_clients = organisation::point(1,1,1);

    organisation::inputs::epoch epoch1(input1);
    organisation::inputs::epoch epoch2(input2);

    parameters.input.push_back(epoch1);
    parameters.input.push_back(epoch2);

    organisation::parallel::inserts inserts(device, &queue, parameters);

    organisation::schema s1(parameters);

    organisation::genetic::inserts::insert insert(parameters);

    organisation::point starting(width/2, height/2, depth/2);

    organisation::genetic::inserts::value a(1, organisation::point(starting.x,starting.y,starting.z));
    organisation::genetic::inserts::value b(1, organisation::point(starting.x + 1,starting.y,starting.z));

    a.words = 3;
    b.words = 2;

    insert.values = { a, b };
    s1.prog.set(insert);

    std::vector<organisation::schema*> source = { &s1 };
    
    inserts.copy(source.data(), source.size());
    inserts.set(mappings, parameters.input);
    
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected0;
    
    expected0[1] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(0,0,1), 0, 0 },
            { organisation::point(6,5,5), organisation::point(2,3,-1), 0, 0 },
        };

    expected0[3] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(4,5,6), 0, 0 }
        };
        
    std::unordered_map<int,std::vector<organisation::parallel::value>> expected1;

    expected1[1] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(7,7,8), 0, 0 },
            { organisation::point(6,5,5), organisation::point(9,10,-1), 0, 0 },
        };

    expected1[3] = std::vector<organisation::parallel::value> { 
            { organisation::point(5,5,5), organisation::point(6,-1,-1), 0, 0 }
        };
    
    std::vector<std::unordered_map<int,std::vector<organisation::parallel::value>>*> epochs = { &expected0, &expected1 };
    
    for(int epoch = 0; epoch < epochs.size(); ++epoch)
    {
        inserts.restart();

        std::unordered_map<int,std::vector<organisation::parallel::value>> *current = epochs[epoch];
        for(int i = 0; i < 15; ++i)
        {
            int count = inserts.insert(epoch, i);    
            auto data = inserts.get();

            if(count > 0)
            {
                EXPECT_TRUE((*current).find(i) != (*current).end());
                EXPECT_EQ(count, ((*current)[i]).size());
                EXPECT_EQ(((*current)[i]), data);
            }
            else
            {
                EXPECT_TRUE((*current).find(i) == (*current).end());
            }
        }        
    }    
}