#include <gtest/gtest.h>

#include "NetworkModel/NetworkModel.h"

TEST(NetworkModelTest, Basic) {
    NetworkModel networkModel;
    networkModel.DurationsInMs = {1'000, 1'000, 2'000, 1'000};
    networkModel.ThroughputsInKbps = {4'000, 8'000, 6'000, 2'000};

    EXPECT_EQ(networkModel.Download(250'000).TimeInMs, 500);
    EXPECT_EQ(networkModel.Download(750'000).TimeInMs, 1'000);
    networkModel.Delay(1'000);
    EXPECT_EQ(networkModel.Download(1'125'000).TimeInMs, 1'500);
    networkModel.Delay(500);
    EXPECT_EQ(networkModel.Download(1'125'000).TimeInMs, 2'000);
}
