#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "ThroughputEstimators/IThroughputEstimator.h"
#include "ABRControllers/IABRController.h"

struct SessionOptions {
    size_t MaxBufferSegmentCount = 10;
};

struct SimulationData {
    double TotalTimeInMs = 0;
    double MaxBufferLevelInMs;
    std::vector<std::optional<double>> BufferedBitRatesInKbps;
    std::vector<double> DownloadDurationsInMs, DownloadBitRatesInKbps;
    std::vector<double> BufferTimesInMs = {0}, BufferLevelsInMs = {0};
    std::vector<std::pair<double, double>> RebufferingPeriodsInMs;
    std::vector<double> FullBufferDelaysInMs;
};

namespace ABRSimulation {
    SimulationData SimulateSession(const VideoModel &videoModel,
                                   NetworkModel &networkModel,
                                   IABRController &controller,
                                   IThroughputEstimator &throughputEstimator,
                                   const SessionOptions &opts = {});
}
