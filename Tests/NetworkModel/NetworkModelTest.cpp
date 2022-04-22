#include <gtest/gtest.h>

#include "NetworkModel/NetworkModel.h"

TEST(NetworkModelTest, Basic) {
    auto networkModel = NetworkModel::Import("Data/NetworkModels/SampleNetworkModel.json");
    EXPECT_EQ(networkModel.Download(250'000).TimeInMs, 500);
    EXPECT_EQ(networkModel.Download(750'000).TimeInMs, 1'000);
    networkModel.Delay(1'000);
    EXPECT_EQ(networkModel.Download(1'125'000).TimeInMs, 1'500);
    networkModel.Delay(500);
    EXPECT_EQ(networkModel.Download(1'125'000).TimeInMs, 2'000);
}
