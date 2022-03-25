#include "ABRControllers/ThroughputBasedController.h"

DownloadDecision ThroughputBasedController::GetDownloadDecision(SessionContext ctx) {
    const auto &opts = dynamic_cast<const ThroughputBasedControllerOptions &>(*ThroughputBasedController::opts);
    DownloadDecision decision{.SegmentID = nextSegmentID};
    const auto &bitRatesInKbps = ctx.VideoModel.get().BitRatesInKbps;
    const auto safeThroughputEstimate =
            std::llround(static_cast<double>(ctx.ThroughputEstimator.get().EstimateInKbps()) * opts.SafetyFactor);
    const auto it = std::lower_bound(bitRatesInKbps.cbegin(), bitRatesInKbps.cend(), safeThroughputEstimate);
    decision.BitRateID = it != bitRatesInKbps.cend() ? it - bitRatesInKbps.cbegin() : 0;
    return decision;
}
