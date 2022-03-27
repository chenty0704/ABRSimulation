#include <gtest/gtest.h>

#include "ThroughputEstimators/ThroughputEstimators.h"
#include "ABRControllers/ABRControllers.h"
#include "ABRSimulation/ABRSimulation.h"

TEST(ABRSimulationTest, Basic) {
    const auto videoModel = VideoModel::Import("Data/Samples/SampleVideoModel.json");
    auto networkModel = NetworkModel::Import("Data/Samples/SampleNetworkModel.json");
    const std::unique_ptr<ABRController> controller = std::make_unique<ThroughputBasedController>();
    const std::unique_ptr<ThroughputEstimator> throughputEstimator = std::make_unique<ExponentialMovingAverageEstimator>();
    const auto simData = ABRSimulation::SimulateSession(videoModel, networkModel, *controller, *throughputEstimator);
    EXPECT_EQ(simData.TotalTimeInMs, 18'665);
}
