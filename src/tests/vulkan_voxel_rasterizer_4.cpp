//
// Created by nic on 07/12/2021.
//

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <bitset>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>

#include <glm/glm.hpp>
#include <unordered_set>

#include "../renderer/vulkan/window.h"
#include "../renderer/vulkan/swapchain.h"
#include "../renderer/modelloader.h"
#include "../renderer/scene.h"
#include "../renderer/ui/uirenderer.h"
#include "../renderer/framebufferselector.h"
#include "../renderer/ui/objectproperties.h"
#include "../renderer/ui/objectlist.h"
#include "../renderer/ui/mainmenu.h"
#include "../renderer/texturelibrary.h"
#include "../renderer/rendermanager.h"


const uint32_t WIDTH = 1080 * 2;
const uint32_t HEIGHT = 720 * 2;

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class VulkanRasterizer {
  public:
    void run() {
        std::cout << "starting application\n";

        init();
        mainLoop();
        cleanup();
    }

  private:
    uint32_t version = VK_MAKE_VERSION(0, 3, 0);
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Window> window;

    std::shared_ptr<Mesh> model;
    std::vector<std::shared_ptr<MeshInstance>> objects;

    std::shared_ptr<ObjectProperties> objectProperties;
    std::shared_ptr<ObjectList> objectList;
    std::shared_ptr<MainMenu> menuBar;

    std::shared_ptr<RenderManager> renderManager;

    void initWindow() {
        Window::initWindows();
        window = Window::create(instance, "API Test " + std::to_string(version), WIDTH, HEIGHT);
    }

    void createRenderManager() {
        TextureQualitySettings settings{};
        settings.anisotropy = 1.0f;
        settings.mipLevels = 16;

        RenderQualityOptions options{};
        options.SSAOSampleCount = 32;
        options.SSAOSampleRadius = 0.05f;
        options.enableSSAO = true;

        CameraModel cameraModel{};
        cameraModel.fieldOfView = 75.0;
        cameraModel.cameraMode = CAMERA_MODE_PERSPECTIVE_FINITE;
        cameraModel.clipNear = 0.1;
        cameraModel.clipFar = 1000;

        renderManager = RenderManager::create(instance, window, settings, options, cameraModel);
    }

    void init() {
        createInstance();
        initWindow();
        createRenderManager();
        loadModels();
        createScene();
        createUI();
    }

    void createUI() {
        objectProperties = ObjectProperties::create(renderManager->getScene());
        renderManager->getUIRenderer()->submitDrawable(objectProperties);
        objectList = ObjectList::create(renderManager->getScene());
        renderManager->getUIRenderer()->submitDrawable(objectList);
        renderManager->getSceneWriter()
            ->setFilename(renderManager->getSceneWriter()->getFilename() + "/resources/saves/test.ccs");
        menuBar = MainMenu::create(renderManager);
        renderManager->getUIRenderer()->submitDrawable(menuBar);
    }

    void loadModels() {
        //renderManager->loadMesh("resources/models/demo/living_room/living_room.obj", true);
        renderManager->getScene()->bakeMaterials(true);
        renderManager->getScene()->collectRenderBuffers();
    }

    void createScene() {
        for (const auto &object: objects)
            renderManager->getScene()->submitInstance(object);
        renderManager->updateRenderData();
    }

    void mainLoop() {
        std::cout << "entering main loop\n";

        while (!glfwWindowShouldClose(window->getWindow())) {
            glfwPollEvents();

            renderManager->registerFunctionNextFrame([&]() { renderManager->processInputs(); });
            renderManager->registerFunctionNextFrame([&]() {
              objectProperties->setObjectByID(renderManager->getScene()->getSelected());
            });
            renderManager->registerFunctionNextFrame([&]() { renderManager->drawFrame(); });

            renderManager->callFunctions();
        }

        std::cout << "exiting\n";
    }

    static void cleanup() {
        std::cout << "cleaning up\n";

        glfwTerminate();
    }

    void createInstance() {
        glfwInit();

#if NDEBUG
        instance = Instance::create("api_test", version, false);
#else
        instance = Instance::create("api_test", version, false);
#endif
    }
};

int main() {
    VulkanRasterizer app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "application closed\n";

    return EXIT_SUCCESS;
}
