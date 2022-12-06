#include "utils.h"



std::tuple<float, glm::vec4, PrimitiveSolver*> findIntersection(std::vector<PrimitiveSolver>& prims, glm::vec4& eye, glm::vec4& d) {

    float t = INFINITY;
    PrimitiveSolver *best = NULL;

    for (auto& PrimSolver: prims) { // potentially split up threads here to find t values

        float currT = PrimSolver.SolverFunction(eye, d);
        if (currT < t) {
            t = currT;
            best = &PrimSolver;
        }
    }

    if (best != NULL) {
        auto objNorm = best->SurfaceNormal(t, eye, d);
        return std::make_tuple(t, best->InvTransposeCTM * objNorm, best);
    } else {
        return std::make_tuple(t, glm::vec4(0.f), best);
    }


}


std::tuple<float, glm::vec4, PrimitiveSolver*> intersectAllObjects(bool accelerate, BVH& bvh, glm::vec4& position, glm::vec4& dir,
                                                                          std::vector<std::vector<PrimitiveSolver>>& objPrimitives) {

    PrimitiveSolver *bestPrim = NULL;
    float bestT = INFINITY;
    glm::vec4 bestNorm;


    if (accelerate) {
        auto [t, norm, prim] = bvh.BVHIntersect(position, dir);
        bestT = t;
        bestNorm = norm;
        bestPrim = prim;
    } else {
        for (auto &prims : objPrimitives) {

            // loop through all objects in our scene
            auto [t, norm, prim] = findIntersection(prims, position, dir); // prim is a pointer to the intersected primitive

            if (t == INFINITY) { // no intersection with any object with this ray
                continue;
            }

            if (t < bestT) {
                bestT = t;
                bestNorm = norm;
                bestPrim = prim;
            }

        }
    }

    bestNorm[3] = 0.f;
    bestNorm = glm::normalize(bestNorm);

    return std::make_tuple(bestT, bestNorm, bestPrim);
}


glm::vec4 reflectLight(glm::vec4& norm, glm::vec4& l) {
    return glm::normalize(l - (2.f * glm::dot(norm, l) * norm));
}


// snell's law -- for refraction, given an incident light l and surface normal norm
// https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
// vector arithmetic implementation
glm::vec4 refractLight(glm::vec4& norm, glm::vec4& l, float n1, float n2) {

    const float n = n1/n2;

    norm = glm::normalize(norm);
    l = glm::normalize(l);

    const float cosI = -glm::dot(norm, l);

    const float sinT2 = n * n * (1.f - cosI * cosI);

    if (sinT2 > 1.0)  {
//        std::cout << "total internal reflectance" << std::endl;
        return glm::vec4(0.f); // total internal reflectance for transmission
    }

    const float cosT = glm::sqrt(1.f - sinT2);

    return n * l + (n * cosI - cosT) * norm;

//    return glm::refract(l, norm, n1/n2);

}
