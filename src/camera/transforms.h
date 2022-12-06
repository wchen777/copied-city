#pragma once

#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <glm/glm.hpp>

glm::mat4 getScalingMatrix(float sx, float sy, float sz);

glm::mat4 getRotationMatrixX(float angle);

glm::mat4 getRotationMatrixY(float angle);

glm::mat4 getRotationMatrixZ(float angle);

glm::mat3 getRotationMatrixY3D(float angle);

glm::mat4 getTranslationMatrix(float dx, float dy, float dz);

glm::mat3 getAxisAngleRotationMatrix3D(glm::vec3& axis, float angle);

#endif // TRANSFORMS_H
