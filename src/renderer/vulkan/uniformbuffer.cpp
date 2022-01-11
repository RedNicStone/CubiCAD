//
// Created by nic on 25/09/2021.
//

#include "uniformbuffer.h"


template<class type>
void UniformBuffer<type>::updateBufferContents() {
    memcpy(dataPtr, &data, sizeof(type));
}

template<class type>
UniformBuffer<type>::~UniformBuffer() {
    buffer->unmap();
}
