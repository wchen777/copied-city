#include <glm/glm.hpp>
#include "../raytracer/bvh.h"
#include "../raytracer/utils.h"


#define SOFTSHADOW_WIDTH 10.f
#define SOFTSHADOW_HEIGHT 10.f

bool IsLightReachable(SceneLightData& light, glm::vec4&  position, BVH &bvh, bool accelerate, std::vector<std::vector<PrimitiveSolver>>& objPrimitives);

// FOR TESTING
void ConstructSoftShadowLight(std::vector<SceneLightData>& lights, SceneLightData& softShadowLight);
