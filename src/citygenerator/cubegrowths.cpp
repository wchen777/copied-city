#include "generator.h"

const static std::vector<float> LEFT_INTERVALS = {-8.f, -14.f, -20.f, -27.f, -41.f, -73.f, -81.f,
                                                  -120.f, -146.f, -155.f, -198.f, -202.f, -232.f, -260.f};


const static std::vector<float> RIGHT_INTERVALS = {-4.f, -12.f, -18.f, -29.f, -33.f, -55.f, -69.f, -75.f,
                                                  -102.f, -140.f, -166.f, -174.f, -182.f, -218.f, -224.f, -230.f};

#define SIZE0 5.f
#define SIZE1 6.f
#define SIZE2 8.f
#define SIZE3 9.5f
#define SIZE4 10.f

void GenerateCubeGrowth(std::vector<CityMeshObject>& meshData, float x, float z, FacadeType side) {
    // random initial scale size
    int randVal = arc4random() % 5;

    float scale = 0.f;

    if (randVal == 0) {
        scale = SIZE0;
    } else if (randVal == 1) {
        scale = SIZE1;
    } else if (randVal == 2) {
        scale = SIZE2;
    } else if (randVal == 3) {
        scale = SIZE3;
    } else if (randVal == 4) {
        scale = SIZE4;
    }

    ObjSceneData cubeGrowth = {.translation = glm::vec3(x, -4.f + scale/2.f, z),
                                       .scale = glm::vec3(scale, scale, scale),
                                       .diffuse = DIFFUSE_WHITE_1,
                                       .specular = glm::vec3(0.5f, 0.5f, 0.5f),
                                       .ambient = AMBIENT_WHITE_1,
                                       .shininess = 0.f};

    auto cubeObj = CityMeshObject{};

    // initialize CTM and inv transpose CTM
    InitializeSpaceConversions(&cubeObj, &cubeGrowth);
    // initialize material
    InitializeMaterial(&cubeObj, &cubeGrowth);

    meshData.emplace_back(cubeObj);
}

void CityPlane::GenerateGroundGrowths() {

    // at hard coded intervals/locations, generate random cube growths
    for (auto& zLoc : LEFT_INTERVALS) {
        float xPerturb = ((arc4random() % 11) - 7) * 0.2f;
        GenerateCubeGrowth(CityPlane::planeProtrusions, -20.f + xPerturb, zLoc, LEFT);

        // 1/2 of generating another
        if ((arc4random() % 2) == 0) {
            float zPerturb = ((arc4random() % 11) - 5) * 0.8f;
            GenerateCubeGrowth(CityPlane::planeProtrusions, -20.f + abs(xPerturb*2), zLoc + zPerturb, LEFT);
        }
    }

    for (auto& zLoc : RIGHT_INTERVALS) {
        float xPerturb = ((arc4random() % 11) - 7) * 0.2f;
        GenerateCubeGrowth(CityPlane::planeProtrusions, 28.f + xPerturb, zLoc, RIGHT);

        // 1/2 of generating another
        if ((arc4random() % 2) == 0) {
            float zPerturb = ((arc4random() % 11) - 5) * 0.8f;
            GenerateCubeGrowth(CityPlane::planeProtrusions, 28.f + -abs(xPerturb*2), zLoc + zPerturb, RIGHT);
        }
    }

}
