#pragma once

#include "staticposdata.h"
#include "utils/scenedata.h"
#include <glm/gtx/transform.hpp>

struct CityMeshObject {
    // vertex/trimesh data
//    TrimeshData* trimesh; // UNUSED RIGHT NOW

    SceneMaterial material;

    // for world space conversions
    glm::mat4 modelMatrix;
    glm::mat4 invTransposeModelMatrix;

    // for openGL bindings
    GLuint vbo;
    GLuint vao;
};

inline void InitializeSpaceConversions(CityMeshObject *obj, ObjSceneData *sceneData) {

    glm::mat4 T = glm::translate(sceneData->translation);
    glm::mat4 S = glm::scale(sceneData->scale);

    obj->modelMatrix = T * S;
    obj->invTransposeModelMatrix = glm::inverse(glm::transpose(obj->modelMatrix));

}


inline void InitializeMaterial(CityMeshObject *obj, ObjSceneData *sceneData) {

   // TODO: more here
   obj->material.cDiffuse = glm::vec4(sceneData->diffuse, 0.f);
   obj->material.cSpecular = glm::vec4(sceneData->specular, 0.f);
   obj->material.shininess = sceneData->shininess;
   obj->material.cAmbient = glm::vec4(sceneData->ambient, 0.f);
}
