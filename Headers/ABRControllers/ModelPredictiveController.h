#pragma once

#include "ABRControllers/ABRControllers.h"
#include "Interface/CoreInterface.h"

struct ModelPredictiveControllerOptions : IABRControllerOptions {
    double TimeIntervalInMs = 2'000;
    size_t WindowSize = 5;
    double TargetBufferRatio = 0.5;
    double BufferDeviationPenaltyFactor = 5;
    double SwitchingCostFactor = 2.5;
};

class ModelPredictiveController : public IABRController {
public:
    explicit ModelPredictiveController(LLU::NativeWSStream &stream, const ModelPredictiveControllerOptions &opts = {})
            : stream(stream), IABRController(std::make_unique<ModelPredictiveControllerOptions>(opts)) {}

    DownloadDecision GetDownloadDecision(const SessionContext &ctx) override;

private:
    std::reference_wrapper<LLU::NativeWSStream> stream;
};
