#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "ThroughputEstimators/ThroughputEstimator.h"
#include "ABRControllers/ABRController.h"

struct SessionOptions {
    size_t MaxBufferSegmentCount = 5;
};

struct SimulationData {
    double TotalTimeInMs = 0;
    std::vector<std::optional<double>> BufferedBitRatesInKbps;
    std::vector<double> DownloadDurationsInMs, DownloadBitRatesInKbps;
    std::vector<double> BufferTimesInMs = {0}, BufferLevelsInMs = {0};
    std::vector<std::pair<double, double>> RebufferingPeriodsInMs;
    std::vector<double> FullBufferDelaysInMs;

    explicit SimulationData(size_t videoSegmentCount) : BufferedBitRatesInKbps(videoSegmentCount) {}
};

namespace ABRSimulation {
    SimulationData SimulateSession(const VideoModel &videoModel,
                                   NetworkModel &networkModel,
                                   ABRController &controller,
                                   ThroughputEstimator &throughputEstimator,
                                   const SessionOptions &opts = {});
}
