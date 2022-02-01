#include <gtest/gtest.h>

#include "ThroughputEstimators.h"

TEST(ExponentialMovingAverageEstimatorTest, ConstantThroughput) {
    ExponentialMovingAverageEstimator estimator;
    estimator.Push({.TimeInMs = 100, .DownloadedBitCount = 20'000});
    EXPECT_EQ(estimator.EstimateInKbps(), 200);
    estimator.Push({.TimeInMs = 150, .DownloadedBitCount = 30'000});
    EXPECT_EQ(estimator.EstimateInKbps(), 200);
    estimator.Push({.TimeInMs = 250, .DownloadedBitCount = 50'000});
    EXPECT_EQ(estimator.EstimateInKbps(), 200);
}

TEST(ExponentialMovingAverageEstimatorTest, VariableThroughput) {
    ExponentialMovingAverageEstimator estimator;
    estimator.Push({.TimeInMs = 1'000, .DownloadedBitCount = 200'000});
    estimator.Push({.TimeInMs = 1'000, .DownloadedBitCount = 0});
    EXPECT_LT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .DownloadedBitCount = 200'000});
    EXPECT_GT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .DownloadedBitCount = 0});
    EXPECT_LT(estimator.EstimateInKbps(), 100);
    estimator.Push({.TimeInMs = 1'000, .DownloadedBitCount = 200'000});
    EXPECT_GT(estimator.EstimateInKbps(), 100);
}
