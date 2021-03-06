#pragma once

#include <LLU/LLU.h>
#include <boost/preprocessor.hpp>

#include "Core/Core.h"
#include "Core/Meta.h"

#define LLU_FIELD_NAME_I(r, data, i, name) BOOST_PP_COMMA_IF(i) BOOST_PP_STRINGIZE(name)
#define LLU_FIELD_REF_I(r, data, i, name) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(data, name)

#define LLU_REGISTER_INPUT_TYPE_IMPL(Type, fields)                          \
    inline WSNativeStream &operator>>(WSNativeStream &stream, Type &obj) {  \
        ReadObject(stream,                                                  \
                   {BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_NAME_I, _, fields)},  \
                   BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_REF_I, obj., fields)); \
        return stream;                                                      \
    }
#define LLU_REGISTER_INPUT_TYPE(Type, ...) LLU_REGISTER_INPUT_TYPE_IMPL(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define LLU_REGISTER_OUTPUT_TYPE_IMPL(Type, fields)                              \
    inline WSNativeStream &operator<<(WSNativeStream &stream, const Type &obj) { \
        WriteObject(stream,                                                      \
                    {BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_NAME_I, _, fields)},      \
                    BOOST_PP_SEQ_FOR_EACH_I(LLU_FIELD_REF_I, obj., fields));     \
        return stream;                                                           \
    }
#define LLU_REGISTER_OUTPUT_TYPE(Type, ...) LLU_REGISTER_OUTPUT_TYPE_IMPL(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

namespace LLU {
    using WSNativeStream = WSStream<WS::Encoding::Native>;

    namespace WS {
        using NativeString = String<WS::Encoding::Native>;
    }

    inline const std::vector<std::pair<std::string, std::string>> PacletErrors = {
            {"UnknownNameError", "Unknown name `name`"}
    };

    inline WSNativeStream &operator>>(WSNativeStream &stream, size_t &value) {
        long long temp;
        stream >> temp;
        assert(temp >= 0);
        value = temp;
        return stream;
    }

    template<typename... Args>
    void ReadObject(WSNativeStream &stream,
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

    inline WSNativeStream &operator<<(WSNativeStream &stream, size_t value) {
        assert(value <= std::numeric_limits<long long>::max());
        return stream << static_cast<long long>(value);
    }

    inline WSNativeStream &operator<<(WSNativeStream &stream, std::string_view str) {
        WS::NativeString::put(stream.get(), str.data(), static_cast<int>(str.size()));
        return stream;
    }

    template<typename T>
    WSNativeStream &operator<<(WSNativeStream &stream, const std::optional<T> &optional) {
        if (optional.has_value()) return stream << optional.value();
        return stream << WS::Missing();
    }

    template<typename T1, typename T2>
    WSNativeStream &operator<<(WSNativeStream &stream, const std::pair<T1, T2> &pair) {
        return stream << WS::List(2) << pair.first << pair.second;
    }

    template<typename... Args>
    WSNativeStream &operator<<(WSNativeStream &stream, const std::tuple<Args...> &tuple) {
        stream << WS::List(sizeof...(Args));
        [&]<std::size_t... Ints>(std::index_sequence<Ints...>) {
            ((stream << std::get<Ints>(tuple)), ...);
        }(std::index_sequence_for<Args...>());
        return stream;
    }

    template<typename... Args>
    void WriteObject(WSNativeStream &stream,
                     const std::array<std::string_view, sizeof...(Args)> &fieldNames,
                     const Args &... fields) {
        stream << WS::Association(sizeof...(Args));
        ForEachCRefIndexed([&](size_t i, const auto &field) {
            stream << WS::Rule << fieldNames[i] << field;
        }, fields...);
    }

    template<typename... Args>
    void EvaluateFunction(WSNativeStream &stream,
                          const std::array<std::string_view, sizeof...(Args)> &argNames,
                          std::string_view body,
                          const Args &... args) {
        std::array<WS::Symbol, sizeof...(Args)> params;
        std::transform(argNames.cbegin(), argNames.cend(), params.begin(),
                       [](std::string_view argName) { return WS::Symbol(std::string(argName)); });
        stream << WS::Function("EvaluatePacket", 1) << WS::Function("ReleaseHold", 1) << WS::Function("Apply", 2)
               << WS::Function("Function", 2) << params << WS::Function("Evaluate", 1)
               << WS::Function("ToExpression", 3) << body << WS::Symbol("InputForm") << WS::Symbol("Hold")
               << std::tie(args...);
        LibraryData::API()->processWSLINK(stream.get());
        stream >> LLU::WS::Function("ReturnPacket", 1);
    }
}

extern "C" DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData);
