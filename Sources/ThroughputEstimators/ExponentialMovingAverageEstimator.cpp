#include "ThroughputEstimators/ExponentialMovingAverageEstimator.h"

void ExponentialMovingAverageEstimator::Push(DownloadData data) {
    const auto &opts = dynamic_cast<const ExponentialMovingAverageEstimatorOptions &>(*ExponentialMovingAverageEstimator::opts);
    IThroughputEstimator::Push(data);
    const auto throughputInKbps = throughputsInKbps.back();
    const auto slowLastWeight = std::pow(0.5, data.TimeInMs / opts.SlowHalfLifeInMs);
    const auto fastLastWeight = std::pow(0.5, data.TimeInMs / opts.FastHalfLifeInMs);
    slowEstimateInKbps = slowEstimateInKbps * slowLastWeight + throughputInKbps * (1 - slowLastWeight);
    fastEstimateInKbps = fastEstimateInKbps * fastLastWeight + throughputInKbps * (1 - fastLastWeight);
}

double ExponentialMovingAverageEstimator::EstimateInKbps() const {
    assert(timeInMs > 0);
    const auto &opts = dynamic_cast<const ExponentialMovingAverageEstimatorOptions &>(*ExponentialMovingAverageEstimator::opts);
    const auto slowWeight = 1 - std::pow(0.5, timeInMs / opts.SlowHalfLifeInMs);
    const auto fastWeight = 1 - std::pow(0.5, timeInMs / opts.FastHalfLifeInMs);
    return std::min(slowEstimateInKbps / slowWeight, fastEstimateInKbps / fastWeight);
}
