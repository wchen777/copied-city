#include "staticposdata.h"
#include "generator.h"
#include <glm/gtx/transform.hpp>


void InitializeSpaceConversions(CityMeshObject *obj, ObjSceneData *sceneData) {

    glm::mat4 T = glm::translate(sceneData->translation);
    glm::mat4 S = glm::scale(sceneData->scale);

    obj->modelMatrix = T * S;
    obj->invTransposeModelMatrix = glm::inverse(glm::transpose(obj->modelMatrix));

}


void InitializeMaterial(CityMeshObject *obj, ObjSceneData *sceneData) {

   // TODO: more here
   obj->material.cDiffuse = glm::vec4(sceneData->diffuse, 0.f);
   obj->material.cSpecular = glm::vec4(sceneData->specular, 0.f);
   obj->material.shininess = sceneData->shininess;
}
