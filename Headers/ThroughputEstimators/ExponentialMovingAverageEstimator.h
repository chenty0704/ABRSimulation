#pragma once

#include "ThroughputEstimators/IThroughputEstimator.h"

struct ExponentialMovingAverageEstimatorOptions : IThroughputEstimatorOptions {
    double SlowHalfLifeInMs = 8'000;
    double FastHalfLifeInMs = 3'000;
};

class ExponentialMovingAverageEstimator : public IThroughputEstimator {
public:
    explicit ExponentialMovingAverageEstimator(const ExponentialMovingAverageEstimatorOptions &opts = {})
            : IThroughputEstimator(std::make_unique<ExponentialMovingAverageEstimatorOptions>(opts)) {}

    void Push(DownloadData data) override;

    [[nodiscard]] double EstimateInKbps() const override;

private:
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
