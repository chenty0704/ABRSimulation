#include <gtest/gtest.h>

#include "NetworkModel/NetworkModel.h"

TEST(NetworkModelTest, Basic) {
    auto networkModel = NetworkModel::Import("Data/Samples/SampleNetworkModel.json");
    EXPECT_EQ(networkModel.Download(500'000).TimeInMs, 500);
    EXPECT_EQ(networkModel.Download(1'500'000).TimeInMs, 1'000);
    networkModel.Delay(1'000);
    EXPECT_EQ(networkModel.Download(2'250'000).TimeInMs, 1'500);
    networkModel.Delay(500);
    EXPECT_EQ(networkModel.Download(2'250'000).TimeInMs, 2'000);
}
