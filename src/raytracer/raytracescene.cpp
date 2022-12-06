#include <stdexcept>
#include "raytracescene.h"
#include "utils/sceneparser.h"

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData) :
    r_height(height),
    r_width(width),
    globalData(metaData.globalData),
    lights(metaData.lights),
    shapes(metaData.shapes),
    camera(metaData.cameraData, height, width, 0.1, 100)
{
}

const int& RayTraceScene::width() const {
    return RayTraceScene::r_width;
}

const int& RayTraceScene::height() const {
    return RayTraceScene::r_height;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return RayTraceScene::globalData;
}

const std::vector<SceneLightData>& RayTraceScene::getLights() const {
    return RayTraceScene::lights;
}

const  std::vector<RenderShapeData>& RayTraceScene::getShapes() const {
    return RayTraceScene::shapes;
}

const Camera& RayTraceScene::getCamera() const {
    return RayTraceScene::camera;
}

void RayTraceScene::setCamera(Camera& camera) {
    RayTraceScene::camera = camera;
}
