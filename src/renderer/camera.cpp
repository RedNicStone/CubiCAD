//
// Created by nic on 16/01/2022.
//

#include "camera.h"


std::shared_ptr<Camera> Camera::create(CameraModel cameraModel, VkExtent2D window) {
    auto camera = std::make_shared<Camera>();

    camera->updateCameraModel(cameraModel, window);

    camera->position = {2, 2, 2};
    camera->rotation = {-2, -2, -2};

    camera->view = glm::lookAt(camera->position, camera->position + camera->rotation, {0, 1, 0});

    return camera;
}

void Camera::updateCameraModel(CameraModel cameraModel, VkExtent2D window) {
    auto fov = glm::radians(cameraModel.fieldOfView);
    switch (cameraModel.cameraMode) {
        case CAMERA_MODE_PERSPECTIVE_INFINITE: projection =
                                                   glm::infinitePerspective(fov,
                                                                            (double) window.width / window.height,
                                                                            cameraModel.clipNear);
            break;
        case CAMERA_MODE_PERSPECTIVE_FINITE: projection =
                                                 glm::perspective(fov,
                                                                  (double) window.width / window.height,
                                                                  cameraModel.clipNear,
                                                                  cameraModel.clipFar);
            break;
        case CAMERA_MODE_ORTHOGRAPHIC_INFINITE:
            projection =
                glm::ortho(-window.width / 2,
                           window.width / 2,
                           -window.height / 2,
                           window.height / 2);
            break;
        case CAMERA_MODE_ORTHOGRAPHIC_FINITE:
            projection =
                glm::ortho((double) -window.width / 2,
                           (double) window.width / 2,
                           (double) -window.height / 2,
                           (double) window.height / 2,
                           cameraModel.clipNear,
                           cameraModel.clipFar);
            break;
    }
    projection[1][1] *= -1;
}

void Camera::update() {
    view = glm::lookAt(position, position + rotation, {0, 1, 0});
}

void Camera::rotate(float angle, glm::vec3 axis) {
    rotation = glm::rotate(rotation, angle, axis);
}

void Camera::face(glm::vec3 direction) {
    rotation = direction;
}

void Camera::move(glm::vec3 pos) {
    position += pos;
}

void Camera::teleport(glm::vec3 pos) {
    position = pos;
}

void Camera::lookAt(glm::vec3 pos) {
    rotation = glm::normalize(pos - position);
}
