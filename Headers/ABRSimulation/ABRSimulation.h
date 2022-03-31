#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "ThroughputEstimators/ThroughputEstimator.h"
#include "ABRControllers/ABRController.h"

struct SessionOptions {
    size_t MaxBufferSegmentCount;
};

struct SimulationData {
    size_t TotalTimeInMs = 0;
    std::vector<std::optional<size_t>> BufferedBitRatesInKbps;
    std::vector<size_t> DownloadDurationsInMs, DownloadBitRatesInKbps;
    std::vector<size_t> BufferTimesInMs = {0}, BufferLevelsInMs = {0};
    std::vector<size_t> RebufferingDurationsInMs;
    std::vector<size_t> FullBufferDelaysInMs;

    explicit SimulationData(size_t videoSegmentCount) : BufferedBitRatesInKbps(videoSegmentCount) {}
};

namespace ABRSimulation {
    SimulationData SimulateSession(const VideoModel &videoModel,
                                   NetworkModel &networkModel,
                                   ABRController &controller,
                                   ThroughputEstimator &throughputEstimator,
                                   const SessionOptions &opts = {});
}
