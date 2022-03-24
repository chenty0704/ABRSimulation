#pragma once

#include "Core/Core.h"
#include "NetworkModel/NetworkModel.h"

class ThroughputEstimator {
public:
    virtual void Push(DownloadData data);

    [[nodiscard]] virtual size_t EstimateInKbps() const = 0;

    virtual ~ThroughputEstimator() = default;

protected:
    size_t timeInMs = 0;
    std::vector<size_t> durationsInMs, throughputsInKbps;
};

struct ExponentialMovingAverageEstimatorOptions {
    size_t SlowHalfLifeInMs = 8'000;
    size_t FastHalfLifeInMs = 3'000;
};

class ExponentialMovingAverageEstimator : public ThroughputEstimator {
public:
    explicit ExponentialMovingAverageEstimator(ExponentialMovingAverageEstimatorOptions opts = {}) : opts(opts) {}

    void Push(DownloadData data) override;

    [[nodiscard]] size_t EstimateInKbps() const override;

private:
    ExponentialMovingAverageEstimatorOptions opts;
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
