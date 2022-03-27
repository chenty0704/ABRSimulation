#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "NetworkModel/NetworkModel.h"
#include "ThroughputEstimators/ThroughputEstimator.h"
#include "ABRControllers/ABRController.h"

struct SimulationData {
    size_t TotalTimeInMs = 0;
    std::vector<std::optional<size_t>> DownloadedBitRatesInKbps;
    std::vector<size_t> RebufferingDurationsInMs;
    std::vector<size_t> FullBufferDelaysInMs;
};

namespace ABRSimulation {
    struct SimulationSessionOptions {
        size_t MaxBufferSegmentCount = 5;
    };

    SimulationData SimulateSession(const VideoModel &videoModel,
                                   NetworkModel &networkModel,
                                   ABRController &controller,
                                   ThroughputEstimator &throughputEstimator,
                                   const SimulationSessionOptions &opts = {});
}
