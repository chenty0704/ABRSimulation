#pragma once

#include <tuple>
#include <utility>

template<typename Fun, typename... Args>
constexpr void ForEachRef(Fun &&f, Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)), ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachRefUntil(Fun &&f, Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)) || ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachRefIndexed(Fun &&f, Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)), ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachRefIndexedUntil(Fun &&f, Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)) || ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachCRef(Fun &&f, const Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)), ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachCRefUntil(Fun &&f, const Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(std::get<Ints>(tuple)) || ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachCRefIndexed(Fun &&f, const Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)), ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}

template<typename Fun, typename... Args>
constexpr void ForEachCRefIndexedUntil(Fun &&f, const Args &...args) {
    []<std::size_t... Ints>(Fun &&f, std::tuple<const Args &...> &&tuple, std::index_sequence<Ints...>) {
        (f(Ints, std::get<Ints>(tuple)) || ...);
    }(std::forward<Fun>(f), std::tie(args...), std::index_sequence_for<Args...>());
}
