#pragma once

#include "Core/Core.h"
#include "NetworkModel/NetworkModel.h"

struct ThroughputEstimatorOptions {
    virtual ~ThroughputEstimatorOptions() = default;
};

class ThroughputEstimator {
public:
    explicit ThroughputEstimator(std::unique_ptr<ThroughputEstimatorOptions> opts) : opts(std::move(opts)) {}

    virtual void Push(DownloadData data);

    [[nodiscard]] virtual size_t EstimateInKbps() const = 0;

    virtual ~ThroughputEstimator() = default;

protected:
    std::unique_ptr<ThroughputEstimatorOptions> opts;
    size_t timeInMs = 0;
    std::vector<size_t> durationsInMs, throughputsInKbps;
};

struct ExponentialMovingAverageEstimatorOptions : ThroughputEstimatorOptions {
    size_t SlowHalfLifeInMs = 8'000;
    size_t FastHalfLifeInMs = 3'000;
};

class ExponentialMovingAverageEstimator : public ThroughputEstimator {
public:
    explicit ExponentialMovingAverageEstimator(const ExponentialMovingAverageEstimatorOptions &opts = {})
            : ThroughputEstimator(std::make_unique<ExponentialMovingAverageEstimatorOptions>(opts)) {}

    void Push(DownloadData data) override;

    [[nodiscard]] size_t EstimateInKbps() const override;

private:
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
