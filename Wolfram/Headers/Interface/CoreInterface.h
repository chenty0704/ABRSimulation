#pragma once

#include <LLU/LLU.h>
#include <boost/preprocessor.hpp>

#include "Core/Core.h"
#include "Core/Meta.h"

namespace LLU {
#define LLU_FIELD_NAME_I(r, data, i, name) BOOST_PP_COMMA_IF(i) BOOST_PP_STRINGIZE(name)
#define LLU_FIELD_REF_I(r, data, i, name) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(data, name)

#define LLU_REGISTER_INPUT_TYPE_IMPL(Type, fields)                          \
    template<WS::Encoding EIn, WS::Encoding EOut>                           \
    auto &operator>>(WSStream<EIn, EOut> &stream, Type &obj) {              \
        ReadObject(stream,                                                  \
                   {BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_NAME_I, _, fields)},  \
                   BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_REF_I, obj., fields)); \
        return stream;                                                      \
    }
#define LLU_REGISTER_INPUT_TYPE(Type, ...) LLU_REGISTER_INPUT_TYPE_IMPL(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define LLU_REGISTER_OUTPUT_TYPE_IMPL(Type, fields)                          \
    template<WS::Encoding EIn, WS::Encoding EOut>                            \
    auto &operator<<(WSStream<EIn, EOut> &stream, const Type &obj) {         \
        WriteObject(stream,                                                  \
                    {BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_NAME_I, _, fields)},  \
                    BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_REF_I, obj., fields)); \
        return stream;                                                       \
    }
#define LLU_REGISTER_OUTPUT_TYPE(Type, ...) LLU_REGISTER_OUTPUT_TYPE_IMPL(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

    inline const std::vector<std::pair<std::string, std::string>> PacletErrors = { // NOLINT(cert-err58-cpp)
            {"UnknownNameError", "Unknown name `name`"}
    };

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator>>(WSStream<EIn, EOut> &stream, size_t &value) {
        long long temp;
        stream >> temp;
        assert(temp >= 0);
        value = temp;
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut, typename... Args>
    void ReadObject(WSStream<EIn, EOut> &stream,
                    const std::array<std::string_view, sizeof...(Args)> &fieldNames,
                    Args &... fields) {
        WS::Function head;
        stream >> head;
        for (auto i = 0; i < head.getArgc(); ++i) {
            std::string name;
            stream >> WS::Rule >> name;
            bool matched = false;
            ForEachRefIndexedUntil([&](size_t j, auto &field) {
                return name == fieldNames[j] && (stream >> field, matched = true);
            }, fields...);
            if (!matched) ErrorManager::throwException("UnknownNameError", name);
        }
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator<<(WSStream<EIn, EOut> &stream, size_t value) {
        assert(value <= std::numeric_limits<long long>::max());
        return stream << static_cast<long long>(value);
    }

    template<WS::Encoding EIn, WS::Encoding EOut>
    auto &operator<<(WSStream<EIn, EOut> &stream, std::string_view str) {
        WS::String<EOut>::put(stream.get(), str.data(), str.size());
        return stream;
    }

    template<WS::Encoding EIn, WS::Encoding EOut, typename T>
    auto &operator<<(WSStream<EIn, EOut> &stream, const std::optional<T> &optional) {
        if (optional.has_value()) return stream << optional.value();
        return stream << WS::Missing();
    }

    template<WS::Encoding EIn, WS::Encoding EOut, typename T1, typename T2>
    auto &operator<<(WSStream<EIn, EOut> &stream, const std::pair<T1, T2> &pair) {
        return stream << WS::List(2) << pair.first << pair.second;
    }

    template<WS::Encoding EIn, WS::Encoding EOut, typename... Args>
    void WriteObject(WSStream<EIn, EOut> &stream,
                     const std::array<std::string_view, sizeof...(Args)> &fieldNames,
                     const Args &... fields) {
        stream << WS::Association(sizeof...(Args));
        ForEachCRefIndexed([&](size_t i, const auto &field) {
            stream << WS::Rule << fieldNames[i] << field;
        }, fields...);
    }
}

extern "C" DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData);
