#include <gtest/gtest.h>

#include "ThroughputEstimators/ThroughputEstimators.h"
#include "ABRControllers/ABRControllers.h"
#include "ABRSimulation/ABRSimulation.h"

TEST(ABRSimulationTest, Basic) {
    const VideoModel videoModel{
            .SegmentDurationInMs = 2'000,
            .EncodingBitRatesInKbps = {1'000, 2'000, 5'000, 10'000},
            .SegmentByteCounts = {
                    {250'000, 500'000, 1'250'000, 2'500'000},
                    {250'000, 500'000, 1'250'000, 2'500'000},
                    {250'000, 500'000, 1'250'000, 2'500'000},
                    {250'000, 500'000, 1'250'000, 2'500'000},
                    {250'000, 500'000, 1'250'000, 2'500'000}
            }
    };
    NetworkModel networkModel;
    networkModel.DurationsInMs = {1'000, 1'000, 2'000, 1'000};
    networkModel.ThroughputsInKbps = {4'000, 8'000, 6'000, 2'000};
    const std::unique_ptr<IABRController> controller = std::make_unique<ThroughputBasedController>();
    const std::unique_ptr<IThroughputEstimator> throughputEstimator = std::make_unique<ExponentialMovingAverageEstimator>();
    const auto simData = ABRSimulation::SimulateSession(videoModel, networkModel, *controller, *throughputEstimator);
    EXPECT_EQ(simData.TotalTimeInMs, 10'500);
}
