#include "NetworkModel/ThroughputEstimators.h"

void AbstractThroughputEstimator::Push(DownloadData data) {
    const auto throughputInKbps = data.DownloadedBitCount / data.TimeInMs;
    timeInMs += data.TimeInMs;
    durationsInMs.push_back(data.TimeInMs);
    throughputsInKbps.push_back(throughputInKbps);
}

ExponentialMovingAverageEstimator::ExponentialMovingAverageEstimator(int slowHalfLifeInMs, int fastHalfLifeInMs) {
    if (slowHalfLifeInMs <= 0 || fastHalfLifeInMs <= 0 || slowHalfLifeInMs < fastHalfLifeInMs)
        throw std::invalid_argument("Half life values are invalid.");

    ExponentialMovingAverageEstimator::slowHalfLifeInMs = slowHalfLifeInMs;
    ExponentialMovingAverageEstimator::fastHalfLifeInMs = fastHalfLifeInMs;
}

void ExponentialMovingAverageEstimator::Push(DownloadData data) {
    AbstractThroughputEstimator::Push(data);
    const auto throughputInKbps = throughputsInKbps.back();
    const auto slowLastWeight = std::pow(0.5, data.TimeInMs / slowHalfLifeInMs);
    const auto fastLastWeight = std::pow(0.5, data.TimeInMs / fastHalfLifeInMs);
    slowEstimateInKbps = slowEstimateInKbps * slowLastWeight + throughputInKbps * (1 - slowLastWeight);
    fastEstimateInKbps = fastEstimateInKbps * fastLastWeight + throughputInKbps * (1 - fastLastWeight);
}

int ExponentialMovingAverageEstimator::EstimateInKbps() const {
    if (timeInMs == 0)
        throw std::runtime_error("No samples exist.");

    const auto adjustedSlowEstimateInKbps = slowEstimateInKbps / (1 - std::pow(0.5, timeInMs / slowHalfLifeInMs));
    const auto adjustedFastEstimateInKbps = fastEstimateInKbps / (1 - std::pow(0.5, timeInMs / fastHalfLifeInMs));
    return std::lround(std::min(adjustedSlowEstimateInKbps, adjustedFastEstimateInKbps));
}
