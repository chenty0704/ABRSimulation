#include "ABRControllers/ThroughputBasedController.h"

DownloadDecision ThroughputBasedController::GetDownloadDecision(const SessionContext &ctx) {
    const auto &opts = dynamic_cast<const ThroughputBasedControllerOptions &>(*ThroughputBasedController::opts);
    DownloadDecision decision{.SegmentID = ctx.NextSegmentID};
    const auto &bitRatesInKbps = ctx.VideoModel.get().BitRatesInKbps;
    const size_t safeThroughputEstimate =
            std::llround(static_cast<double>(ctx.ThroughputEstimator.get().EstimateInKbps()) * opts.SafetyFactor);
    const auto it = std::upper_bound(bitRatesInKbps.cbegin(), bitRatesInKbps.cend(), safeThroughputEstimate);
    decision.BitRateID = it != bitRatesInKbps.cbegin() ? it - bitRatesInKbps.cbegin() - 1 : 0;
    return decision;
}
