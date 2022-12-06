#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

void traverseSceneGraph(SceneNode *node, std::vector<RenderShapeData> &shapes, glm::mat4 &currCTM) {
    // calculate current CTM for this scene node
    glm::mat4 T = glm::mat4(1.f);
    glm::mat4 R = glm::mat4(1.f);
    glm::mat4 S = glm::mat4(1.f);

    glm::mat4 totalCTM = glm::mat4(1.f);

    for (auto transform: node->transformations) {
        switch(transform->type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                T = glm::translate(T, transform->translate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                S = glm::scale(S, transform->scale);
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                R = R * glm::rotate(transform->angle, transform->rotate);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                totalCTM = transform->matrix;
                break;
        }
    }

    totalCTM = currCTM * totalCTM * T * R * S; // apply children first, then parent (parent * T * R * S)

    // this transform is for each primitive, apply to shapes
    for (auto prim : node->primitives) {
        shapes.push_back(RenderShapeData{.primitive = *prim, .ctm = totalCTM});
    }

    // recursive call to children
    for (auto child : node->children) {
        traverseSceneGraph(child, shapes, totalCTM);
    }

}


bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // Populate renderData with global data, lights, and camera data

    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();

    //         Populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    renderData.shapes.clear();
    auto root = fileReader.getRootNode();

    glm::mat4 startingCTM = glm::mat4(1.f);

    traverseSceneGraph(root, renderData.shapes, startingCTM);

    return true;
}

