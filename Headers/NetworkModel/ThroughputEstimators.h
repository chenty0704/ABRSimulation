#pragma once

#include "Core/Core.h"
#include "NetworkModel/NetworkModel.h"

class AbstractThroughputEstimator {
public:
    virtual void Push(DownloadData data);

    [[nodiscard]] virtual int EstimateInKbps() const = 0;

protected:
    int timeInMs = 0;
    std::vector<int> durationsInMs, throughputsInKbps;
};

class ExponentialMovingAverageEstimator : public AbstractThroughputEstimator {
public:
    ExponentialMovingAverageEstimator(int slowHalfLifeInMs, int fastHalfLifeInMs);

    ExponentialMovingAverageEstimator() : ExponentialMovingAverageEstimator(8'000, 3'000) {}

    void Push(DownloadData data) override;

    [[nodiscard]] int EstimateInKbps() const override;

private:
    double slowHalfLifeInMs, fastHalfLifeInMs;
    double slowEstimateInKbps = 0, fastEstimateInKbps = 0;
};
