#pragma once

#include "../trimeshes/trimesh.h"
#include "../utils/scenedata.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "settings.h"
#include "../trimeshes/cube.h"



struct CityMeshObject {
    // vertex/trimesh data
    TrimeshData* trimesh; // UNUSED RIGHT NOW

    SceneMaterial material;

    // for world space conversions
    glm::mat4 modelMatrix;
    glm::mat4 invTransposeModelMatrix;

    // for openGL bindings
    GLuint vbo;
    GLuint vao;
};


struct CityPlane {
    CityMeshObject planeData; // the baseline
    std::vector<CityMeshObject> planeProtrusions; // extra details on the ground
};


struct CityBoundaryBox {
    CityMeshObject top;
    CityMeshObject left;
    CityMeshObject right;
    CityMeshObject front;
};

struct CityFacade {
    std::vector<CityMeshObject> data;
};


struct CopiedCityData {
    CityBoundaryBox boundaryBox; // white bounding box to create a white environment

    CityPlane plane; // bottom plane

    CityFacade leftFacade; // left building
    CityFacade rightFacade; // right building

    std::vector<SceneLightData> lights; // the lights in the scene

    void GenerateBoundaryBox();
    void GenerateFacades();
    void GeneratePlane();

    void InitializeLights();

    SceneGlobalData globals;

    TrimeshData* cube; // the cube trimesh we can reuse for all objects in the scene

    std::vector<CityMeshObject*> cityData; // all the meshes

    // for future: extend view/generate more data when camera reaches beyond a certain point in the plane
};
