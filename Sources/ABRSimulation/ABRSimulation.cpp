#include "ABRSimulation/ABRSimulation.h"

SimulationData ABRSimulation::SimulateSession(const VideoModel &videoModel,
                                              NetworkModel &networkModel,
                                              ABRController &controller,
                                              ThroughputEstimator &throughputEstimator,
                                              const SessionOptions &opts) {
    SimulationData simData;
    simData.BufferedBitRatesInKbps.resize(videoModel.SegmentCount());
    SessionContext ctx{
            .VideoModel = videoModel,
            .BufferedBitRatesInKbps = simData.BufferedBitRatesInKbps,
            .ThroughputEstimator = throughputEstimator
    };

    const auto DownloadSegment = [&](DownloadDecision decision) {
        assert(ctx.PlaybackTimeInSegmentInMs > 0 ? decision.SegmentID > ctx.PlaybackSegmentID
                                                 : decision.SegmentID >= ctx.PlaybackSegmentID);
        assert(decision.SegmentID <= ctx.NextSegmentID);
        const auto segmentByteCount = videoModel.SegmentByteCounts.at(decision.SegmentID).at(decision.BitRateID);
        const auto downloadData = networkModel.Download(segmentByteCount);
        throughputEstimator.Push(downloadData);
        simData.TotalTimeInMs += downloadData.TimeInMs;
        simData.BufferedBitRatesInKbps.at(decision.SegmentID) = videoModel.BitRatesInKbps.at(decision.BitRateID);
        simData.DownloadDurationsInMs.push_back(downloadData.TimeInMs);
        simData.DownloadBitRatesInKbps.push_back(segmentByteCount / videoModel.SegmentDurationInMs);
        if (decision.SegmentID == ctx.NextSegmentID) ++ctx.NextSegmentID;
        return downloadData;
    };

    // Downloads the first segment at the lowest bit rate.
    DownloadSegment({ctx.NextSegmentID, 0});

    // Downloads the rest of the segments.
    while (!simData.BufferedBitRatesInKbps.back().has_value()) {
        auto bufferEndSegmentID =
                std::min(ctx.PlaybackSegmentID + opts.MaxBufferSegmentCount, videoModel.SegmentCount());

        // Waits until the buffer is not full.
        if (ctx.NextSegmentID == bufferEndSegmentID) {
            const auto delayInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
            ++bufferEndSegmentID;
            networkModel.Delay(delayInMs);
            simData.FullBufferDelaysInMs.push_back(delayInMs);
            simData.DownloadDurationsInMs.push_back(delayInMs);
            simData.DownloadBitRatesInKbps.push_back(0);
        }

        // Downloads a new or existing segment.
        const auto decision = controller.GetDownloadDecision(ctx);
        const auto downloadData = DownloadSegment(decision);

        // Plays the buffer content while downloading.
        size_t playbackTimeInMs = 0;
        while (playbackTimeInMs < downloadData.TimeInMs &&
               simData.BufferedBitRatesInKbps.at(ctx.PlaybackSegmentID).has_value()) {
            const auto restTimeInSegmentInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            if (downloadData.TimeInMs - playbackTimeInMs < restTimeInSegmentInMs) {
                ctx.PlaybackTimeInSegmentInMs += downloadData.TimeInMs - playbackTimeInMs;
                playbackTimeInMs = downloadData.TimeInMs;
            } else {
                ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
                playbackTimeInMs += restTimeInSegmentInMs;
            }
        }

        // Triggers rebuffering if there isn't enough buffer content to play.
        if (playbackTimeInMs < downloadData.TimeInMs)
            simData.RebufferingDurationsInMs.push_back(downloadData.TimeInMs - playbackTimeInMs);
    }

    // Plays the rest of the buffer content.
    if (ctx.PlaybackSegmentID < videoModel.SegmentCount()) {
        const auto restPlaybackTimeInMs = (videoModel.SegmentCount() - ctx.PlaybackSegmentID)
                                          * videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
        simData.TotalTimeInMs += restPlaybackTimeInMs;
        simData.DownloadDurationsInMs.push_back(restPlaybackTimeInMs);
        simData.DownloadBitRatesInKbps.push_back(0);
    }

    return simData;
}
