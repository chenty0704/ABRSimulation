#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "ThroughputEstimators/ThroughputEstimator.h"

struct SessionContext {
    size_t PlaybackSegmentID = 0, PlaybackTimeInSegmentInMs = 0;
    std::reference_wrapper<const VideoModel> VideoModel;
    std::span<const std::optional<size_t>> DownloadedBitRatesInKbps;
    std::reference_wrapper<const ThroughputEstimator> ThroughputEstimator;
};

struct DownloadDecision {
    size_t SegmentID;
    size_t BitRateID;
};

struct ABRControllerOptions {
    virtual ~ABRControllerOptions() = default;
};

class ABRController {
public:
    explicit ABRController(std::unique_ptr<ABRControllerOptions> opts) : opts(std::move(opts)) {}

    virtual DownloadDecision GetDownloadDecision(const SessionContext &ctx) = 0;

    virtual ~ABRController() = default;

protected:
    std::unique_ptr<ABRControllerOptions> opts;
    size_t nextSegmentID = 1;
};
