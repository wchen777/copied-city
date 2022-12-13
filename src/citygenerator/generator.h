#pragma once

#include "../trimeshes/trimesh.h"
#include "../utils/scenedata.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "settings.h"
#include "../trimeshes/cube.h"
#include "block.h"

enum FacadeType {
    LEFT,
    RIGHT,
    BACK
};

struct CityPlane {
    CityMeshObject planeData; // the baseline
    std::vector<CityMeshObject> planeProtrusions; // extra details on the ground

    // generate ground cube growths near the buildings
    void GenerateGroundGrowths();
};


struct CityFacade {
    std::vector<CityMeshObject> data; // mesh data

    std::vector<Block> shapeGrammarData; // shape grammar vocabulary

    void InitShapeGrammar(); // initialize with starting block

    void SubdividePhaseZAxis(); // first subdivision phase
    void SubdividePhaseXAxis(); // second subdivision phase
    void PerturbationPhase(); // perturb along x-axis
    void DecorationPhase(FacadeType facadeType); // EXTRA: create cube protrusions from top of structures

    // generate directly to the mesh data, for cube growths
    void GenerateSideRecursiveCubeGrowth(float x, float y, float z, FacadeType side, int depth);

    void VaryHeight(std::vector<Block>& blocks); // go through each block in the given list and modify the height within a certain range

    void ConvertShapeGrammar(FacadeType facadeType); // fill mesh data from shape grammar. must pass in whether or not the facade is left or right
};


struct CopiedCityData {

    CityPlane plane; // bottom plane

    CityFacade leftFacade; // left building
    CityFacade rightFacade; // right building
    CityFacade backFacade; // back building

    std::vector<SceneLightData> lights; // the lights in the scene

    void GenerateLights();
    void GenerateFacades();
    void GeneratePlane();

    void InitializeLights();

    SceneGlobalData globals;

    TrimeshData* cube; // the cube trimesh we can reuse for all objects in the scene

    std::vector<CityMeshObject*> cityData; // all the meshes

    // for future: extend view/generate more data when camera reaches beyond a certain point in the plane
};
