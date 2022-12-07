#pragma once

#include "utils/rgba.h"
#include <glm/glm.hpp>
#include <algorithm>
#include "utils/scenedata.h"
#include "shadow.h"


inline RGBA toRGBA(const glm::vec3 &illumination) {

    std::uint8_t r = 255 * std::min(std::max(illumination[0], 0.f), 1.f);
    std::uint8_t g = 255 * std::min(std::max(illumination[1], 0.f), 1.f);
    std::uint8_t b = 255 * std::min(std::max(illumination[2], 0.f), 1.f);

    return RGBA{r, g, b, 255};
}

namespace BRDFs {

    inline constexpr float Ambient(float k_a, float Oambient);

    inline constexpr float Lambertian(float k_d, float Odiffuse, float NdotL);

    inline float Specular(float k_s, float Ospecular, float RdotV, float shininess);

}

namespace Lights {

    inline void Directional(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                            float k_d, float k_s, SceneMaterial& material,
                            glm::vec4& surfaceNormal, glm::vec4& dirToCamera, TextureMap* texture, bool enableTexture);

    inline void Point(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                      float k_d, float k_s, SceneMaterial& material,
                      glm::vec4& surfaceNormal, glm::vec4& dirToCamera, TextureMap* texture, bool enableTexture);

    inline void Spot(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                     float k_d, float k_s, SceneMaterial& material,
                     glm::vec4& surfaceNormal, glm::vec4& dirToCamera, TextureMap* texture, bool enableTexture);
}


glm::vec3 Phong(glm::vec4&  position,
           glm::vec4&  normal, // normalized
           glm::vec4&  dirToCamera, // normalized
           SceneGlobalData& global,
           SceneMaterial& material,
           std::vector<SceneLightData>& lights,
           BVH& bvh,
           RayTracer::Config& config,
           std::vector<std::vector<PrimitiveSolver>>& objPrimitives, // for naive object detection
           TextureMap* texture, // for texture mapping
           glm::vec2& uv
           );
