#include "ABRSimulation/ABRSimulation.h"

SimulationData ABRSimulation::SimulateSession(const VideoModel &videoModel,
                                              NetworkModel &networkModel,
                                              ABRController &controller,
                                              ThroughputEstimator &throughputEstimator,
                                              const SessionOptions &opts) {
    SimulationData simData;
    simData.DownloadedBitRatesInKbps.resize(videoModel.SegmentCount());
    SessionContext ctx{
            .VideoModel = videoModel,
            .DownloadedBitRatesInKbps = simData.DownloadedBitRatesInKbps,
            .ThroughputEstimator = throughputEstimator
    };

    // Downloads the first segment at the lowest bit rate.
    auto segmentByteCount = videoModel.SegmentByteCounts.at(0).at(0);
    auto downloadData = networkModel.Download(segmentByteCount);
    throughputEstimator.Push(downloadData);
    simData.DownloadedBitRatesInKbps.at(0) = videoModel.BitRatesInKbps.at(0);
    simData.TotalTimeInMs += downloadData.TimeInMs;

    // Downloads the rest of the segments.
    while (!simData.DownloadedBitRatesInKbps.back().has_value()) {
        auto bufferEndSegmentID =
                std::min(ctx.PlaybackSegmentID + opts.MaxBufferSegmentCount, videoModel.SegmentCount());

        // Waits until the buffer is not full.
        if (simData.DownloadedBitRatesInKbps.at(bufferEndSegmentID - 1).has_value()) {
            const auto delayInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
            ++bufferEndSegmentID;
            networkModel.Delay(delayInMs);
            simData.FullBufferDelaysInMs.push_back(delayInMs);
        }

        // Downloads a new or existing segment.
        const auto decision = controller.GetDownloadDecision(ctx);
        assert(ctx.PlaybackTimeInSegmentInMs > 0 ? decision.SegmentID > ctx.PlaybackSegmentID
                                                 : decision.SegmentID >= ctx.PlaybackSegmentID);
        assert(decision.SegmentID < bufferEndSegmentID);
        segmentByteCount = videoModel.SegmentByteCounts.at(decision.SegmentID).at(decision.BitRateID);
        downloadData = networkModel.Download(segmentByteCount);
        throughputEstimator.Push(downloadData);

        // Plays the buffer content while downloading.
        size_t downloadTimeInMs = 0;
        while (downloadTimeInMs < downloadData.TimeInMs &&
               simData.DownloadedBitRatesInKbps.at(ctx.PlaybackSegmentID).has_value()) {
            const auto restTimeInSegmentInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            if (downloadData.TimeInMs - downloadTimeInMs < restTimeInSegmentInMs) {
                ctx.PlaybackTimeInSegmentInMs += downloadData.TimeInMs - downloadTimeInMs;
                downloadTimeInMs = downloadData.TimeInMs;
            } else {
                ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
                downloadTimeInMs += restTimeInSegmentInMs;
            }
        }

        // Triggers rebuffering if there isn't enough buffer content to play.
        if (downloadTimeInMs < downloadData.TimeInMs)
            simData.RebufferingDurationsInMs.push_back(downloadData.TimeInMs - downloadTimeInMs);

        // Places the downloaded segment in the buffer.
        simData.DownloadedBitRatesInKbps.at(decision.SegmentID) = videoModel.BitRatesInKbps.at(decision.BitRateID);
        simData.TotalTimeInMs += downloadData.TimeInMs;
    }

    // Plays the rest of the buffer content.
    if (ctx.PlaybackSegmentID < videoModel.SegmentCount())
        simData.TotalTimeInMs += (videoModel.SegmentCount() - ctx.PlaybackSegmentID) * videoModel.SegmentDurationInMs
                                 - ctx.PlaybackTimeInSegmentInMs;

    return simData;
}
