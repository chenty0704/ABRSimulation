#pragma once

#include "ThroughputEstimators/ThroughputEstimators.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(ExponentialMovingAverageEstimatorOptions,
                            SlowHalfLifeInMs,
                            FastHalfLifeInMs)

    NativeWSStream &operator>>(NativeWSStream &stream, std::unique_ptr<IThroughputEstimator> &throughputEstimator) {
        std::string type;
        stream >> type;
        if (type == "ExponentialMovingAverageEstimator") {
            ExponentialMovingAverageEstimatorOptions opts;
            stream >> opts;
            throughputEstimator = std::make_unique<ExponentialMovingAverageEstimator>(opts);
        } else ErrorManager::throwException("UnknownNameError", type);
        return stream;
    }
}
