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
#include "genetic/links.h"

TEST(BasicSerialisationDeserialisation, BasicAssertions)
{
    GTEST_SKIP();

    const int width = 20, height = 20, depth = 20;
    organisation::point starting(width / 2, height / 2, depth / 2);

    std::vector<std::string> strings = { "daisy", "give" };
    organisation::data mappings(strings);

    organisation::parameters parameters(width, height, depth);
    parameters.mappings = mappings;

    organisation::program p1(parameters);
    organisation::program p2(parameters);

    organisation::genetic::cache cache(parameters);
    cache.set(organisation::point(0,-1,-1), starting);
    cache.set(organisation::point(0,1,-1), organisation::point(starting.x + 1, starting.y, starting.z));
    cache.set(organisation::point(0,2,3), organisation::point(starting.x + 2, starting.y, starting.z));
    
    organisation::genetic::inserts::insert insert(parameters);

    organisation::genetic::movements::movement movement0(parameters.min_movements, parameters.max_movements);
    organisation::genetic::movements::movement movement1(parameters.min_movements, parameters.max_movements);
    movement0.directions = { { 1,0,0 }, { 0,0,1 } };
    movement1.directions = { { 0,1,0 }, { -1,0,0 }, { 0,0,-1} };

    organisation::genetic::inserts::value a(1, organisation::point(starting.x,starting.y,starting.z), movement0, 1);
    organisation::genetic::inserts::value b(4, organisation::point(starting.x + 1,starting.y,starting.z), movement1, 2);
    organisation::genetic::inserts::value c(7, organisation::point(starting.x + 2,starting.y,starting.z), movement0, 3);

    insert.values = { a, b, c };

    organisation::genetic::collisions collisions(parameters);

    organisation::vector up = { 1,0,0 };
    organisation::vector rebound = { 0,1,0 };

    collisions.set(rebound.encode(),up.encode());
    collisions.set(rebound.encode(),up.encode() + parameters.max_collisions);
    
    organisation::genetic::links links(parameters);

    links.set(organisation::point(1,2,3), 0);
    links.set(organisation::point(3,4,5), 1);

    p1.set(cache);
    p1.set(insert);
    p1.set(collisions);
    p1.set(links);

    std::string data = p1.serialise();
    p2.deserialise(data);

    EXPECT_TRUE(p1.equals(p2));
}
