#include <iostream>
#include <ostream>
#include <stdexcept>
#include "camera.h"
//#include "glm/gtx/transform.hpp"
//#include "glm/ext/quaternion_transform.hpp"
//#include "glm/ext/matrix_clip_space.hpp"
//#include "glm/ext/matrix_transform.hpp"

// personally think these factors make the sensitivity better
#define ROTATION_SCALE 0.3f
#define TRANS_SCALE 0.4f

void Camera::setViewMatrices() {

    auto w = glm::normalize(-1.f * look);

    auto vTop = Camera::up - (glm::dot(Camera::up, w) * w);
    auto v =  glm::normalize(vTop);

    auto u = glm::cross(v, w);

    auto Mtranslate = getTranslationMatrix(-1 * Camera::pos[0], -1 * Camera::pos[1], -1 * Camera::pos[2]);
    auto Mrotate = glm::mat4(
                u[0], v[0], w[0], 0.f,
                u[1], v[1], w[1], 0.f,
                u[2], v[2], w[2], 0.f,
                0.f, 0.f, 0.f, 1.f
            ) ;

    auto viewM = Mrotate * Mtranslate;

    Camera::viewMatrix = viewM;
    Camera::invViewMatrix = glm::inverse(viewM);

    float c = -Camera::nearPlane / Camera::farPlane;
    auto unhingingMatrix = glm::mat4(
                    1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f/(1.f+c), -1.f,
                    0.f, 0.f, -c/(1.f+c), 0.f);


    float tanHalfHeightAngle = glm::tan(Camera::camData.heightAngle / 2.f);

//    // tan (theta_h / 2) = (height / 2) / k
//    float k = static_cast<float>(Camera::height / 2.f) / tanHalfHeightAngle;

    // tan(theta_w / 2) = (width / 2) / k
//    float tanHalfWidthAngle = static_cast<float>(Camera::width / 2.f) / k;
    float tanHalfWidthAngle = Camera::aspectRatio * tanHalfHeightAngle;

    auto scaleMatrix = glm::mat4(
                        1.f/(Camera::farPlane * tanHalfWidthAngle), 0.f, 0.f, 0.f,
                        0.f, 1.f/(Camera::farPlane * tanHalfHeightAngle), 0.f, 0.f,
                        0.f, 0.f, 1.f/Camera::farPlane, 0.f,
                        0.f, 0.f, 0.f, 1.f);

    glm::mat4 zRemapMatrix = glm::mat4(
                        1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, -2.f, 0.f,
                        0.f, 0.f, -1.f, 1.f);

    Camera::projMatrix = zRemapMatrix * unhingingMatrix * scaleMatrix;
//    Camera::projMatrix = glm::perspective(glm::degrees(Camera::camData.heightAngle), aspectRatio, Camera::nearPlane, Camera::farPlane);
//    Camera::viewMatrix = glm::lookAt(Camera::pos, Camera::look, Camera::up);
    Camera::projViewMatrix =  Camera::projMatrix * Camera::viewMatrix; // MVP = P * V * M
}

void Camera::updateViewPlanes(float farPlaneNew, float nearPlaneNew) {

    Camera::farPlane = farPlaneNew;
    Camera::nearPlane = nearPlaneNew;

    Camera::setViewMatrices();
}

void Camera::updateAspectRatio(float aspectRatio) {
    Camera::aspectRatio = aspectRatio;

    setViewMatrices();
}

glm::mat4 Camera::getViewMatrix() const {
    return Camera::viewMatrix;
}

glm::mat4 Camera::getInvViewMatrix() const {
    return Camera::invViewMatrix;
}

glm::mat4 Camera::getProjMatrix() const {
    return Camera::projMatrix;
}

glm::mat4 Camera::getProjViewMatrix() const {
    return Camera::projViewMatrix;
}

float Camera::getAspectRatio() const {
    return Camera::aspectRatio;
}

float Camera::getHeightAngle() const {
    return Camera::camData.heightAngle;
}

float Camera::getFocalLength() const {
    return Camera::camData.focalLength;
}

float Camera::getAperture() const {
    return Camera::camData.aperture;
}


// ------------- ACTION ------------- //


//void Camera::updateViewMatricesTranslate(glm::mat4& translate) {

//    // set view matrices
//    Camera::viewMatrix =  Camera::viewMatrix * translate;
//    Camera::invViewMatrix = glm::inverse(viewMatrix);

//    // set projecion and view matices
//    Camera::projViewMatrix =  Camera::projMatrix * Camera::viewMatrix;

//    // accumulate current translation
////    Camera::pos = glm::vec3(translate * glm::vec4(Camera::pos, 1.f));
//    Camera::currentTranslation = Camera::currentTranslation * translate;
//}

//void Camera::updateViewMatricesRotation(glm::mat4& rotation) {
//    // account for current translation

//    // set view matrices
//    Camera::viewMatrix =  rotation * Camera::viewMatrix;
//    Camera::invViewMatrix = glm::inverse(viewMatrix);

//    // set projecion and view matices
//    Camera::projViewMatrix = Camera::projMatrix * Camera::viewMatrix;

////    Camera::setViewMatrices();

//    // accumulate current translation
//    Camera::currentRotation = rotation * Camera::currentRotation;
//}


// apply a translation vector to the camera's position vector
void Camera::ApplyTranslation(glm::vec3& translation) {

    Camera::pos += translation;

    setViewMatrices();
}

// apply a rotation vector to the camera's look vector
void Camera::ApplyRotation(glm::mat3& rotation) {

    Camera::look = rotation * Camera::look;

    setViewMatrices();
}


glm::vec3 Camera::WPressed() {
    // translate along direciton of look vector

    return 0.75f * Camera::look;

//    // DEFAULT SPEED IS TOO FAST FOR ME TO TEST
//    speed = TRANS_SCALE * speed;

////    auto worldLook = glm::vec3(Camera::viewMatrix * glm::vec4(Camera::look, 0.f));

//    // translation matrix
//    auto transMat = getTranslationMatrix(-Camera::look[0] * speed, -Camera::look[1] * speed, -Camera::look[2] * speed);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);

}


glm::vec3 Camera::SPressed() {
    // translate along opposite direciton of look vector

    return -0.75f * Camera::look;

//    speed = TRANS_SCALE * speed;

//    auto worldLook = glm::vec3(Camera::viewMatrix * glm::vec4(Camera::look, 0.f));

//    // translation matrix
//    auto transMat = getTranslationMatrix(Camera::look[0] * speed, Camera::look[1] * speed, Camera::look[2] * speed);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);
}


glm::vec3 Camera::APressed() {

    return -0.4f * glm::cross(Camera::look, Camera::up);
//    speed = TRANS_SCALE * speed;
//    // translation in left direction
//    auto leftDir = glm::cross(Camera::look, Camera::up);

//    // translation matrix
//    auto transMat = getTranslationMatrix(leftDir[0] * speed, leftDir[1] * speed, leftDir[2] * speed);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);
}

glm::vec3 Camera::DPressed() {

    // scale based on user sensitivity
    return 0.4f * glm::cross(Camera::look, Camera::up);
//    speed = TRANS_SCALE * speed;
//    // translation in right direction

//    // translation matrix
//    auto transMat = getTranslationMatrix(rightDir[0] * 0.8f * speed, rightDir[1] *  0.8f * speed, rightDir[2] *  0.8f * speed);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);
}

glm::vec3 Camera::SpacePressed() {

    return glm::vec3(0.f, 2.f, 0.f);
//    speed = TRANS_SCALE * speed;

//    // translate in direction of 0,1,0
//    auto transMat = getTranslationMatrix(0, -3.f * speed, 0);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);
}

glm::vec3 Camera::CtrlPressed() {

    return glm::vec3(0.f, -2.f, 0.f);

//    speed = TRANS_SCALE * speed;

//    // translate in direciton of 0,-1,0
//    auto transMat = getTranslationMatrix(0, 3.f * speed, 0);

//    // apply to the view matrix
//    Camera::updateViewMatricesTranslate(transMat);
}


void Camera::RotateX(float deltaX) {
    // scaled angle in degrees
    auto angle = ROTATION_SCALE * 360.f * deltaX / static_cast<float>(Camera::aspectRatio * Camera::height);

    auto rotation = getRotationMatrixY3D(angle);

    Camera::ApplyRotation(rotation);
}


void Camera::RotateY(float deltaY) {
    // scaled angle in degrees
    auto angle = ROTATION_SCALE * 360.f * deltaY / static_cast<float>(Camera::height);

    // get axis in camera space
    auto axis = glm::normalize(glm::cross(Camera::look, Camera::up));

//    // get axis in world space
//    auto worldAxis = glm::vec3(Camera::viewMatrix * glm::vec4(axis, 0.f));

    // get rotation matrix
    auto rotation = getAxisAngleRotationMatrix3D(axis, angle);

    Camera::ApplyRotation(rotation);
//    Camera::updateViewMatricesRotation(rotation);
}









