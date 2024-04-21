#include <gtest/gtest.h>
#include <vector.h>
#include "general.h"

TEST(BasicScoreBowAlmostSimilarParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string a = "crazy for the the love crazy";
    std::string b = "crazy for the crazy for the";

    float result = organisation::compare_bow(a,b);

    EXPECT_FLOAT_EQ(result, 0.912871f);    
}

TEST(BasicScoreBowExactMatchParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string a = "crazy for the the love crazy";
    std::string b = "crazy for the the love crazy";

    float result = organisation::compare_bow(a,b);

    EXPECT_FLOAT_EQ(result, 1.0f);    
}

TEST(BasicScoreBowSpecialCaseMatchParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string a = "of you I";
    std::string b = "of you I";

    float result = organisation::compare_bow(a,b);

    EXPECT_FLOAT_EQ(result, 1.0f);    
}

TEST(BasicScoreBowNoMatchParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string a = "crazy for the the love crazy";
    std::string b = "banana walnut monkey";

    float result = organisation::compare_bow(a,b);

    EXPECT_FLOAT_EQ(result, 0.0f);    
}

TEST(BasicScoreBowSingleMatchParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string a = "banana walnut monkey crazy";
    std::string b = "crazy for the the love crazy";
    
    float result = organisation::compare_bow(a,b);

    EXPECT_FLOAT_EQ(result, 0.316227764f);    
}