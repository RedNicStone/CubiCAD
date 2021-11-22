//
// Created by nic on 23/04/2021.
//

#include <array>
#include <cstddef>
#include <type_traits>


#ifndef CUBICAD_VEC_ABS_H
#define CUBICAD_VEC_ABS_H


// this section is to createInstance an typedef func all_same which checks if all elements are of the same type

template<typename T, typename ...Ts>
constexpr bool all_convertible_to() {
    return (std::is_convertible_v<T, Ts> && ...);
}

template<typename T, size_t N>
class vec_abs {
  public:
    vec_abs();  // construct from another vector
    vec_abs(const vec_abs &vec);  // construct from another vector
    explicit vec_abs(const std::array<T, N> &arr);  // construct from array

    template<typename ...Ts> requires // only compile if:
    std::conjunction<std::is_convertible<Ts, T>...>::value
    &&  // all can be converted to T
    (sizeof...(Ts) == N)  // there are N arguments
    explicit vec_abs(const Ts &&... values);  // construct from multiple arguments

  private:
    std::array<T, N> data;
};

#endif //CUBICAD_VEC_ABS_H
