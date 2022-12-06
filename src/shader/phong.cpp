#include "phong.h"
#include <iostream>
#include <ostream>


// bidirecitonal reflectance distribution functions
namespace BRDFs {

    inline constexpr float Ambient(float k_a, float Oambient) {
        return k_a * Oambient;
    }

    inline constexpr float Lambertian(float k_d, float Odiffuse, float NdotL) {
        return k_d * Odiffuse * NdotL;
    }

    inline constexpr float Specular(float k_s, float Ospecular, float RdotV, float shininess) {
        return k_s * Ospecular * std::pow(RdotV, shininess);
    }

}

// falloff factor for spot lights
inline constexpr float falloff(float angle, float innerA, float outerA) {
    auto t = (angle - innerA)/(outerA - innerA);
    return -2 * std::pow(t, 3) + 3 * std::pow(t, 2);
}

inline float diffuseWithTexture(float k_d, float NdotL, SceneMaterial& material, TextureMap* texture,
                                int channel, glm::vec3& textureColor, bool enableTexture) {
    if (!material.textureMap.isUsed && enableTexture) {
        return BRDFs::Lambertian(k_d, material.cDiffuse[channel], NdotL);
    } else {
        auto B = textureColor[channel] * NdotL;
        auto A = BRDFs::Lambertian(k_d, material.cDiffuse[channel], NdotL);

        return A + (material.blend * (B - A));
    }
}


// helper for both specular and diffuse
inline void phongSpecDiffuse(glm::vec3& illumination, SceneLightData &light, glm::vec4& L,
                             glm::vec4& dirToCamera, glm::vec4& surfaceNormal, float k_d, float k_s,
                             SceneMaterial& material, float Fatt, float falloffScale, TextureMap* texture,
                             glm::vec4& position, glm::vec2& uv, bool enableTexture) {

    // for diffuse, surface normal dot direction to light
    auto NdotL = std::max(glm::dot(surfaceNormal, L), 0.f);

    float Racc = 0.f;
    float Gacc = 0.f;
    float Bacc = 0.f;

    // ----- DIFFUSE -----

    glm::vec3 textureColor;
    if (material.textureMap.isUsed && enableTexture) {
//        textureColor = texture->NearestNeighborSampling(uv[0], uv[1], material.textureMap.repeatU, material.textureMap.repeatV);
        textureColor = texture->BilinearSampling(uv[0], uv[1], material.textureMap.repeatU, material.textureMap.repeatV);
    }

    Racc += diffuseWithTexture(k_d, NdotL, material, texture, 0, textureColor, enableTexture);
    Gacc += diffuseWithTexture(k_d, NdotL, material, texture, 1, textureColor, enableTexture);
    Bacc += diffuseWithTexture(k_d, NdotL, material, texture, 2, textureColor, enableTexture);


    // ----- SPECULAR ------
    auto R = (2 * NdotL * surfaceNormal) - L;
    auto RdotV = std::max(glm::dot(R, dirToCamera), 0.f);

    Racc += BRDFs::Specular(k_s, material.cSpecular[0], RdotV, material.shininess);
    Gacc += BRDFs::Specular(k_s, material.cSpecular[1], RdotV, material.shininess);
    Bacc += BRDFs::Specular(k_s, material.cSpecular[2], RdotV, material.shininess);

    // multiply all by the attentuation and falloff if applicable
    Racc *= light.color[0] * Fatt * falloffScale;
    Gacc *= light.color[1] * Fatt * falloffScale;
    Bacc *= light.color[2] * Fatt * falloffScale;

    illumination[0] += Racc;
    illumination[1] += Gacc;
    illumination[2] += Bacc;
}

namespace Lights {

    // direcitonal lights
    inline void Directional(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                            float k_d, float k_s, SceneMaterial& material,
                            glm::vec4& surfaceNormal, glm::vec4& dirToCamera, TextureMap* texture, glm::vec2& uv, bool enableTexture) {

        auto L = glm::normalize(-1.f * light.dir); // for directional lights

        // mutates illumination with the specular and diffuse terms
        phongSpecDiffuse(illumination, light, L, dirToCamera, surfaceNormal, k_d, k_s, material, 1.f, 1.f, texture, position, uv, enableTexture);
    }

    // point lights
    inline void Point(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                      float k_d, float k_s, SceneMaterial& material,
                      glm::vec4& surfaceNormal, glm::vec4& dirToCamera, TextureMap* texture, glm::vec2& uv, bool enableTexture) {


        // attentuation for point lights
        auto d = glm::length(position - light.pos);
        float Fatt = std::min(1 / (light.function[0] + (d * light.function[1]) + (d * d * light.function[2])), 1.f);

        auto L = glm::normalize(light.pos - position); // for point lights

        // mutates illumination with the specular and diffuse terms
        phongSpecDiffuse(illumination, light, L, dirToCamera, surfaceNormal, k_d, k_s, material, Fatt, 1.f, texture, position, uv, enableTexture);

    }


    // spot lights
    inline void Spot(glm::vec3& illumination, glm::vec4& position, SceneLightData &light,
                     float k_d, float k_s, SceneMaterial& material,
                     glm::vec4& surfaceNormal, glm::vec4& dirToCamera,
                     TextureMap* texture, glm::vec2& uv, bool enableTexture) {

        // attentuation
        auto d = glm::length(position - light.pos);
        float Fatt = std::min(1 / (light.function[0] + (d * light.function[1]) + (d * d * light.function[2])), 1.f);

        auto L = glm::normalize(light.pos - position);

        // calculate falloff factor

        // x is the angle between current direction and spotlight direction
        auto x = std::acos(glm::dot(L, glm::normalize(-1.f * light.dir))); // TODO: normalize this every time????
        auto inner = light.angle - light.penumbra; // inner angle is the total angle - the penumbra

        float falloffScale = x <= inner ? 1.f :
                                (x > light.angle) ? 0.f :
                                    (1.f - falloff(x, inner, light.angle));

        phongSpecDiffuse(illumination, light, L, dirToCamera, surfaceNormal, k_d, k_s, material, Fatt, falloffScale, texture, position, uv, enableTexture);

    }
}


/*
 * general phong illumination model function. incorporates texture mapping.
 */
glm::vec3 Phong(glm::vec4&  position,
           glm::vec4&  normal, // normalized
           glm::vec4&  dirToCamera, // normalized
           SceneGlobalData& global,
           SceneMaterial& material,
           std::vector<SceneLightData>& lights,
           BVH& bvh, // for checking shadows
           RayTracer::Config& config,
           std::vector<std::vector<PrimitiveSolver>>& objPrimitives, // for naive object detection
           TextureMap* texture,
           glm::vec2& uv
           ) {

     glm::vec3 illumination(0.f, 0.f, 0.f);

     illumination[0] += BRDFs::Ambient(global.ka, material.cAmbient[0]);
     illumination[1] += BRDFs::Ambient(global.ka, material.cAmbient[1]);
     illumination[2] += BRDFs::Ambient(global.ka, material.cAmbient[2]);

     for (SceneLightData &light : lights) {

//          check intersection direction to the light for shadows
         if (config.enableShadow && !IsLightReachable(light, position, bvh, config.enableAcceleration, objPrimitives)) {
             continue;
         }

         switch (light.type) { // ONLY SUPPORTS DIRECTIONAL NOW
            case LightType::LIGHT_DIRECTIONAL:
                // modifies illumination vector
                Lights::Directional(illumination, position, light, global.kd, global.ks, material, normal, dirToCamera, texture, uv, config.enableTextureMap);
                break;
            case LightType::LIGHT_POINT:
                // modifies illumination vector
                Lights::Point(illumination, position, light, global.kd, global.ks, material, normal, dirToCamera, texture, uv, config.enableTextureMap);
                break;
            case LightType::LIGHT_SPOT:
                // modifies illumination vector
                Lights::Spot(illumination, position, light, global.kd, global.ks, material, normal, dirToCamera, texture, uv, config.enableTextureMap);
                break;
             default:
                break;
         }

     }

     return illumination;

}
