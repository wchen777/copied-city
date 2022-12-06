#include "shadow.h"


glm::vec4 SampleSoftShadowLight(SceneLightData& light) {
    float randWidth = static_cast <float> (rand()) / (static_cast <float> (float(RAND_MAX)/(0.5f * light.width))) - 0.5f*light.width;
    float randHeight = static_cast <float> (rand()) / (static_cast <float> (float(RAND_MAX)/(0.5f * light.height))) - 0.5f*light.height;

    return glm::vec4(light.pos[0] + randWidth, light.pos[1], light.pos[2] + randHeight, 1.f);

}


bool IsLightReachable(SceneLightData& light, glm::vec4& position, BVH &bvh, bool accelerate,
                      std::vector<std::vector<PrimitiveSolver>>& objPrimitives) {

    auto pos = light.type==LightType::LIGHT_AREA ? SampleSoftShadowLight(light) : light.pos;

    // direction to light depends on the light type
    auto dirToLight = light.type == LightType::LIGHT_DIRECTIONAL ? -1.f * glm::normalize(light.dir) : glm::normalize(pos - position);

    auto positionOffset = position + (EPSILON * dirToLight); // avoids self-shadowing

    auto [t, prim, norm] = intersectAllObjects(accelerate, bvh, positionOffset, dirToLight, objPrimitives); // find t value of interseciton in this dir

    // position + t*d = light_pos
    // t = (light_pos - position) / d
    auto lightT = light.type == LightType::LIGHT_DIRECTIONAL ? INFINITY : glm::length(light.pos-position); // t-value of light

    return t >= lightT; // true if there is no intersection with any other object, false if there is an object

}


// FOR TESTING!
void ConstructSoftShadowLight(std::vector<SceneLightData>& lights, SceneLightData& softShadowLight) {

    softShadowLight.type = LightType::LIGHT_AREA;
    softShadowLight.id = lights.size();
    softShadowLight.color = SceneColor(glm::vec4(0.5f));
    softShadowLight.width = SOFTSHADOW_WIDTH;
    softShadowLight.height = SOFTSHADOW_HEIGHT;

    lights.emplace_back(softShadowLight);
}
