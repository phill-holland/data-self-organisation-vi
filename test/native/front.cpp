#include <gtest/gtest.h>
#include "kdpoint.h"

TEST(BasicFrontTest, BasicAssertions)
{
    GTEST_SKIP();

    int points[][2] = { {97,23},{55,77},{34,76},{80,60},{99,04},{81,05},{05,81},{30,79},{15,80},{70,65},
                        {90,40},{40,30},{30,40},{20,60},{60,50},{20,20},{30,01},{60,40},{70,25},{44,62},
                        {55,55},{55,10},{15,45},{83,22},{76,46},{56,32},{45,55},{10,70},{10,30},{97,23}
                      };
    
	const int count = 30;
    const int terms = 2;

    bool expected[count];
    for(int i = 0; i < count; ++i) expected[i] = false;

    expected[0] = true;
    expected[1] = true;
    expected[3] = true;
    expected[4] = true;
    expected[6] = true;
    expected[7] = true;
    expected[8] = true;
    expected[9] = true;
    expected[10] = true;
    expected[29] = true;

    bool result[count];
    for(int i = 0; i < count; ++i) result[i] = true;

    organisation::kdpoint a(2);
    organisation::kdpoint b(2);

    for(int i = 0; i < count; ++i)
    {
        a.set(points[i][0],0);
        a.set(points[i][1],1);

        for(int j = 0; j < count; ++j)
        {
            if(i != j)
            {
                b.set(points[j][0],0);
                b.set(points[j][1],1);

                if(b.dominates(a)) 
                {
                    result[i] = false;
                    break;
                }
            }    
        }
    }

    for(int i = 0; i < count; ++i)
    {
        EXPECT_EQ(result[i], expected[i]);
    }
}