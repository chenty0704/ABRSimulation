#pragma once

#include "Core/Core.h"
#include "NetworkModel/NetworkModel.h"

struct IThroughputEstimatorOptions {
    virtual ~IThroughputEstimatorOptions() = default;
};

class IThroughputEstimator {
public:
    explicit IThroughputEstimator(std::unique_ptr<IThroughputEstimatorOptions> opts) : opts(std::move(opts)) {}

    virtual void Push(DownloadData data);

    [[nodiscard]] virtual double EstimateInKbps() const = 0;

    virtual ~IThroughputEstimator() = default;

protected:
    std::unique_ptr<IThroughputEstimatorOptions> opts;
    double timeInMs = 0;
    std::vector<double> durationsInMs, throughputsInKbps;
};
