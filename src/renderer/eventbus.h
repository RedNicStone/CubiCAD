//
// Created by nic on 13/08/2021.
//

#pragma once

#ifndef CUBICAD_EVENTBUS_H
#define CUBICAD_EVENTBUS_H

#include <dexode/EventBus.hpp>

#include "vulkan/device.h"  //todo make device independent


struct RenderEvent {
    std::shared_ptr<Device> device;
};

struct PreRenderAcquireFrameEvent : public RenderEvent {};
struct PreRenderUpdateBuffersEvent : public RenderEvent {};
struct PreRenderResizeEvent : public RenderEvent {};
struct RenderSubmitQueueEvent : public RenderEvent {};
struct PostRenderSubmitQueueEvent : public RenderEvent {};
struct PostRenderPresentImageEvent : public RenderEvent {};
struct PostRenderRecreateEvent : public RenderEvent {};

class RenderBus {
  private:
    std::shared_ptr<dexode::EventBus> eventBus;

  public:
    RenderBus();

    void runRenderLoop(std::shared_ptr<Device> device);
    void runRenderLoop_(std::shared_ptr<Device> device);

    std::shared_ptr<dexode::EventBus> getBus() { return eventBus; }
};

extern RenderBus publicRenderBus;

#endif //CUBICAD_EVENTBUS_H
