#pragma once

#include "Core/Core.h"
#include "VideoModel/VideoModel.h"
#include "ThroughputEstimators/IThroughputEstimator.h"

struct SessionContext {
    size_t MaxBufferSegmentCount;
    size_t NextSegmentID = 0;
    size_t PlaybackSegmentID = 0;
    double PlaybackTimeInSegmentInMs = 0;
    std::reference_wrapper<const VideoModel> VideoModel;
    std::span<const std::optional<double>> BufferedBitRatesInKbps;
    std::reference_wrapper<const IThroughputEstimator> ThroughputEstimator;

    template<typename Range>
    SessionContext(size_t maxBufferSegmentCount,
                   const struct VideoModel &videoModel, Range &&bufferedBitRatesInKbps,
                   const class IThroughputEstimator &throughputEstimator)
            : MaxBufferSegmentCount(maxBufferSegmentCount),
              VideoModel(videoModel), BufferedBitRatesInKbps(bufferedBitRatesInKbps),
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

struct IABRControllerOptions {
    virtual ~IABRControllerOptions() = default;
};

class IABRController {
public:
    explicit IABRController(std::unique_ptr<IABRControllerOptions> opts) : opts(std::move(opts)) {}

    virtual DownloadDecision GetDownloadDecision(const SessionContext &ctx) = 0;

    virtual ~IABRController() = default;

protected:
    std::unique_ptr<IABRControllerOptions> opts;
};
