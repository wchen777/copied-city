#include "realtime.h"
#include "primitive.h"
#include "settings.h"

#include "../trimeshes/cube.h"
#include "../trimeshes/cone.h"
#include "../trimeshes/sphere.h"
#include "../trimeshes/cylinder.h"
#include "../trimeshes/obj_mesh.h"

#define DISTANCE_SCALE 10.f

/*
 * turn the parsed shapes into scene primitive mesh structs.
 * assumes scene has been parsed,
 * populates field within Realtime
*/
void Realtime::CompilePrimitiveMeshes() {
    auto objs = Realtime::sceneRenderData.shapes;

    // clear existing vector for mesh data
    Realtime::objectMeshes.clear();

    // go through each object and create a trimesh for them
    for (auto& obj : objs) {

//        std::cout << "a shape" << std::endl;

        MeshPrimitive prim = MeshPrimitive{}; // the primitive

        // TODO: trimesh OBJ stuff

        TrimeshData* trimesh; // the trimesh abstract class

        switch (obj.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            trimesh = new Cube();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            trimesh = new Cylinder();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            trimesh = new Sphere();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            trimesh = new Cone();
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            trimesh = new OBJMesh(obj.primitive.meshfile);
            break;
        default:
            trimesh = new Cube();
            break;
        }

        float adaptiveLevelOfDetailMult = Realtime::GetParamMultiple(obj.ctm);

        if (obj.primitive.type != PrimitiveType::PRIMITIVE_MESH) {
            // this builds the trimesh.
            trimesh->UpdateParams(static_cast<int>(adaptiveLevelOfDetailMult * settings.shapeParameter1),
                                  static_cast<int>(adaptiveLevelOfDetailMult * settings.shapeParameter2));
        }

        // set all remaining fields
        prim.trimesh = trimesh;
        prim.type = obj.primitive.type;
        prim.invTransposeModelMatrix = glm::inverse(glm::transpose(obj.ctm));
        prim.modelMatrix = obj.ctm;
        prim.material = obj.primitive.material;
        prim.adaptiveMultiple = adaptiveLevelOfDetailMult;

        std::cout << prim.trimesh->m_vertexData.size() << std::endl;

        Realtime::objectMeshes.emplace_back(prim);

    }

}

/*
 * update tesselation parameters for all the trimeshes.
*/
void Realtime::UpdateTesselations() {
    auto param1 = settings.shapeParameter1;
    auto param2 = settings.shapeParameter2;

    for (auto& trimesh : Realtime::objectMeshes) {
        // only meshes need to be updated

        if (trimesh.type != PrimitiveType::PRIMITIVE_MESH) {
            trimesh.trimesh->UpdateParams(static_cast<int>(trimesh.adaptiveMultiple * param1),
                                          static_cast<int>(trimesh.adaptiveMultiple * param2));
        }
    }
//    Realtime::changedScene = true; // requires a rebuild
}

/*
 * free all allocated mesh objects
*/
void Realtime::DestroyMeshes() {
    for (auto& trimesh : Realtime::objectMeshes) {
        delete trimesh.trimesh;
    }
}


/*
 * EXTRA CREDIT: distance-based tesselation parameters
*/
float Realtime::GetParamMultiple(glm::mat4 ctm) {

    if (!settings.extraCredit1) {
        return 1.f;
    }

    glm::vec3 worldCoord = glm::vec3(ctm * glm::vec4(0.f, 0.f, 0.f, 1.f));
    glm::vec3 cameraCoord = Realtime::sceneCamera->pos;

    float distance = glm::distance(cameraCoord, worldCoord);

    // if the distance is closer than distance scale -> tesselation parameters scaled up
    // if the distance is farther than distance scale -> tesselation parameters scaled down
    return DISTANCE_SCALE / distance;
}


