//
// Created by nic on 01/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELFRAGMENTTYPES_H_
#define CUBICAD_VOXELFRAGMENTTYPES_H_

#include <cstddef>
#include <utility>
#include <tuple>

#include "VoxelUtils.h"


template<typename ...Types>
class VoxelFragment {
  public:
    static constexpr size_v getFragmentTotalSize();
    static constexpr size_v getFragmentCount();
    template<size_t nFragment>
    static constexpr size_v getFragmentSize();
    template<size_t nFragment>
    static constexpr size_v getFragmentOffset();
    template<size_t nFragment>
    static constexpr decltype(auto) getFragmentType();
};

template<typename ...Types>
constexpr size_v VoxelFragment<Types...>::getFragmentTotalSize() {
    return (sizeof(Types) + ...);
}

template<typename ...Types>
constexpr size_v VoxelFragment<Types...>::getFragmentCount() {
    return sizeof...(Types);
}

template<std::size_t I, typename ...Types>
constexpr auto get_size_of_n() {
    return sizeof(std::tuple_element_t<I, std::tuple<Types...>>);
}

template<typename ...Types>
template<size_t nFragment>
constexpr size_v VoxelFragment<Types...>::getFragmentSize() {
    return get_size_of_n<nFragment, Types...>();
}

template<typename ...Types, std::size_t ...I>
constexpr auto get_size_to_n(std::index_sequence<I...>) {
    constexpr size_t values[] = {sizeof(Types)...};
    return (values[I] + ...);
}

template<std::size_t N, typename ...Types>
constexpr auto get_size_to_n() {
    return get_size_to_n<Types...>(std::make_index_sequence<N>());
}

template<typename ...Types>
template<std::size_t nFragment>
constexpr size_v VoxelFragment<Types...>::getFragmentOffset() {
    return get_size_to_n<nFragment + 1, Types...>();
}

template<typename ...Types>
template<std::size_t nFragment>
constexpr decltype(auto) VoxelFragment<Types...>::getFragmentType() {
    return typename std::tuple_element<nFragment, std::tuple<Types...>>::type();
}

#endif //CUBICADVOXELTYPES__VOXELFRAGMENTTYPES_H_
