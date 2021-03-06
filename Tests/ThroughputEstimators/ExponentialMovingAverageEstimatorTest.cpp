#include <gtest/gtest.h>

#include "ThroughputEstimators/ExponentialMovingAverageEstimator.h"

TEST(ExponentialMovingAverageEstimatorTest, ConstantThroughput) {
    ExponentialMovingAverageEstimator estimator;
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 25'000});
    EXPECT_DOUBLE_EQ(estimator.EstimateInKbps(), 200);
    estimator.Push({.TimeInMs = 1'500, .ByteCount = 37'500});
    EXPECT_DOUBLE_EQ(estimator.EstimateInKbps(), 200);
    estimator.Push({.TimeInMs = 2'500, .ByteCount = 62'500});
    EXPECT_DOUBLE_EQ(estimator.EstimateInKbps(), 200);
}

TEST(ExponentialMovingAverageEstimatorTest, VariableThroughput) {
    ExponentialMovingAverageEstimator estimator;
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 25'000});
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 0});
    EXPECT_LT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 25'000});
    EXPECT_GT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 0});
    EXPECT_LT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .ByteCount = 25'000});
    EXPECT_GT(estimator.EstimateInKbps(), 100);
}
