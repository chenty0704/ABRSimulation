#pragma once

#include <LLU/LLU.h>

#include "ABRControllers/ABRControllers.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, ThroughputBasedControllerOptions &opts) {
        WS::List rules;
        stream >> rules;
        for (auto i = 0; i < rules.getArgc(); ++i) {
            std::string opt;
            stream >> WS::Rule >> opt;
            if (opt == "SafetyFactor") stream >> opts.SafetyFactor;
            else ErrorManager::throwException("UnknownOptionError", opt);
        }
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, std::unique_ptr<ABRController> &controller) {
        std::string type;
        stream >> type;
        if (type == "ThroughputBasedController") {
            ThroughputBasedControllerOptions opts;
            stream >> opts;
            controller = std::make_unique<ThroughputBasedController>(opts);
        } else std::abort();
        return stream;
    }
}
