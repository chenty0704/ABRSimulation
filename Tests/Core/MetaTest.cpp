#include <gtest/gtest.h>

#include "Core/Core.h"
#include "Core/Meta.h"

TEST(MetaTest, ForEachRef) {
    auto a = 1, b = 2;
    ForEachRef([](auto &x) { ++x; }, a, b);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 3);
}

TEST(MetaTest, ForEachRefUntil) {
    auto a = 1, b = 2, c = -1, d = 4;
    ForEachRefUntil([](auto &x) { return x <= 0 || (++x, false); }, a, b, c, d);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 3);
    EXPECT_EQ(c, -1);
    EXPECT_EQ(d, 4);
}

TEST(MetaTest, ForEachRefIndexed) {
    auto a = 1, b = 2;
    constexpr std::array values = {1, 2};
    ForEachRefIndexed([&](size_t i, auto &x) { x += values[i]; }, a, b);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 4);
}
