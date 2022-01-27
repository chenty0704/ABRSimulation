#include <gtest/gtest.h>

#include "NetworkModel.h"

TEST(NetworkModelTest, Basic) {
    const auto durationsInMs = {1'000, 1'000, 2'000, 1'000};
    const auto bandwidthsInKBps = {10, 20, 15, 5};
    NetworkModel networkModel(durationsInMs.begin(), durationsInMs.end(),
                              bandwidthsInKBps.begin(), bandwidthsInKBps.end());
    EXPECT_EQ(networkModel.Download(5'000).TimeInMs, 500);
    EXPECT_EQ(networkModel.Download(15'000).TimeInMs, 1'000);
    networkModel.Delay(1'000);
    EXPECT_EQ(networkModel.Download(22'500).TimeInMs, 1'500);
    networkModel.Delay(500);
    EXPECT_EQ(networkModel.Download(22'500).TimeInMs, 2'000);
}
