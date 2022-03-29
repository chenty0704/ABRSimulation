#pragma once

#include <LLU/LLU.h>

#include "ABRControllers/ABRControllers.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, ThroughputBasedControllerOptions &opts) {
        WS::Association assoc;
        stream >> assoc;
        for (auto i = 0; i < assoc.getArgc(); ++i) {
            std::string key;
            stream >> WS::Rule >> key;
            if (key == "SafetyFactor") stream >> opts.SafetyFactor;
            else std::abort();
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
