#include "generator.h"
#include "staticposdata.h"

// PLANE STUFF


const static std::vector<float> LEFT_INTERVALS = {0.f, 2.f, -6.f, -8.f, -14.f, -20.f, -27.f, -41.f, -73.f, -81.f,
                                                  -120.f, -146.f, -155.f, -198.f, -202.f, -232.f, -252.f, -260.f};


const static std::vector<float> RIGHT_INTERVALS = {0.f, 1.f, -4.f, -12.f, -18.f, -29.f, -33.f, -55.f, -69.f, -75.f,
                                                  -102.f, -140.f, -166.f, -174.f, -182.f, -218.f, -224.f, -230.f};


const static std::vector<float> MIDDLE_Z = {0.f, 2.f, 1.f, 1.f, 4.f, 5.f, 6.f,
                                            6.5f, 8.f, 3.f, 4.5f, 3.5, 9.5f, 9.75f, 10.f,
                                            13.f, 15.75f, 14.f, 18.f, 19.f, 18.f,
                                            -2.f, -3.f, -4.f, -4.f, -5.f, -6.f, -7.f,
                                            -7.f, -8.f, -8.f, -10.f, -10.f,
                                            -12.f, -12.f, -14.f, -14.f,
                                             -12.f, -12.f, -14.f, -14.f
                                            -17.f, -17.f, -20.f, -20.f, -23.f,
                                            -26.f, -26.f, -28.f, -28.f,
                                            -26.f, -26.f, -28.f, -28.f,
                                            -30.f, -30.f, -31.f, -35.f,
                                            -44.f, -48.f, -51.f, -52.f,
                                             -44.f, -48.f, -51.f, -52.f,
                                            -54.f, -55.f, -56.f, -59.f, -61.f, -63.f, -63.f, -68.f,
                                            -61.f, -63.f, -63.f, -68.f,
                                            -69.f, -71.f, -73.f, -78.f, -80.f, -88.f, -90.f, -91.f, -91.f,
                                            -80.f, -88.f, -90.f, -91.f, -91.f,
                                            -93.f, -95.f, -95.f, -99.f,-100.f,
                                            -102.f, -108.f, -110.f, -120.f,
                                            -102.f, -111.f, -112.f, -116.f,
                                            -122.f, -127.f, -134.f, -134.f, -146.f,
                                            -122.f, -127.f, -134.f, -134.f, -146.f,
                                            -146.f,-148.f, -154.f, -176.f, -189.f,
                                            -147.f,-149.f, -152.f, -180.f, -188.f,
                                            -192.f, -202.f, -220.f,
                                            -192.f, -202.f, -220.f,-235.f,-249.f, -249.f};

#define SIZE0 5.f
#define SIZE1 6.f
#define SIZE2 8.f
#define SIZE3 9.5f
#define SIZE4 10.f

#define GROUNDSIZE0 1.1f
#define GROUNDSIZE1 0.9f
#define GROUNDSIZE2 1.5f
#define GROUNDSIZE3 1.9f

#define SIDESIZE0 8.5f
#define SIDESIZE1 10.5f
#define SIDESIZE2 11.5f
#define SIDESIZE3 12.5f
#define SIDESIZE4 14.5f

#define MAX_DEPTH 3


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


void GenerateGroundPlate(std::vector<CityMeshObject>& meshData, float x, float z) {
    int randVal = arc4random() % 4;

    float scale = 0.f;

    if (randVal == 0) {
        scale = GROUNDSIZE0;
    } else if (randVal == 1) {
        scale = GROUNDSIZE1;
    } else if (randVal == 2) {
        scale = GROUNDSIZE2;
    } else if (randVal == 3) {
        scale = GROUNDSIZE3;
    }

    float sizeOff = static_cast<float>(arc4random() % 4);

    ObjSceneData cubePlate = {.translation = glm::vec3(x, -4.f + scale/2.f, z),
                                       .scale = glm::vec3(6.f + sizeOff, scale, 6.f + sizeOff),
                                       .diffuse = DIFFUSE_WHITE_1,
                                       .specular = glm::vec3(0.5f, 0.5f, 0.5f),
                                       .ambient = AMBIENT_WHITE_1,
                                       .shininess = 0.f};

    auto cubeObj = CityMeshObject{};

    // initialize CTM and inv transpose CTM
    InitializeSpaceConversions(&cubeObj, &cubePlate);
    // initialize material
    InitializeMaterial(&cubeObj, &cubePlate);

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

    for (auto& zLoc : MIDDLE_Z) {
        float side = 1.f;
        if ((arc4random() % 2) == 0) {
            side *= -1.f;
        }

        float offsetX = ((arc4random() % 18) + 9.f) * side + 1.f;

        GenerateGroundPlate(CityPlane::planeProtrusions, offsetX, zLoc);
    }

}


// FACADE STUFF
void CityFacade::GenerateSideRecursiveCubeGrowth(float x, float y, float z, FacadeType side, int depth) {

    // check max depth reached
    if (depth > MAX_DEPTH) {
        return;
    }

    // get a random size
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

    // multiply the scale by the depth factor
    float depthFactor = randRange(0.5f, 0.75f);
    scale *= std::pow(depthFactor, depth);

    // create new block
    int randDiffuse = arc4random() % 3;
    int randSpec = arc4random() % 2;

    int recursiveSide = side == LEFT ? 1.f : -1.f;
    float xOff = side == LEFT ? LEFT_FACADE_OFFSET+7.5f : RIGHT_FACADE_OFFSET+3.f;

    ObjSceneData blockData = ObjSceneData{
            .translation = glm::vec3(x + xOff + ((-recursiveSide*scale*1.2f)/2.f), -4.f + y, z - Z_SHIFT),
            .scale = glm::vec3(scale, scale, scale),
            // randomly choose shades of color
            .diffuse = randDiffuse == 0 ? DIFFUSE_WHITE_1 : randDiffuse == 1 ? DIFFUSE_WHITE_2 : DIFFUSE_WHITE_3,
            .specular = randSpec == 0 ? SPECULAR_WHITE_1 : SPECULAR_WHITE_2,
            .ambient = AMBIENT_WHITE_1,
            .shininess = 0.f
    };

    auto cubeObj = CityMeshObject{};

    // initialize CTM and inv transpose CTM
    InitializeSpaceConversions(&cubeObj, &blockData);
    // initialize material
    InitializeMaterial(&cubeObj, &blockData);

    CityFacade::data.emplace_back(cubeObj); // add object to the data

    if (arc4random()%2 == 0) {
        // TODO: randomly geenrate these scales for y and z.

        float scaleOff = 5.f * scale/8.f;
        float newY = randRange(y-scaleOff, y+scaleOff);
        float newZ = randRange(z-scaleOff, z+scaleOff);

        CityFacade::GenerateSideRecursiveCubeGrowth(x + (recursiveSide * scale/2.f), newY, newZ, side, depth + 1);
    }
}



