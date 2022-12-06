#pragma once

#include "trimeshes/trimesh.h"
#include "utils/scenedata.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct MeshPrimitive {
    // vertex/trimesh data
    TrimeshData* trimesh;

    // primitive data
    PrimitiveType type;
    SceneMaterial material;

    // for world space conversions
    glm::mat4 modelMatrix;
    glm::mat4 invTransposeModelMatrix;

    // for openGL bindings
    GLuint vbo;
    GLuint vao;

    // extra credit
    float adaptiveMultiple;
};

