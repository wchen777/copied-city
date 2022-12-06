#include "raytracer.h"
#include "raytracescene.h"
#include <thread>
#include "../shader/phong.h"
#include "bvh.h"
#include <ctime>
#include "utils.h"


RayTracer::RayTracer(Config config) :
    m_config(config)
{}

const float k = 1.f;

// for super sampling
#define IMG_SPACE_OFFSET_INTERV 0.5f
#define OFFSET_RANGE 2
#define NUM_RAYS 9

// for reflection
#define MAX_RECUR_DEPTH 4


// for testing normals
inline RGBA toRGBATest(glm::vec4 &illumination) {

   illumination[3] = 0.f;
   illumination = glm::normalize(illumination);

    // testing normals
    std::uint8_t r = 255 * std::min((illumination[0] + 1) / 2.f, 1.f);
    std::uint8_t g = 255 * std::min((illumination[1] + 1) / 2.f, 1.f);
    std::uint8_t b = 255 * std::min((illumination[2] + 1) / 2.f, 1.f);

    return RGBA{r, g, b};
}

inline RGBA toRGBAFloat(float rAcc, float gAcc, float bAcc) {
    std::uint8_t r = (std::uint8_t) std::min(rAcc, 255.f);
    std::uint8_t g = (std::uint8_t) std::min(gAcc, 255.f);
    std::uint8_t b = (std::uint8_t) std::min(bAcc, 255.f);

    return RGBA{r, g, b, 255};
}


std::tuple<float, float> normalizedImageSpaceCoords(int i, int j, int width, int height, float offsetX, float offsetY) {
    return std::tuple<float, float>(((j + 0.5f + offsetX) / width - 0.5f), ((height - 1 - i + 0.5f + offsetY) / height - 0.5f));
}

std::tuple<float, float> viewPlaneCoords(std::tuple<float, float>& xy, float viewAngleH, float aspectRatio) {
    auto V = 2 * k * glm::tan(viewAngleH / 2.f);

    return std::tuple<float, float>(V * aspectRatio * std::get<0>(xy), V * std::get<1>(xy));
}


/*
 * Given a starting point and incoming light direction, refract the light about the intersection point
 * calculate contribution to color from refraction
 *
*/
void refractionRay(BVH& bvh, glm::vec4& incomingL, glm::vec4& p_intersect,
                        glm::vec4& norm, SceneGlobalData& global, std::vector<SceneLightData>& lights,
                        SceneMaterial& prevMaterial, int depth, glm::vec3& illumination, RayTracer::Config& config,
                   std::vector<std::vector<PrimitiveSolver>>& objPrimitives, float incidentIOR, float nextIOR){

    // max recursive depth
    if (depth == MAX_RECUR_DEPTH) {
        return;
    }

    // if zero transparent component
    if (prevMaterial.cTransparent[0] == 0.f && prevMaterial.cTransparent[1] == 0.f && prevMaterial.cTransparent[2] == 0.f) {
        return;
    }

    // find refracted path of light using snell's law
    auto refractDir = refractLight(norm, incomingL, incidentIOR, nextIOR);

    if (refractDir[0] == 0.f && refractDir[1] == 0.f && refractDir[2] == 0.f) { // total internal reflectance
        return;
    }

    // find new intersection
    auto p_intersectOff = (p_intersect + (OFFSET * refractDir)); // prevent self intersection
    auto [t, intersectNorm, prim] = intersectAllObjects(config.enableAcceleration, bvh, p_intersectOff, refractDir, objPrimitives);


    if (t == INFINITY) { // refraction ray does not intersect with anything
        return;
    }

    auto p_intersectNew = (p_intersectOff) + (t * refractDir); // where this ray hits on the intersected surface
    auto refractDirOpp = -1.f * refractDir; // this is the direction from the reflection point to the originating ray

    glm::vec2 uv;
    if (prim->MaterialData.textureMap.isUsed) { // get uv point of texture map if it is enabled
        uv = prim->UVMappingFunction(p_intersectNew);
    }

    // see where this new reflected ray hits, add illumination terms
    glm::vec3 refrColor = Phong(p_intersectNew, intersectNorm, refractDirOpp, global, prim->MaterialData,
                                lights, bvh, config, objPrimitives, prim->Texture, uv);

    illumination[0] += refrColor[0] * prevMaterial.cTransparent[0] * global.kt;
    illumination[1] += refrColor[1] * prevMaterial.cTransparent[1] * global.kt;
    illumination[2] += refrColor[2] * prevMaterial.cTransparent[2] * global.kt;

    // check if the refracted direction's dot product with the intersected normal is negative. if it is when we need to flip it
    auto newRefractNorm = intersectNorm;
    auto nextIor = prevMaterial.ior;
    if (glm::dot(refractDir, intersectNorm) > 0) { // this also means we have exited the surface
         newRefractNorm = -1.f * intersectNorm; // flip the normal as "inside" the surface
         nextIor = 1.f; // set the next index of refraction to be that of air transmission
    }

    return refractionRay(bvh, refractDir, p_intersectNew, newRefractNorm, global, lights,
                         prim->MaterialData, depth + 1, illumination, config, objPrimitives, prevMaterial.ior, nextIor);

}



/*
 * Given a starting point and a direction (a ray), find the new intersection point (if any) and calculate the light contribution
 * Calculate reflection direction to intersect and check material properties
 *
*/
void reflectionRay(BVH& bvh, glm::vec4& incomingL, glm::vec4& p_intersect,
                        glm::vec4& norm, SceneGlobalData& global, std::vector<SceneLightData>& lights,
                        SceneMaterial& prevMaterial, int depth, glm::vec3& illumination, RayTracer::Config& config,
                        std::vector<std::vector<PrimitiveSolver>>& objPrimitives) {

    // max recursive depth
    if (depth == MAX_RECUR_DEPTH) {
        return;
    }

    // if non-zero reflective component
    if (prevMaterial.cReflective[0] == 0.f && prevMaterial.cReflective[1] == 0.f && prevMaterial.cReflective[2] == 0.f) {
        return;
    }

    // find reflected path of light
    auto reflectDir = reflectLight(norm, incomingL);

    // find new intersection
    auto p_intersectOff = (p_intersect + (OFFSET * reflectDir)); // prevent self intersection
    auto [t, intersectNorm, prim] = intersectAllObjects(config.enableAcceleration, bvh, p_intersectOff, reflectDir, objPrimitives);


    if (t == INFINITY) { // reflection ray does not intersect with anything
        return;
    }


    auto p_intersectNew = (p_intersectOff) + (t * reflectDir); // where this ray hits on the intersected surface
    auto reflectDirOpp = -1.f * reflectDir; // this is the direction from the reflection point to the originating ray

    glm::vec2 uv;
    if (prim->MaterialData.textureMap.isUsed) { // get uv point of texture map if it is enabled
        uv = prim->UVMappingFunction(p_intersectNew);
    }

    // see where this new reflected ray hits, add illumination terms
    glm::vec3 reflColor = Phong(p_intersectNew, intersectNorm, reflectDirOpp, global, prim->MaterialData,
                                lights, bvh, config, objPrimitives, prim->Texture, uv);

    // compound the ks factor for diminishingly strong reflections
    illumination[0] += reflColor[0] * prevMaterial.cReflective[0] * std::pow(global.ks, depth + 1);
    illumination[1] += reflColor[1] * prevMaterial.cReflective[1] * std::pow(global.ks, depth + 1);
    illumination[2] += reflColor[2] * prevMaterial.cReflective[2] * std::pow(global.ks, depth + 1);

    return reflectionRay(bvh, reflectDir, p_intersectNew, intersectNorm, global, lights,
                         prim->MaterialData, depth + 1, illumination, config, objPrimitives);
}


/*
 * helper function to shoot a ray and return a pixel
 */
inline RGBA shootRay(int i, int j, int height, int width, float angleH, float aspectRatio,
              glm::mat4& invV, std::vector<std::vector<PrimitiveSolver>>& objPrimitives,
              std::vector<SceneLightData>& lights, glm::vec4& p_eye, SceneGlobalData& global, BVH& bvh,
               float offsetX, float offsetY, RayTracer::Config& config) {
    // separate all this into new inline function that gets run for super sampling

    // converting pixel indices to view plane coords
    auto normImgSpcCoords = normalizedImageSpaceCoords(i, j, width, height, offsetX, offsetY);
    auto vpCoords = viewPlaneCoords(normImgSpcCoords, angleH, aspectRatio);

    // expression for the ray: p_eye + t * d
    // where d = norm((V^-1 * p_pixel) - p_eye)
    // p_pixel in camera space, p_eye in world space

    // d = uvk - eye, in camera space
    auto d_cam = glm::vec4(std::get<0>(vpCoords), std::get<1>(vpCoords), -1 * k, 0);

    glm::vec4 d = invV * d_cam; // ray direction (world space)
    d[3] = 0.f; // change to vector

    auto dirToCam = -1.f * glm::normalize(d); // direction to camera is direction to point but inverted

    auto [bestT, bestNorm, bestPrim] = intersectAllObjects(config.enableAcceleration, bvh, p_eye, d, objPrimitives);

    if (bestT == INFINITY || bestPrim == NULL) {
        return RGBA{0, 0, 0, 0}; // return 0 a value if we don't find an intersection, this is to speed up not sending 9 rays on no intersection
    }

    // TESTING, make the point on the image the RGB of the surface normal
//                imageData[i * width + j] = toRGBATest(bestNorm);

    /*
     * - with that t value,
     * - light the sample
     * - use phong illumination model at that position
     * - generate secondary rays from that point
    */

    glm::vec4 position = p_eye + (bestT * d); // world space position of intersection
    bestNorm[3] = 0.f; // to vector
    bestNorm = glm::normalize(bestNorm);

    glm::vec2 uv;
    if (bestPrim->MaterialData.textureMap.isUsed) { // get uv point of texture map if it is enabled
        uv = bestPrim->UVMappingFunction(position);
    }

    glm::vec3 primaryColor = Phong(position, bestNorm, dirToCam, global, bestPrim->MaterialData,
                                   lights, bvh, config, objPrimitives, bestPrim->Texture, uv);


    glm::vec3 reflectionColor(0.f);
    if (config.enableReflection) {
         reflectionRay(bvh, d, position, bestNorm, global, lights, bestPrim->MaterialData, 0, reflectionColor, config, objPrimitives);
    }

    glm::vec3 refractionColor(0.f);
    if (config.enableRefraction) {
        refractionRay(bvh, d, position, bestNorm, global, lights, bestPrim->MaterialData, 0, refractionColor, config, objPrimitives, 1.f, bestPrim->MaterialData.ior);
    }

    auto totalColor = primaryColor + reflectionColor + refractionColor;

    return toRGBA(totalColor);

}


// for each (u,v) on the image plane:

/*
 * - shoot 1 or more rays (for super-sampling) from that u, v
 * - loop over objects (for now, implement BVH later)
 * - calculate that object's intersection with the ray
 * - save lowest t value of intersection
*/
void traceRaySection(RGBA *imageData, int i_start, int i_end,
                                int height, int width, float angleH, float aspectRatio,
                                glm::mat4& invV, std::vector<std::vector<PrimitiveSolver>>& objPrimitives,
                                std::vector<SceneLightData>& lights, glm::vec4& p_eye, SceneGlobalData& global,
                                BVH& bvh, RayTracer::Config& config) {

    for (int i = i_start; i < i_end; ++i) {
        for (int j = 0; j < width; ++j) {

            if (config.enableSuperSample) {

                std::uint32_t rAvg = 0;
                std::uint32_t gAvg = 0;
                std::uint32_t bAvg = 0;

                for (int ii = -1; ii < OFFSET_RANGE; ++ii) {
                    for (int jj = -1; jj < OFFSET_RANGE; ++jj) {
                        auto pixRes = shootRay(i, j, height, width, angleH,
                                               aspectRatio, invV, objPrimitives, lights, p_eye, global, bvh,
                                               ii*IMG_SPACE_OFFSET_INTERV, jj*IMG_SPACE_OFFSET_INTERV, config);
                        if (pixRes.a == 0) {
                            goto END_LOOP;
                        }

                        rAvg += pixRes.r;
                        gAvg += pixRes.g;
                        bAvg += pixRes.b;
                    }
                }
                rAvg = rAvg / NUM_RAYS;
                gAvg = gAvg / NUM_RAYS;
                bAvg = bAvg / NUM_RAYS;

                std::uint8_t rFinal = std::min((int) rAvg, 255);
                std::uint8_t gFinal = std::min((int) gAvg, 255);
                std::uint8_t bFinal = std::min((int) bAvg, 255);

                imageData[i * width + j] = RGBA{rFinal, gFinal, bFinal};
            } else {
                // just shoot one ray
                imageData[i * width + j] =
                        shootRay(i, j, height, width, angleH,
                                 aspectRatio, invV, objPrimitives, lights, p_eye, global, bvh, 0.f, 0.f , config);

            }

            END_LOOP:;
        }
    }


}


void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
    // Note that we're passing `data` as a pointer (to its first element)
    // Recall from Lab 1 that you can access its elements like this: `data[i]`


    // generate an image plane in world space from the camera's intrinsics and extrinsics
    auto cam = scene.getCamera();
    auto lights = scene.getLights();
    auto objs = scene.getShapes();

    auto width = scene.width();
    auto height = scene.height();

    auto angleH = cam.getHeightAngle();

    auto invV = cam.getInvViewMatrix();
    auto aspectRatio = cam.getAspectRatio();

    auto global = scene.getGlobalData();

    // ------------------ INITIALIZATION --------------------

    // vector of primitive solver collections
    std::vector<std::vector<PrimitiveSolver>> objPrimitives(objs.size()); // will be empty is acceleration not enable (CHECK THAT IT"S NOT USED ELSEWHERE)

    // map of all filename to texture map objects
    std::map<std::string, TextureMap*> filepathToTexture = std::map<std::string,TextureMap*>(); // for texture mapping

//    // soft shadow initialiaation for testing
//    SceneLightData softShadowLight;
//    ConstructSoftShadowLight(lights, softShadowLight);

    BVH bvh;

    if (RayTracer::m_config.enableAcceleration) {
        // CONSTRUCT BVH
        bvh = BVH();
        bvh.ConstructBVH(&objs, filepathToTexture, RayTracer::m_config.enableTextureMap);
        std::cout << "done constructing bvh" << std::endl;
    } else {
        int count = 0;
        for (const auto& obj : objs) {
            std::vector<PrimitiveSolver> p; // init primitives collection
            CompilePrimitives(obj, p, filepathToTexture, RayTracer::m_config.enableTextureMap); // get all the primitives based on type
            objPrimitives[count] = p; // add to our primitives
            count++;
        }
    }

    // -----------------------------------------------------------

    clock_t now = clock();

    glm::vec4 p_eye(cam.pos[0], cam.pos[1],  cam.pos[2], 1.f); // eye point (world space)

    // multithreaded implementation by splitting up image by rows to be ray-traced.
    int numSplits = RayTracer::m_config.enableParallelism ? NUM_THREADS : 1; // TODO: change this to 1 when not active
    // NOTE: numSplits will be NUM_THREADS + 1 when not evenly divisible

    RayTracer::Config config(RayTracer::m_config);

    std::vector<std::thread> threads;
    for (int t = 0; t < height; t+=(height/numSplits)) {

        // calculate
        int start = t; // get start
        int end = std::min(t + height/numSplits, height - 1); // get next chunk or what is left over

        // start the thread
        threads.emplace_back(traceRaySection, imageData, start, end, height, width, angleH, aspectRatio,
                             std::ref(invV), std::ref(objPrimitives), std::ref(lights),
                             std::ref(p_eye), std::ref(global), std::ref(bvh), std::ref(config));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto req = clock() - now;
    std::cout << "time: " << (float)req/CLOCKS_PER_SEC << std::endl; // this is not entirely accurate as sec especially in debug mode

    // cleanup texture map mapping
    for (auto& fToText : filepathToTexture) {
        delete fToText.second;
    }

}
