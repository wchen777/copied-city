#include "raytracer/bvh.h"
#include "raytracer/primitive.h"
#include <glm/glm.hpp>

#define OFFSET 0.005f

/*
 *  function here to find the smallest t value and surface normal based on a PrimitiveCollection
 *  after applying them all to a eye point and ray direction in world space
 *
 *  if returns an intersection with INFINITY, there was no intersection
*/
std::tuple<float, glm::vec4, PrimitiveSolver*> findIntersection(std::vector<PrimitiveSolver>& prims, glm::vec4& eye, glm::vec4& d);

/*
 * function to find the closest intersection either by using the BVH or by iterating through all objects (depending on accelerate parameter)
 */
std::tuple<float, glm::vec4, PrimitiveSolver*> intersectAllObjects(bool accelerate, BVH& bvh, glm::vec4& position, glm::vec4& dir,
                                                                          std::vector<std::vector<PrimitiveSolver>>& objPrimitives);

/*
 * get a reflected light direction about a surface normal
 */
glm::vec4 reflectLight(glm::vec4& norm, glm::vec4& l);


/*
 * get a refracted light direciton about a surface normal
 */
glm::vec4 refractLight(glm::vec4& norm, glm::vec4& l, float n1, float n2);
