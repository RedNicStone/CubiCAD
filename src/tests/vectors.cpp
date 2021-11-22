//
// Created by nic on 23/04/2021.
//

#include "Math/vec_abs.h"
#include <array>


int main() {
    vec_abs<int, 3> veci3_0{};

    vec_abs<int, 3> veci3_1(1, 2, 3);

    std::array<int, 3> arr{1, 2, 3};
    vec_abs<int, 3> veci3_2(arr);

    vec_abs<int, 3> veci3_3(veci3_2);
}