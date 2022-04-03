#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "ThroughputEstimators/ThroughputEstimator.h"

struct SessionContext {
    size_t NextSegmentID = 0;
    size_t PlaybackSegmentID = 0;
    double PlaybackTimeInSegmentInMs = 0;
    std::reference_wrapper<const VideoModel> VideoModel;
    std::span<const std::optional<double>> BufferedBitRatesInKbps;
    std::reference_wrapper<const ThroughputEstimator> ThroughputEstimator;

    template<typename Range>
    SessionContext(const struct VideoModel &videoModel, Range &&bufferedBitRatesInKbps,
                   const class ThroughputEstimator &throughputEstimator)
            : VideoModel(videoModel), BufferedBitRatesInKbps(bufferedBitRatesInKbps),
              ThroughputEstimator(throughputEstimator) {}

    [[nodiscard]] double BufferLevelInMs() const {
        return static_cast<double>(NextSegmentID - PlaybackSegmentID) *
               VideoModel.get().SegmentDurationInMs - PlaybackTimeInSegmentInMs;
    }
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
};
