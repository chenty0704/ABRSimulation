#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"

struct SessionContext { // NOLINT(cppcoreguidelines-pro-type-member-init)
    size_t MaxBufferSizeInSegments;
    size_t PlaybackTimeInMs;
    std::reference_wrapper<const VideoModel> VideoModel;
    std::span<const std::optional<size_t>> DownloadedBitRateIDs;
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

    virtual DownloadDecision GetDownloadDecision(SessionContext ctx) = 0;

    virtual ~ABRController() = default;

protected:
    std::unique_ptr<ABRControllerOptions> opts;
    size_t nextSegmentID = 1;
};
