#pragma once

#include "Core/Core.h"
#include "NetworkModel/NetworkModel.h"

class ThroughputEstimator {
public:
    virtual void Push(DownloadData data);

    [[nodiscard]] virtual size_t EstimateInKbps() const = 0;

protected:
    size_t timeInMs = 0;
    std::vector<size_t> durationsInMs, throughputsInKbps;
};

class ExponentialMovingAverageEstimator : public ThroughputEstimator {
public:
    ExponentialMovingAverageEstimator(size_t slowHalfLifeInMs, size_t fastHalfLifeInMs);

    ExponentialMovingAverageEstimator() : ExponentialMovingAverageEstimator(8'000, 3'000) {}

    void Push(DownloadData data) override;

    [[nodiscard]] size_t EstimateInKbps() const override;

private:
    double slowHalfLifeInMs, fastHalfLifeInMs;
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
