//
// Created by nic on 19/01/2022.
//

#include "uirenderer.h"

#include <utility>


std::shared_ptr<UIRenderer> UIRenderer::create(const std::shared_ptr<Queue>& graphicsQueue,
                                               const std::shared_ptr<CommandPool>& transferPool,
                                               const std::shared_ptr<RenderPass>& renderPass,
                                               const std::shared_ptr<Window>& window,
                                               uint32_t imageCount,
                                               uint32_t subpass) {
    auto uiRenderer = std::make_shared<UIRenderer>();

    std::vector<VkDescriptorPoolSize> poolSizes =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

    uiRenderer->ImGUIPool = DescriptorPool::create(renderPass->getDevice(), poolSizes, 1000);

    IMGUI_CHECKVERSION();
    uiRenderer->context = ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window->getWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = renderPass->getDevice()->getPhysicalDevice()->getInstance()->getHandle();
    initInfo.PhysicalDevice = renderPass->getDevice()->getPhysicalDevice()->getHandle();
    initInfo.Device = renderPass->getDevice()->getHandle();
    initInfo.QueueFamily = graphicsQueue->getQueueFamilyIndex();
    initInfo.Queue = graphicsQueue->getHandle();
    initInfo.DescriptorPool = uiRenderer->ImGUIPool->getHandle();
    initInfo.MinImageCount = imageCount;
    initInfo.ImageCount = imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Subpass = subpass;

    ImGui_ImplVulkan_Init(&initInfo, renderPass->getHandle());

    auto transferBuffer = CommandBuffer::create(transferPool);
    transferBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    ImGui_ImplVulkan_CreateFontsTexture(transferBuffer->getHandle());
    transferBuffer->endCommandBuffer();

    std::vector<uint32_t> accessingQueues = { transferPool->getQueueFamily()->getQueueFamilyIndex() };
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = transferBuffer->getHandlePtr();

    transferPool->getQueue()->submitCommandBuffer({ submitInfo });
    transferPool->getQueue()->waitForIdle();

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    return uiRenderer;
}

void UIRenderer::draw(const std::shared_ptr<CommandBuffer>& graphicsCommandBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!hideUI) {
        for (const auto& drawable : drawables) {
            drawable->drawUI();
        }
        ImGui::ShowDemoWindow();
    }

    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), graphicsCommandBuffer->getHandle());
}

UIRenderer::~UIRenderer() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIRenderer::setHidden(bool hide) {
    hideUI = hide;
}
