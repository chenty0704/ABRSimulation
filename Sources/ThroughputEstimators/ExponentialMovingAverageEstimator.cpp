#include "ThroughputEstimators/ExponentialMovingAverageEstimator.h"

void ExponentialMovingAverageEstimator::Push(DownloadData data) {
    const auto &opts = dynamic_cast<const ExponentialMovingAverageEstimatorOptions &>(*ExponentialMovingAverageEstimator::opts);
    ThroughputEstimator::Push(data);
    const auto throughputInKbps = throughputsInKbps.back();
    const auto slowLastWeight =
            std::pow(0.5, static_cast<double>(data.TimeInMs) / static_cast<double>(opts.SlowHalfLifeInMs));
    const auto fastLastWeight =
            std::pow(0.5, static_cast<double>(data.TimeInMs) / static_cast<double>(opts.FastHalfLifeInMs));
    slowEstimateInKbps =
            slowEstimateInKbps * slowLastWeight + static_cast<double>(throughputInKbps) * (1 - slowLastWeight);
    fastEstimateInKbps =
            fastEstimateInKbps * fastLastWeight + static_cast<double>(throughputInKbps) * (1 - fastLastWeight);
}

size_t ExponentialMovingAverageEstimator::EstimateInKbps() const {
    assert(timeInMs > 0);
    const auto &opts = dynamic_cast<const ExponentialMovingAverageEstimatorOptions &>(*ExponentialMovingAverageEstimator::opts);
    const auto slowWeight =
            1 - std::pow(0.5, static_cast<double>(timeInMs) / static_cast<double>(opts.SlowHalfLifeInMs));
    const auto fastWeight =
            1 - std::pow(0.5, static_cast<double>(timeInMs) / static_cast<double>(opts.FastHalfLifeInMs));
    return std::llround(std::min(slowEstimateInKbps / slowWeight, fastEstimateInKbps / fastWeight));
}