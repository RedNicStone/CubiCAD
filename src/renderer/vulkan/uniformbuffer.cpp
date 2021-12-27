//
// Created by nic on 25/09/2021.
//

#include "uniformbuffer.h"


template<class type>
void UniformBuffer<type>::updateBufferContents() {
    void *mapDst = buffer->map();
    memcpy(mapDst, &data, sizeof(type));
    buffer->unmap();
}
