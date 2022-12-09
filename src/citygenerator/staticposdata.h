#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>


// define grayscale/white color constants here
#define DIFFUSE_WHITE_1 glm::vec3(0.95f, 0.95f, 0.95f)


struct ObjSceneData {
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};


//static ObjSceneData PLANE = {.translation = glm::vec3(0.f),
//                                   .scale = glm::vec3(20.f, 0.1f, 20.f),
//                                   .diffuse = DIFFUSE_WHITE_1,
//                                   .specular = glm::vec3(0.f),
//                                   .shininess = 0.f};

static ObjSceneData PLANE = {.translation = glm::vec3(0.f, -10.f, 0.f),
                                   .scale = glm::vec3(40.f, 10.f, 200.f),
                                   .diffuse = DIFFUSE_WHITE_1,
                                   .specular = glm::vec3(0.8f, 0.8f, 0.8f),
                                   .shininess = 0.f};
