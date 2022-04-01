#include <gtest/gtest.h>

#include "Core/Meta.h"

TEST(MetaTest, ForEachRef) { // NOLINT(cert-err58-cpp)
    auto a = 1, b = 2;
    ForEachRef([](auto &x) { ++x; }, a, b);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 3);

    const auto c = 1, d = 2;
    auto sum = 0;
    ForEachRef([&](const auto &x) { sum += x; }, c, d);
    EXPECT_EQ(sum, 3);
}

TEST(MetaTest, ForEachRefUntil) { // NOLINT(cert-err58-cpp)
    auto a = 1, b = 2, c = -1, d = 4;
    ForEachRefUntil([](auto &x) { return x <= 0 || (++x, false); }, a, b, c, d);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 3);
    EXPECT_EQ(c, -1);
    EXPECT_EQ(d, 4);
}

TEST(MetaTest, ForEachRefIndexed) { // NOLINT(cert-err58-cpp)
    auto a = 1, b = 2;
    const std::array values = {1, 2};
    ForEachRefIndexed([&](size_t i, auto &x) { x += values[i]; }, a, b);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 4);
}
