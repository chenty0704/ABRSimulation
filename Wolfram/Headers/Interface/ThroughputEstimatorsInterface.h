#pragma once

#include <LLU/LLU.h>

#include "ThroughputEstimators/ThroughputEstimators.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, ExponentialMovingAverageEstimatorOptions &opts) {
        WS::List rules;
        stream >> rules;
        for (auto i = 0; i < rules.getArgc(); ++i) {
            std::string opt;
            stream >> WS::Rule >> opt;
            if (opt == "SlowHalfLifeInMs") stream >> opts.SlowHalfLifeInMs;
            else if (opt == "FastHalfLifeInMs") stream >> opts.FastHalfLifeInMs;
            else ErrorManager::throwException("UnknownOptionError", opt);
        }
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, std::unique_ptr<ThroughputEstimator> &throughputEstimator) {
        std::string type;
        stream >> type;
        if (type == "ExponentialMovingAverageEstimator") {
            ExponentialMovingAverageEstimatorOptions opts;
            stream >> opts;
            throughputEstimator = std::make_unique<ExponentialMovingAverageEstimator>(opts);
        } else std::abort();
        return stream;
    }
}
