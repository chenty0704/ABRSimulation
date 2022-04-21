#pragma once

#include "ABRControllers/IABRController.h"

struct ThroughputBasedControllerOptions : IABRControllerOptions {
    double SafetyFactor = 0.9;
};

class ThroughputBasedController : public IABRController {
public:
    explicit ThroughputBasedController(const ThroughputBasedControllerOptions &opts = {})
            : IABRController(std::make_unique<ThroughputBasedControllerOptions>(opts)) {}

    DownloadDecision GetDownloadDecision(const SessionContext &ctx) override;
};
