//
// Created by nic on 13/08/2021.
//

#include "eventbus.h"


void RenderBus::runRenderLoop(std::shared_ptr<Device> device) {
    eventBus->postpone(PreRenderAcquireFrameEvent{{device}});
    eventBus->process();
    eventBus->postpone(PreRenderUpdateBuffersEvent{{device}});
    eventBus->process();
    eventBus->postpone(PreRenderResizeEvent{{device}});
    eventBus->process();
    eventBus->postpone(RenderSubmitQueueEvent{{device}});
    eventBus->process();
    eventBus->postpone(PostRenderSubmitQueueEvent{{device}});
    eventBus->process();
    eventBus->postpone(PostRenderPresentImageEvent{{device}});
    eventBus->process();
    eventBus->postpone(PostRenderRecreateEvent{{device}});
    eventBus->process();
}

void RenderBus::runRenderLoop_(std::shared_ptr<Device> device) {
    eventBus->postpone(PreRenderAcquireFrameEvent{{device}});
    eventBus->process();
}

RenderBus::RenderBus() {
    eventBus = std::make_shared<dexode::EventBus>();

    dexode::EventBus::Listener listener = dexode::EventBus::Listener{eventBus};

    listener.listen<PreRenderResizeEvent>([this](const auto &event) { std::cout << "test"; });
}

RenderBus publicRenderBus = RenderBus();
