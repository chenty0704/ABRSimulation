#pragma once

#include "ThroughputEstimators/ThroughputEstimator.h"

struct ExponentialMovingAverageEstimatorOptions : ThroughputEstimatorOptions {
    double SlowHalfLifeInMs = 8'000;
    double FastHalfLifeInMs = 3'000;
};

class ExponentialMovingAverageEstimator : public ThroughputEstimator {
public:
    explicit ExponentialMovingAverageEstimator(const ExponentialMovingAverageEstimatorOptions &opts = {})
            : ThroughputEstimator(std::make_unique<ExponentialMovingAverageEstimatorOptions>(opts)) {}

    void Push(DownloadData data) override;

    [[nodiscard]] double EstimateInKbps() const override;

private:
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
