//
// Created by nic on 16/01/2022.
//

#pragma once

#ifndef CUBICAD_CAMERA_H
#define CUBICAD_CAMERA_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>


/// Enumeration storing possible camera configurations
enum CameraMode : uint8_t {
    CAMERA_MODE_PERSPECTIVE_INFINITE = 1,
    CAMERA_MODE_PERSPECTIVE_FINITE = 2,
    CAMERA_MODE_ORTHOGRAPHIC_INFINITE = 3,
    CAMERA_MODE_ORTHOGRAPHIC_FINITE = 4
};

/// Struct storing a camera model
struct CameraModel {
    CameraMode cameraMode;  //< Camera mode
    double fieldOfView;     //< Field of view factor
    double clipNear;        //< Near clipping distance
    double clipFar;         //< Far clipping distance
};

class Camera {
  private:
    glm::vec3 position;
    glm::vec3 rotation;

    glm::mat4 view;
    glm::mat4 projection;

    CameraModel model{};

  public:
    /// Create a new camera
    /// \param cameraModel Camera model the camera should use
    /// \param window Window the camera should operator on
    /// \return Handle to the new camera
    static std::shared_ptr<Camera> create(CameraModel cameraModel, VkExtent2D window);

    /// Update the camera model
    /// \param cameraModel Camera model to use
    /// \param window Window to operate on
    void updateCameraModel(CameraModel cameraModel, VkExtent2D window);
    /// Update all camera data
    void update();

    void rotate(float angle, glm::vec3 axis);
    void face(glm::vec3 direction);
    void move(glm::vec3 pos);
    void teleport(glm::vec3 pos);

    void lookAt(glm::vec3 pos);

    glm::vec3 getPosition() { return position; }

    glm::vec3 getRotation() { return rotation; }

    glm::mat4 getView() { return view; }

    glm::mat4 getProj() { return projection; }

    CameraModel getCameraModel() { return model; }
};

#endif //CUBICAD_CAMERA_H
