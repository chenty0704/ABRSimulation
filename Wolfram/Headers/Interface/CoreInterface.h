#pragma once

#include <LLU/LLU.h>

#include "Core/Core.h"

namespace LLU {
    inline const std::vector<std::pair<std::string, std::string>> PacletErrors = { // NOLINT(cert-err58-cpp)
            {"UnknownOptionError", "Unknown option `opt`"}
    };

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, size_t &value) {
        long long temp;
        stream >> temp;
        assert(temp >= 0);
        value = temp;
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator<<(WSStream<EIn, EOut> &stream, size_t value) {
        assert(value <= std::numeric_limits<long long>::max());
        return stream << static_cast<long long>(value);
    }

    template<WS::Encoding EIn, WS::Encoding EOut, typename T>
    auto &operator<<(WSStream<EIn, EOut> &stream, std::optional<T> optional) {
        if (optional.has_value()) return stream << optional.value();
        return stream << WS::Missing();
    }
}

extern "C" DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData);
