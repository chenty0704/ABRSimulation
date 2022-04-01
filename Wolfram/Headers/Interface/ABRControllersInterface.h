#pragma once

#include <LLU/LLU.h>

#include "ABRControllers/ABRControllers.h"
#include "Interface/CoreInterface.h"

namespace LLU {
    LLU_REGISTER_INPUT_TYPE(
            ThroughputBasedControllerOptions,
            SafetyFactor
    )

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
