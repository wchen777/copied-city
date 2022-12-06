#pragma once

#include <glm/glm.hpp>
#include "../utils/scenedata.h"
#include "transforms.h"


// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:

//    Camera(); // default constructor

    Camera(SceneCameraData camData, int height, int width, float farPlane, float nearPlane) :
       look{glm::vec3(camData.look)},
       pos{glm::vec3(camData.pos)},
       up{glm::vec3(camData.up)},
       camData{camData},
       aspectRatio{static_cast<float>(width) / static_cast<float>(height)},
       farPlane(farPlane),
       nearPlane(nearPlane),
       width(width),
       height(height)
    {
        setViewMatrices();
    }

    glm::vec3 up;
    glm::vec3 look;
    glm::vec3 pos;

    float aspectRatio; // this is width / height!!
    float farPlane;
    float nearPlane;

    int height;
    int width;

    SceneCameraData camData;

    glm::mat4 invViewMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    glm::mat4 projViewMatrix; // PV matrix

    void setViewMatrices(); // set the view matrix fields

    void updateViewPlanes(float farPlaneNew, float nearPlaneNew); // when the far and new planes are updated

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;
    glm::mat4 getInvViewMatrix() const;
    glm::mat4 getProjMatrix() const;
    glm::mat4 getProjViewMatrix() const;

//    glm::mat4 currentTranslation = glm::mat4(1.f); // current translation for rebuilding
//    glm::mat4 currentRotation = glm::mat4(1.f); // current rotation for rebuilding

//    void updateViewMatricesTranslate(glm::mat4& translate); // helper for camera transformation
//    void updateViewMatricesRotation(glm::mat4& rotation); // helper for camera rotation

    void ApplyTranslation(glm::vec3& translation);
    void ApplyRotation(glm::mat3& rotation);

    // W: Translates the camera in the direction of the look vector
    glm::vec3 WPressed();

    // S: Translates the camera in the opposite direction of the look vector
    glm::vec3 SPressed();

    // A: Translates the camera in the left direction, perpendicular to the look and up vectors
    glm::vec3 APressed();

    // D: Translates the camera in the right direction, also perpendicular to the look and up vectors.
    // This movement should be opposite to that of pressing A
    glm::vec3 DPressed();

    // Space: Translates the camera along the world space vector (0,1,0)
    static glm::vec3 SpacePressed();

    // Ctrl: Translates the camera along the world space vector (0,-1,0)
    static glm::vec3 CtrlPressed();

    void RotateX(float deltaX); // rotate about world space vector 0,1,0
    void RotateY(float deltaY); // Rotates the camera about the axis defined by a vector perpendicular to the look and up vectors

    // update aspect ratio for resize
    void updateAspectRatio(float aspectRatio);

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
};
