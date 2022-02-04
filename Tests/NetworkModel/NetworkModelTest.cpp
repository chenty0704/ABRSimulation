#include <gtest/gtest.h>

#include "NetworkModel/NetworkModel.h"

TEST(NetworkModelTest, Basic) {
    auto networkModel = NetworkModel::Import("Data/Samples/SampleNetworkModel.json");
    EXPECT_EQ(networkModel.Download(62'500).TimeInMs, 500);
    EXPECT_EQ(networkModel.Download(187'500).TimeInMs, 1'000);
    networkModel.Delay(1'000);
    EXPECT_EQ(networkModel.Download(281'250).TimeInMs, 1'500);
    networkModel.Delay(500);
    EXPECT_EQ(networkModel.Download(281'250).TimeInMs, 2'000);
}
