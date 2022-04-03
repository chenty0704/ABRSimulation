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

    [[nodiscard]] virtual double EstimateInKbps() const = 0;

    virtual ~ThroughputEstimator() = default;

protected:
    std::unique_ptr<ThroughputEstimatorOptions> opts;
    double timeInMs = 0;
    std::vector<double> durationsInMs, throughputsInKbps;
};
