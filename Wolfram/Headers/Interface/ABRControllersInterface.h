#pragma once

#include "ABRControllers/ABRControllers.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(ThroughputBasedControllerOptions,
                            SafetyFactor)

    LLU_REGISTER_INPUT_TYPE(ModelPredictiveControllerOptions,
                            TimeIntervalInMs,
                            WindowSize,
                            TargetBufferRatio,
                            BufferDeviationPenaltyFactor,
                            SwitchingCostFactor)

    WSNativeStream &operator>>(WSNativeStream &stream, std::unique_ptr<IABRController> &controller) {
        std::string type;
        stream >> type;
        if (type == "ThroughputBasedController") {
            ThroughputBasedControllerOptions opts;
            stream >> opts;
            controller = std::make_unique<ThroughputBasedController>(opts);
        } else if (type == "ModelPredictiveController") {
            ModelPredictiveControllerOptions opts;
            stream >> opts;
            controller = std::make_unique<ModelPredictiveController>(stream, opts);
        } else ErrorManager::throwException("UnknownNameError", type);
        return stream;
    }
}
