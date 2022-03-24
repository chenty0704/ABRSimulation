#pragma once

#include "Core/Core.h"
#include "NetworkModel/ThroughputEstimators.h"
#include "ABRControllers/ABRController.h"

struct ThroughputBasedControllerOptions {
    double SafetyFactor = 0.9;
};

class ThroughputBasedController : public ABRController {
public:
    explicit ThroughputBasedController(std::unique_ptr<ThroughputEstimator> throughputEstimator,
                                       ThroughputBasedControllerOptions opts = {})
            : throughputEstimator(std::move(throughputEstimator)), opts(opts) {}

    DownloadDecision GetDownloadDecision(SessionContext ctx) override;

private:
    ThroughputBasedControllerOptions opts;
    std::unique_ptr<ThroughputEstimator> throughputEstimator;
};
