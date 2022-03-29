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
    size_t TotalTimeInMs = 0;
    std::vector<std::optional<size_t>> BufferedBitRatesInKbps;
    std::vector<size_t> RebufferingDurationsInMs;
    std::vector<size_t> FullBufferDelaysInMs;
    std::vector<size_t> DownloadDurationsInMs;
    std::vector<size_t> DownloadBitRatesInKbps;
};

namespace ABRSimulation {
    SimulationData SimulateSession(const VideoModel &videoModel,
                                   NetworkModel &networkModel,
                                   ABRController &controller,
                                   ThroughputEstimator &throughputEstimator,
                                   const SessionOptions &opts = {});
}
