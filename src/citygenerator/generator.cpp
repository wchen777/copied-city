#include "generator.h"
#include "../realtime.h"

// TODO: CHANGE THESE
#define CITY_KA 0.5f
#define CITY_KD 0.6f
#define CITY_KS 0.5f

void CopiedCity::GenerateCity() {

    // clear city data
    CopiedCity::city.cityData.clear();

    // global constant data
    CopiedCity::city.globals.ka = CITY_KA;
    CopiedCity::city.globals.kd = CITY_KD;
    CopiedCity::city.globals.ks = CITY_KS;

    // set singular cube trimesh
    CopiedCity::city.cube = new Cube(); // TODO: remember to destroy this!
    CopiedCity::city.cube->UpdateParams(settings.shapeParameter1, settings.shapeParameter2); // set tesselation parameters to build the cube

    // generate the bottom plane
    CopiedCity::city.GeneratePlane();

    // then generate the left and right facades
    CopiedCity::city.GenerateFacades();

    // generate the lights
    CopiedCity::city.GenerateLights();
}



void CopiedCityData::GeneratePlane() {
    CopiedCityData::plane.planeData = CityMeshObject{};
    // initialize CTM and inv transpose CTM
    InitializeSpaceConversions(&CopiedCityData::plane.planeData, &PLANE);

    // initialize material
    InitializeMaterial(&CopiedCityData::plane.planeData, &PLANE);

//    // initialize object tesselation DONT NEED BECAUSE USE SAME CUBE
//    CopiedCityData::plane.planeData.trimesh = new Cube();
//    CopiedCityData::plane.planeData.trimesh->UpdateParams(settings.shapeParameter1, settings.shapeParameter2);

    CopiedCityData::cityData.emplace_back(&CopiedCityData::plane.planeData);

    CopiedCityData::plane.GenerateGroundGrowths();
}
// more stuff goes here about potential details on the bottom of the plane



void CopiedCityData::GenerateFacades() {

    auto backF = CityMeshObject{};

    // initialize CTM and inv transpose CTM
    InitializeSpaceConversions(&backF, &BACKFACADE);

    // initialize material
    InitializeMaterial(&backF, &BACKFACADE);

    // add to back
    CopiedCityData::backFacade.data.emplace_back(backF);

    CopiedCityData::leftFacade.InitShapeGrammar();
    CopiedCityData::leftFacade.SubdividePhaseZAxis();
    CopiedCityData::leftFacade.DecorationPhase(LEFT);
//    CopiedCityData::leftFacade.PerturbationPhase();
    CopiedCityData::leftFacade.SubdividePhaseXAxis();
    CopiedCityData::leftFacade.ConvertShapeGrammar(LEFT);

    CopiedCityData::rightFacade.InitShapeGrammar();
    CopiedCityData::rightFacade.SubdividePhaseZAxis();
    CopiedCityData::rightFacade.DecorationPhase(RIGHT);
//    CopiedCityData::rightFacade.PerturbationPhase();
    CopiedCityData::rightFacade.SubdividePhaseXAxis();
    CopiedCityData::rightFacade.ConvertShapeGrammar(RIGHT);

}


void CopiedCityData::GenerateLights() {
    // add lights
//    SceneLightData directionalLightR = {.id = 0, .type = LightType::LIGHT_DIRECTIONAL, .color = SceneColor(0.50f,0.50f,0.50f,1.f),
//                                       .function = glm::vec3(1.f,0.f,0.f), .pos = glm::vec4(15.f,20.f,-150.f,1.f),
//                                        .dir = glm::normalize(glm::vec4(-0.7f,-1.f, 0.5f, 0.f)),
//                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

//    SceneLightData directionalLightL = {.id = 0, .type = LightType::LIGHT_DIRECTIONAL, .color = SceneColor(0.6f,0.6f,0.6f,1.f),
//                                       .function = glm::vec3(1.f,0.f,0.f), .pos = glm::vec4(15.f,20.f,-150.f,1.f),
//                                        .dir = glm::normalize(glm::vec4(0.7f,-1.f, 0.5f, 0.f)),
//                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};
//    CopiedCityData::lights.emplace_back(directionalLightL);
//    CopiedCityData::lights.emplace_back(directionalLightR);

    // far
    SceneLightData directionalLightFarR = {.id = 0, .type = LightType::LIGHT_DIRECTIONAL, .color = SceneColor(0.5f,0.5f,0.5f,1.f),
                                       .function = glm::vec3(1.f,0.f,0.f), .pos = glm::vec4(50.f,25.f,0.f,1.f),
                                        .dir = glm::normalize(glm::vec4(-0.7f,-1.f, 0.5f, 0.f)),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};


//    SceneLightData directionalLightFarL = {.id = 0, .type = LightType::LIGHT_DIRECTIONAL, .color = SceneColor(0.6f,0.6f,0.6f,1.f),
//                                       .function = glm::vec3(1.f,0.f,0.f), .pos = glm::vec4(15.f,20.f,0.f,1.f),
//                                        .dir = glm::normalize(glm::vec4(0.7f,-1.f, 0.5f, 0.f)),
//                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};
//    CopiedCityData::lights.emplace_back(directionalLightFarL);
    CopiedCityData::lights.emplace_back(directionalLightFarR);

//    SceneLightData rightSideDir = {.id = 0, .type = LightType::LIGHT_DIRECTIONAL, .color = SceneColor(0.45f,0.45f,0.45f,1.f),
//                                       .function = glm::vec3(1.f,0.f,0.f), .pos = glm::vec4(-4.21f, 38.8f, -74.4f, 1.f),
//                                        .dir = glm::normalize(glm::vec4(0.418f, -0.272f, 0.866f, 0.f)),
//                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

//    CopiedCityData::lights.emplace_back(rightSideDir);


    // right side point lights
    SceneLightData point1 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.60f,0.60f, 0.60f,1.f),
                                       .function = glm::vec3(0.5f,0.1f,0.0f), .pos = glm::vec4(-3.334f, 27.02f, 3.13f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

    SceneLightData point2 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.67,0.67f, 0.67f,1.f),
                                       .function = glm::vec3(0.6f,0.22f,0.0f), .pos = glm::vec4(-3.334f, 27.02f, -50.13f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

    SceneLightData point3 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.6f,0.6f, 0.6f,1.f),
                                       .function = glm::vec3(0.5f,0.13f,0.0f), .pos = glm::vec4(10.4f, 30.02f, 0.13f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

    SceneLightData point4 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.7f,0.7f, 0.7f,1.f),
                                       .function = glm::vec3(0.4f,0.1f,0.0f), .pos = glm::vec4(18.4f, 2.f, -125.2f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};


    SceneLightData point5 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.5f,0.5f, 0.5f,1.f),
                                       .function = glm::vec3(0.5f,0.11f,0.0f), .pos = glm::vec4(15.4f, 30.02f, -20.f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

    SceneLightData point6 = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.55f,0.55f, 0.55f,1.f),
                                       .function = glm::vec3(0.4f,0.11f,0.0f), .pos = glm::vec4(-16.4f, 10.02f, -165.f, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};



    CopiedCityData::lights.emplace_back(point1);
    CopiedCityData::lights.emplace_back(point2);
    CopiedCityData::lights.emplace_back(point3);
    CopiedCityData::lights.emplace_back(point4);
    CopiedCityData::lights.emplace_back(point5);
    CopiedCityData::lights.emplace_back(point6);
}




