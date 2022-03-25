#pragma once

#include "Core/Core.h"
#include "NetworkModel/ThroughputEstimators.h"
#include "ABRControllers/ABRController.h"

struct ThroughputBasedControllerOptions : ABRControllerOptions {
    double SafetyFactor = 0.9;
};

class ThroughputBasedController : public ABRController {
public:
    explicit ThroughputBasedController(const ThroughputBasedControllerOptions &opts = {})
            : ABRController(std::make_unique<ThroughputBasedControllerOptions>(opts)) {}

    DownloadDecision GetDownloadDecision(SessionContext ctx) override;
};