#include "NetworkModel/ThroughputEstimators.h"

void ThroughputEstimator::Push(DownloadData data) {
    const auto throughputInKbps = data.ByteCount * CHAR_BIT / data.TimeInMs;
    timeInMs += data.TimeInMs;
    durationsInMs.push_back(data.TimeInMs);
    throughputsInKbps.push_back(throughputInKbps);
}

ExponentialMovingAverageEstimator::ExponentialMovingAverageEstimator(size_t slowHalfLifeInMs, size_t fastHalfLifeInMs) {
    assert(slowHalfLifeInMs >= fastHalfLifeInMs);
    ExponentialMovingAverageEstimator::slowHalfLifeInMs = static_cast<double>(slowHalfLifeInMs);
    ExponentialMovingAverageEstimator::fastHalfLifeInMs = static_cast<double>(fastHalfLifeInMs);
}

void ExponentialMovingAverageEstimator::Push(DownloadData data) {
    ThroughputEstimator::Push(data);
    const auto throughputInKbps = throughputsInKbps.back();
    const auto slowLastWeight = std::pow(0.5, static_cast<double>(data.TimeInMs) / slowHalfLifeInMs);
    const auto fastLastWeight = std::pow(0.5, static_cast<double>(data.TimeInMs) / fastHalfLifeInMs);
    slowEstimateInKbps = slowEstimateInKbps * slowLastWeight +
                         static_cast<double>(throughputInKbps) * (1 - slowLastWeight);
    fastEstimateInKbps = fastEstimateInKbps * fastLastWeight +
                         static_cast<double>(throughputInKbps) * (1 - fastLastWeight);
}

size_t ExponentialMovingAverageEstimator::EstimateInKbps() const {
    assert(timeInMs > 0);
    const auto adjustedSlowEstimateInKbps = slowEstimateInKbps /
                                            (1 - std::pow(0.5, static_cast<double>(timeInMs) / slowHalfLifeInMs));
    const auto adjustedFastEstimateInKbps = fastEstimateInKbps /
                                            (1 - std::pow(0.5, static_cast<double>(timeInMs) / fastHalfLifeInMs));
    return std::llround(std::min(adjustedSlowEstimateInKbps, adjustedFastEstimateInKbps));
}
