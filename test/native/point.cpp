#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "point.h"

TEST(PointGenerateData, BasicAssertions)
{    
    GTEST_SKIP();

    std::vector<int> data = { 0,1,2,3,4,5,6,7,8,9 };
    int counts[] = { 0, 0, 0 };

    const int total = 10000;
    for(int i = 0; i < total; ++i)
    {
        organisation::point point;
        point.generate(data);

        int coordinates[] = { point.x, point.y, point.z };

        EXPECT_FALSE((point.y != -1)&&(point.x == -1));
        EXPECT_FALSE((point.z != -1)&&(point.y == -1));

        int count = 0;
        for(int j = 0; j < 3; ++j)
        {
            if(coordinates[j] != -1) 
            {
                EXPECT_FALSE(std::find(data.begin(), data.end(), coordinates[j]) == data.end());
                
                counts[j]++;                
                ++count;
            }
        }

        EXPECT_GT(count, 0);
        EXPECT_LT(count, 4);
    }

    for(int j = 0; j < 3; ++j)
    {
        EXPECT_GT(counts[j], total / 4);
    }
}