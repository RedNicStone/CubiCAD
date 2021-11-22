//
// Created by nic on 23/04/2021.
//

#include "vec_abs.h"


template<typename T, size_t N>
vec_abs<T, N>::vec_abs() {
    data = std::array<T, N>();
}

template<typename T, size_t N>
vec_abs<T, N>::vec_abs(const vec_abs &vec) {
    this = vec;
}

template<typename T, size_t N>
vec_abs<T, N>::vec_abs(const std::array<T, N> &arr) {
    data = arr;
}

template<typename T, size_t N>
template<typename ...Ts> requires // only compile if:
std::conjunction<std::is_convertible<Ts, T>...>::value
&&  // all can be converted to T
(sizeof...(Ts) == N)  // there are N arguments
vec_abs<T, N>::vec_abs(const Ts &&... values) {

}

