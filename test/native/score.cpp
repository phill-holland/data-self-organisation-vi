#include <gtest/gtest.h>
#include <vector.h>
#include "score.h"
#include "general.h"
#include "compute.h"

TEST(BasicScoreEqualsOneParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy give me your answer do .");
    std::string expected("daisy daisy give me your answer do .");

    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);

    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, 1.0f ,
                                1.0f, 1.0f, 1.0f  };

    EXPECT_EQ(values, data);
    EXPECT_FLOAT_EQ(score.sum(), 1.0f);
}

TEST(BasicScoreOneOffParallel, BasicAssertions)
{    
    GTEST_SKIP();
    
    std::string value("daisy daisy monkey me your answer do .");
    std::string expected("daisy daisy give me your answer do .");

    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                                0.0f, 1.0f, 1.0f, 1.0f, 1.0f ,
                                1.0f, 1.0f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.83333331f);
}

TEST(BasicScoreOneOffErrorParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy me give your answer do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.857142866f, 0.100000001f, 1.0f,
                                1.0f, 1.0f, 1.0f, 1.0f, 0.857142866f,
                                0.0f, 0.857142866f, 1.0f, 1.0f, 1.0f ,
                                1.0f, 1.0f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.87063485f);
}

TEST(BasicScoreTwoOffErrorParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy me your give answer do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.714285731f, 0.100000001f, 0.100000001f, 1.0f, 1.0f, 1.0f, 
                                1.0f, 0.714285731f, 0.0f, 1.0, 0.857142866f, 1.0f, 1.0f , 1.0f, 
                                1.0f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.80476189f);
}

TEST(BasicScoreThreeOffErrorParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy me your answer give do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.571428537f, 0.100000001f, 0.100000001f, 0.100000001f, 1.0f, 1.0f, 
                                1.0f, 0.571428537f, 0.0f, 1.0, 1.0f, 0.857142866f, 1.0f, 1.0f, 
                                1.0f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.73888886f);
}

TEST(BasicScoreRepeatOneParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy daisy give me your answer do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.857142866f, 0.857142866f, 0.857142866f, 0.857142866f, 0.857142866f, 0.857142866f, 
                                1.0f, 0.857142866f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 
                                0.5f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.91666657f);
}

TEST(BasicScoreRepeatTwoParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy daisy daisy give me your answer do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.714285731f, 0.714285731f, 0.714285731f, 0.714285731f, 0.714285731f, 0.714285731f, 
                                1.0f, 0.714285731f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 
                                0.33333334f, 1.0f  };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.85185188f);
}

TEST(BasicScoreRepeatThreeParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy daisy daisy daisy daisy give me your answer do .");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 1.0f, 0.571428537f, 0.571428537f, 0.571428537f, 0.571428537f, 0.571428537f, 0.571428537f, 
                                1.0f, 0.571428537f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 
                                0.200000003f, 1.0f };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.78888875f);
}

TEST(BasicScoreShortSentenceParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("daisy");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);

    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 1.0f, 0.100000001f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
                                0.0294117648f, 1.0f };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.11830065f);
}

TEST(BasicScoreBadOrderOneParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("me your answer do . daisy daisy give");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);

    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 0.285714269f, 0.428571403f, 0.285714269f, 0.100000001f, 0.100000001f, 0.100000001f, 0.100000001f, 0.100000001f,
                                0.0f, 0.857142866f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.571428537f, 
                                1.0f, 1.0f };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.49603164f);
}

TEST(BasicScoreIncorrectParallel, BasicAssertions)
{    
    GTEST_SKIP();

    std::string value("incorrect");
    std::string expected("daisy daisy give me your answer do .");
    
    std::vector<std::string> strings = organisation::split(expected);

    organisation::scores::score score;
    organisation::scores::settings settings;

    organisation::statistics::data statistics(settings.max_collisions);


    score.compute(organisation::compute(expected, value, statistics), settings);

    std::vector<float> values;
    int total = score.size();

    EXPECT_EQ(total, (strings.size() * 2) + 2);

    for(int i = 0; i < total; ++i)
    {
        values.push_back(score.get(i));
    }

    std::vector<float> data = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                0.0294117648f, 1.0f };

    for(int i = 0; i < total; ++i)
    {
        EXPECT_FLOAT_EQ(values[i], data[i]);
    }

    EXPECT_FLOAT_EQ(score.sum(), 0.057189543f);
}