#include "ABRSimulation/ABRSimulation.h"

SimulationData ABRSimulation::SimulateSession(const VideoModel &videoModel,
                                              NetworkModel &networkModel,
                                              ABRController &controller,
                                              ThroughputEstimator &throughputEstimator,
                                              const SessionOptions &opts) {
    SimulationData simData(videoModel.SegmentCount());
    SessionContext ctx(videoModel, simData.BufferedBitRatesInKbps, throughputEstimator);

    const auto DownloadSegment = [&](DownloadDecision decision) {
        assert(ctx.PlaybackTimeInSegmentInMs > 0 ? decision.SegmentID > ctx.PlaybackSegmentID
                                                 : decision.SegmentID >= ctx.PlaybackSegmentID);
        assert(decision.SegmentID <= ctx.NextSegmentID);
        const auto segmentByteCount = videoModel.SegmentByteCounts.at(decision.SegmentID).at(decision.BitRateID);
        const auto downloadData = networkModel.Download(segmentByteCount);
        throughputEstimator.Push(downloadData);
        simData.DownloadDurationsInMs.push_back(downloadData.TimeInMs);
        simData.DownloadBitRatesInKbps.push_back(
                static_cast<double>(segmentByteCount * CHAR_BIT) / videoModel.SegmentDurationInMs);
        return downloadData;
    };
    const auto UpdateBuffer = [&](DownloadDecision decision) {
        simData.BufferTimesInMs.push_back(simData.TotalTimeInMs);
        simData.BufferLevelsInMs.push_back(ctx.BufferLevelInMs());
        simData.BufferedBitRatesInKbps.at(decision.SegmentID) = videoModel.BitRatesInKbps.at(decision.BitRateID);
        if (decision.SegmentID == ctx.NextSegmentID) ++ctx.NextSegmentID;
        simData.BufferTimesInMs.push_back(simData.TotalTimeInMs + 1);
        simData.BufferLevelsInMs.push_back(ctx.BufferLevelInMs());
    };

    // Downloads the first segment at the lowest bit rate.
    DownloadDecision decision{ctx.NextSegmentID, 0};
    auto downloadData = DownloadSegment(decision);
    simData.TotalTimeInMs += downloadData.TimeInMs;
    UpdateBuffer(decision);

    // Downloads the rest of the segments.
    while (ctx.NextSegmentID < videoModel.SegmentCount()) {
        auto bufferEndSegmentID =
                std::min(ctx.PlaybackSegmentID + opts.MaxBufferSegmentCount, videoModel.SegmentCount());

        // Waits until the buffer is not full.
        if (ctx.NextSegmentID == bufferEndSegmentID) {
            const auto delayInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
            ++bufferEndSegmentID;
            networkModel.Delay(delayInMs);
            simData.TotalTimeInMs += delayInMs;
            simData.DownloadDurationsInMs.push_back(delayInMs);
            simData.DownloadBitRatesInKbps.push_back(0);
            simData.BufferTimesInMs.push_back(simData.TotalTimeInMs);
            simData.BufferLevelsInMs.push_back(ctx.BufferLevelInMs());
            simData.FullBufferDelaysInMs.push_back(delayInMs);
        }

        // Downloads a new or existing segment.
        decision = controller.GetDownloadDecision(ctx);
        downloadData = DownloadSegment(decision);

        // Plays the buffer content while downloading.
        double elapsedTimeInMs = 0;
        while (elapsedTimeInMs < downloadData.TimeInMs && ctx.PlaybackSegmentID < ctx.NextSegmentID) {
            const auto restTimeInSegmentInMs = videoModel.SegmentDurationInMs - ctx.PlaybackTimeInSegmentInMs;
            if (downloadData.TimeInMs - elapsedTimeInMs < restTimeInSegmentInMs) {
                ctx.PlaybackTimeInSegmentInMs += downloadData.TimeInMs - elapsedTimeInMs;
                elapsedTimeInMs = downloadData.TimeInMs;
            } else {
                ++ctx.PlaybackSegmentID, ctx.PlaybackTimeInSegmentInMs = 0;
                elapsedTimeInMs += restTimeInSegmentInMs;
            }
        }
        simData.TotalTimeInMs += elapsedTimeInMs;

        // Triggers rebuffering if there is not enough buffer content.
        if (elapsedTimeInMs < downloadData.TimeInMs) {
            const auto rebufferingDurationInMs = downloadData.TimeInMs - elapsedTimeInMs;
            simData.BufferTimesInMs.push_back(simData.TotalTimeInMs);
            simData.BufferLevelsInMs.push_back(ctx.BufferLevelInMs());
            simData.RebufferingPeriodsInMs.emplace_back(simData.TotalTimeInMs,
                                                        simData.TotalTimeInMs + rebufferingDurationInMs);
            simData.TotalTimeInMs += rebufferingDurationInMs;
        }

        UpdateBuffer(decision);
    }

    // Plays the rest of the buffer content.
    const auto bufferLevelInMs = ctx.BufferLevelInMs();
    simData.TotalTimeInMs += bufferLevelInMs;
    simData.DownloadDurationsInMs.push_back(bufferLevelInMs);
    simData.DownloadBitRatesInKbps.push_back(0);
    simData.BufferTimesInMs.push_back(simData.TotalTimeInMs);
    simData.BufferLevelsInMs.push_back(0);

    return simData;
}
