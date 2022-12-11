#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>


// define grayscale/white color constants here

#define AMBIENT_WHITE_1 glm::vec3(0.77f, 0.77f, 0.77f)

#define DIFFUSE_WHITE_1 glm::vec3(0.90f, 0.90f, 0.90f)
#define DIFFUSE_WHITE_2 glm::vec3(0.92f, 0.925f, 0.91f)
#define DIFFUSE_WHITE_3 glm::vec3(0.94f, 0.93f, 0.95f)

#define SPECULAR_WHITE_1 glm::vec3(0.6f, 0.6f, 0.6f)
#define SPECULAR_WHITE_2 glm::vec3(0.7f, 0.7f, 0.7f)

struct ObjSceneData {
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
    float shininess;
};


//static ObjSceneData PLANE = {.translation = glm::vec3(0.f),
//                                   .scale = glm::vec3(20.f, 0.1f, 20.f),
//                                   .diffuse = DIFFUSE_WHITE_1,
//                                   .specular = glm::vec3(0.f),
//                                   .shininess = 0.f};

static ObjSceneData PLANE = {.translation = glm::vec3(0.f, -8.f, -120.f),
                                   .scale = glm::vec3(100.f, 10.f, 300.f),
                                   .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
//                                   .diffuse = DIFFUSE_WHITE_1,
                                   .specular = glm::vec3(0.5f, 0.5f, 0.5f),
                                   .ambient = AMBIENT_WHITE_1,
                                   .shininess = 0.f};


//static ObjSceneData LEFTFACADEPLACE = {.translation = glm::vec3(-15.f, -4.f, 0.f),
//                                   .scale = glm::vec3(10.f, 10.f, 100.f),
//                                   .diffuse = DIFFUSE_WHITE_1,
//                                   .specular = SPECULAR_WHITE_1,
//                                   .ambient = AMBIENT_WHITE_1,
//                                   .shininess = 0.f};


//static ObjSceneData RIGHTFACADEPLACE = {.translation = glm::vec3(15.f, -4.f, 0.f),
//                                   .scale = glm::vec3(10.f, 10.f, 100.f),
//                                   .diffuse = DIFFUSE_WHITE_2,
//                                   .specular = SPECULAR_WHITE_1,
//                                   .ambient = AMBIENT_WHITE_1,
//                                   .shininess = 0.f};


static ObjSceneData BACKFACADE = {.translation = glm::vec3(0.f, -4.f, -200.f),
                                   .scale = glm::vec3(1000.f, 100.f, 10.f),
                                   .diffuse = glm::vec3(0.6f, 0.6f, 0.6f),
                                   .specular = SPECULAR_WHITE_2,
                                   .ambient = glm::vec3(0.6f, 0.6f, 0.6f),
                                   .shininess = 0.f};
