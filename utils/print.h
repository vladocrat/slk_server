#pragma once

#include <iostream>
#include <tuple>

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template <typename Tuple, std::size_t Index = 0>
void print_impl(const Tuple& tup, std::true_type) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
        std::cout << std::get<Index>(tup);

        if constexpr (Index + 1 < std::tuple_size_v<Tuple>) {
            std::cout << ", ";
        }

        print_impl<Tuple, Index + 1>(tup, std::true_type{});
    }
}

template <typename Container>
void print_impl(const Container& container, std::false_type) {
    auto it = container.begin();
    auto end = container.end();

    std::cout << "{";
    for (; it != end; ++it) {
        std::cout << *it;
        if (std::next(it) != end) {
            std::cout << ", ";
        }
    }
    std::cout << "}";
}

template <typename T>
void print(const T& obj) {
    if constexpr (is_tuple<T>::value) {
        std::cout << "(";
        print_impl(obj, std::true_type{});
        std::cout << ")";
    } else {
        print_impl(obj, std::false_type{});
    }
}
