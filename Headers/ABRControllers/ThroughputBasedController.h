#pragma once

#include "ABRControllers/ABRController.h"

struct ThroughputBasedControllerOptions : ABRControllerOptions {
    double SafetyFactor = 0.9;
};

class ThroughputBasedController : public ABRController {
public:
    explicit ThroughputBasedController(const ThroughputBasedControllerOptions &opts = {})
            : ABRController(std::make_unique<ThroughputBasedControllerOptions>(opts)) {}

    DownloadDecision GetDownloadDecision(const SessionContext &ctx) override;
};
