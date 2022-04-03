#pragma once

#include "Core/Core.h"

template<typename Function, typename... Args>
constexpr void ForEachRef(Function &&f, Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)), ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachRefUntil(Function &&f, Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)) || ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachRefIndexed(Function &&f, Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)), ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachRefIndexedUntil(Function &&f, Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)) || ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachCRef(Function &&f, const Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)), ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachCRefUntil(Function &&f, const Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)) || ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachCRefIndexed(Function &&f, const Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)), ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Function, typename... Args>
constexpr void ForEachCRefIndexedUntil(Function &&f, const Args &...args) {
    []<std::size_t... Ints>(Function &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)) || ...);
    }(std::forward<Function>(f), std::tie(args...), std::index_sequence_for<Args...>());
}
