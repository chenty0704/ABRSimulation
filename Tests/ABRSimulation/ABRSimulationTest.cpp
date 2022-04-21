#include <gtest/gtest.h>

#include "ThroughputEstimators/ThroughputEstimators.h"
#include "ABRControllers/ABRControllers.h"
#include "ABRSimulation/ABRSimulation.h"

TEST(ABRSimulationTest, Basic) { // NOLINT(cert-err58-cpp)
    const auto videoModel = VideoModel::Import("Data/VideoModels/SampleVideoModel.json");
    auto networkModel = NetworkModel::Import("Data/NetworkModels/SampleNetworkModel.json");
    const std::unique_ptr<IABRController> controller = std::make_unique<ThroughputBasedController>();
    const std::unique_ptr<IThroughputEstimator> throughputEstimator = std::make_unique<ExponentialMovingAverageEstimator>();
    const auto simData = ABRSimulation::SimulateSession(videoModel, networkModel, *controller, *throughputEstimator);
    EXPECT_EQ(simData.TotalTimeInMs, 40'500);
}
