//
// Created by nic on 29/05/2021.
//

#include "pushconstant.h"


template<class type>
PushConstantRange::PushConstantRange() {
    static_assert(sizeof(type) % 4 == 0);
    handle.size = sizeof(type);
    handle.offset = 0;
}