#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>


// define grayscale/white color constants here
#define DIFFUSE_WHITE_1 glm::vec3(0.9f, 0.9f, 0.9f)


struct ObjSceneData {
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};


static ObjSceneData PLANE = {.translation = glm::vec3(0.f),
                                   .scale = glm::vec3(20.f, 0.1f, 20.f),
                                   .diffuse = DIFFUSE_WHITE_1,
                                   .specular = glm::vec3(0.f),
                                   .shininess = 0.f};
