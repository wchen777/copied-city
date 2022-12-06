#pragma once

#include <glm/glm.hpp>
#include <list> // for optimal remove
#include <array>

#include "primitive.h"

#define HEIGHT_MAX 0.5f
#define HEIGHT_MIN -0.5f
#define EPSILONN 0.0001f


// building BVH bottom-up approach


// TWO TYPES OF BOUNDING VOLUME OBJECT --> bounding volume primitive and bounding volume box
// shares an intersect function


// full BVH construction function (takes in an intersect function for a primitive)

// create a bounding volume object for each primitive, add to bounding volume list
    // pass in a primitive solver object and the intersect function that gets called when passed in a ray and eye
    // save max_x, min_x, max_y, min_y, ... in world space (using inverseCTM in primitive solver)
    // primitive goes into "primitive field"
    // coordinate of the bounding volume is the center of the primitive to start (0, 0),
    // then the average of each of the min and max dimensions

    // for a bounding volume, have an intersect function
    // this function checks if the ray hits the bounding box
    // if it doesn't return infinity, nil, nil
    // if it does, first check if there is a primitive,
    // if it is a primitive volume,
        // intersect with the primitive, return t-value, primitive pointer, and norm
    // if it is a box volume,
        // try both bounding volumes, find the return value for both
        // return the smallest t-value, its primitive pointer, and norm

// creating the BVH:
    // while the list of bounding volumes is > 1,
    // choose a random bounding volume object from the list (or just choose the first one)
    // iterate through the list and find the closest bounding volume to the chosen one
    // remove this bounding volume from the list
    // merge the two bounding volumes, (find new min max for each dimensions, find new center)
    // add new box bounding volume to end of the list

// now we are left with root bounding volume.
// this is the root bounding volume for the BVH.

// traverse BVH function, takes in an eye and d
    // call roots intersect function


/*
 * An axis-aligned bounding box defined by 6 floats in world space.
 */
struct Box {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
};



/*
 * an abstract class represent a bounding volume, either a box or a primitive bounding box
*/
class BoundingVolume {
    public:

        virtual std::tuple<float, glm::vec4, PrimitiveSolver*> FindIntersection(glm::vec4& eye, glm::vec4& d) = 0;

        virtual Box& GetAABoundingBox() = 0;

        virtual glm::vec4& GetWorldCenter() = 0;

        virtual void GetType() = 0;

        virtual ~BoundingVolume() = default;

        /*
         * check if a parallel ray is within a certain dimension
        */
        inline static bool WithinParallel(float d1, float d2, float d1_min, float d1_max, float d2_min, float d2_max) {
            return d1 >= (d1_min - EPSILONN) && d1 <= (d1_max + EPSILONN) && d2 >= (d2_min - EPSILONN) && d2 <= (d2_max + EPSILONN);
        };

        inline static bool CheckIntersection(glm::vec4& eye, glm::vec4& d, Box& box) {

            if (eye[0] <= box.x_max && eye[0] >= box.x_min &&
                    eye[1] <= box.y_max && eye[1] >= box.y_min &&
                    eye[2] <= box.z_max && eye[2] >= box.z_min) {
                return true; // check if the eye point is within the bounding box
            }

            // parallel to x
            if (d[0] == 0.f) {
                if (WithinParallel(d[1], d[2], box.y_min, box.y_max, box.z_min, box.z_max)) {
                    return true;
                }
            } else { // not parallel to x, intersect with both x planes

                float t_min = (box.x_min - eye[0]) / d[0];
                float t_max = (box.x_max - eye[0]) / d[0];

                auto minIntersect = eye + (t_min * d); // where the ray intersects the x_min
                auto maxIntersect = eye + (t_max * d); // where the ray intersects the x_max

                // if the other dimensions of intersection points of either are within bounds, return true, otherwise keep going
                if (WithinParallel(minIntersect[1], minIntersect[2], box.y_min, box.y_max, box.z_min, box.z_max)
                        || WithinParallel(maxIntersect[1], maxIntersect[2], box.y_min, box.y_max, box.z_min, box.z_max)) {
                        return true;
                }
            }

            // parallel to y
            if (d[1] == 0.f) {
                if (WithinParallel(d[0], d[2], box.x_min, box.x_max, box.z_min, box.z_max)) {
                    return true;
                }
            } else { // not parallel to y, find intersection

                float t_min = (box.y_min - eye[1]) / d[1];
                float t_max = (box.y_max - eye[1]) / d[1];

                auto minIntersect = eye + (t_min * d); // where the ray intersects the y_min
                auto maxIntersect = eye + (t_max * d); // where the ray intersects the y_max

                // if the other dimensions of intersection points of either are within bounds, return true, otherwise keep going
                if (WithinParallel(minIntersect[0], minIntersect[2], box.x_min, box.x_max, box.z_min, box.z_max)
                        || WithinParallel(maxIntersect[0], maxIntersect[2], box.x_min, box.x_max, box.z_min, box.z_max)) {
                        return true;
                }
            }

            // parallel to z
            if (d[2] == 0.f) {
                if (WithinParallel(d[0], d[1], box.x_min, box.x_max, box.y_min, box.y_max)) {
                    return true;
                }
            } else { // not parallel to z, find intersection

                float t_min = (box.z_min - eye[2]) / d[2];
                float t_max = (box.z_max - eye[2]) / d[2];

                auto minIntersect = eye + (t_min * d); // where the ray intersects the z_min
                auto maxIntersect = eye + (t_max * d); // where the ray intersects the z_max

                // if the other dimensions of intersection points of either are within bounds, return true, otherwise keep going
                if (WithinParallel(minIntersect[0], minIntersect[1], box.x_min, box.x_max, box.y_min, box.y_max)
                        || WithinParallel(maxIntersect[0], maxIntersect[1], box.x_min, box.x_max, box.y_min, box.y_max)) {
                        return true;
                }
            }

            return false;
        };
};


/*
 * a derived class from bounding volume representing a box that encloses two bounding volumes
*/
class BoundingVolumeBox : public BoundingVolume {
public:
    BoundingVolume* box1;
    BoundingVolume* box2;

    Box BoundingBox;

    glm::vec4 center;

    BoundingVolumeBox(BoundingVolume* box1, BoundingVolume* box2, Box& NewBox) : // take in two initialized pointers of bounding volumes
        box1(box1),
        box2(box2)
    {
        BoundingBox.x_max = NewBox.x_max;
        BoundingBox.y_max = NewBox.y_max;
        BoundingBox.z_max = NewBox.z_max;

        BoundingBox.x_min = NewBox.x_min;
        BoundingBox.y_min = NewBox.y_min;
        BoundingBox.z_min = NewBox.z_min;

        // center is the middle of the new bounding box
        center = glm::vec4((BoundingBox.x_max + BoundingBox.x_min) / 2.f,
                           (BoundingBox.y_max + BoundingBox.y_min) / 2.f,
                           (BoundingBox.z_max + BoundingBox.z_min) / 2.f, 1.f);
    };

    virtual std::tuple<float, glm::vec4, PrimitiveSolver*> FindIntersection(glm::vec4& eye, glm::vec4& d) override {
        PrimitiveSolver* best = NULL;

        if (!CheckIntersection(eye, d, BoundingBox)) {
            return std::make_tuple(INFINITY, glm::vec4(0.f), best);
        }

        auto res1 = box1->FindIntersection(eye, d);
        auto res2 = box2->FindIntersection(eye, d);

        return std::get<0>(res1) < std::get<0>(res2) ? res1 : res2;

    };

    virtual Box& GetAABoundingBox() override {
        return BoundingBox;
    }

    virtual glm::vec4& GetWorldCenter() override {
        return center;
    };

    virtual void GetType() override {
        std::cout << "box" << std::endl;
    }
};


// 8 vertices to represent each vertex of an object space box, set to default
struct BoxVertices {
    std::array<glm::vec4, 8> vertices;
};


/*
 * a derived class from bounding volume representing a primitive w/ its respective bounding box
*/
class BoundingVolumePrimitive : public BoundingVolume {
public:
    Box BoundingBox;

    std::vector<PrimitiveSolver>* Prims;

    glm::vec4 center;

    BoundingVolumePrimitive(std::vector<PrimitiveSolver>* PrimitiveSolvers, RenderShapeData* ShapeData) :
         Prims{PrimitiveSolvers}
    {
        // setup axis aligned bounding box in world space


        BoxVertices boxVertices;

        switch(ShapeData->primitive.type) {
        case PrimitiveType::PRIMITIVE_CONE:
        case PrimitiveType::PRIMITIVE_CYLINDER:
        case PrimitiveType::PRIMITIVE_SPHERE:
        case PrimitiveType::PRIMITIVE_CUBE:
        default: // TODO: maybe something else for the vertices???
            // default initialization here for vertices as 1x1x1 cube centered at 0,0,0
            boxVertices.vertices[0] = glm::vec4(HEIGHT_MIN, HEIGHT_MIN, HEIGHT_MIN, 1.f);
            boxVertices.vertices[1] = glm::vec4(HEIGHT_MIN, HEIGHT_MAX, HEIGHT_MAX, 1.f);
            boxVertices.vertices[2] = glm::vec4(HEIGHT_MIN, HEIGHT_MAX, HEIGHT_MIN, 1.f);
            boxVertices.vertices[3] = glm::vec4(HEIGHT_MIN, HEIGHT_MIN, HEIGHT_MAX, 1.f);
            boxVertices.vertices[4] = glm::vec4(HEIGHT_MAX, HEIGHT_MIN, HEIGHT_MIN, 1.f);
            boxVertices.vertices[5] = glm::vec4(HEIGHT_MAX, HEIGHT_MAX, HEIGHT_MAX, 1.f);
            boxVertices.vertices[6] = glm::vec4(HEIGHT_MAX, HEIGHT_MIN, HEIGHT_MAX, 1.f);
            boxVertices.vertices[7] = glm::vec4(HEIGHT_MAX, HEIGHT_MAX, HEIGHT_MIN, 1.f);
            break;

        }

        BoundingBox = Box{};
        // init all
        BoundingBox.x_max = -INFINITY;
        BoundingBox.y_max = -INFINITY;
        BoundingBox.z_max = -INFINITY;

        BoundingBox.x_min = INFINITY;
        BoundingBox.y_min = INFINITY;
        BoundingBox.z_min = INFINITY;

        // take every vertex to world space. find the maximum and minimum x, y, and z coordinates, use this as the box.
        for (auto& v : boxVertices.vertices) {
            glm::vec4 newPoint = ShapeData->ctm * v;
            BoundingBox.x_max = std::fmax(newPoint[0], BoundingBox.x_max);
            BoundingBox.y_max = std::fmax(newPoint[1], BoundingBox.y_max);
            BoundingBox.z_max = std::fmax(newPoint[2], BoundingBox.z_max);

            BoundingBox.x_min = std::fmin(newPoint[0], BoundingBox.x_min);
            BoundingBox.y_min = std::fmin(newPoint[1], BoundingBox.y_min);
            BoundingBox.z_min = std::fmin(newPoint[2], BoundingBox.z_min);
        }

        // initialize center point in world space
        center = ShapeData->ctm * glm::vec4(0.f, 0.f, 0.f, 1.f);
    };


    virtual std::tuple<float, glm::vec4, PrimitiveSolver*> FindIntersection(glm::vec4& eye, glm::vec4& d) override {
        PrimitiveSolver* best = NULL;

        if (!CheckIntersection(eye, d, BoundingBox)) {
            return std::make_tuple(INFINITY, glm::vec4(0.f), best);
        }


        float t = INFINITY;

        for (auto& PrimSolver: *Prims) { // potentially split up threads here to find t values
            float currT = PrimSolver.SolverFunction(eye, d); // apply solver function to find the t

            if (currT < t) {  // find best t
                t = currT;
                best = &PrimSolver;
            }
        }


        if (best != NULL) {
            auto objNorm = best->SurfaceNormal(t, eye, d);
            return std::make_tuple(t, best->InvTransposeCTM * objNorm, best); // return t, world space normal, the primitive solver
        } else {
            return std::make_tuple(t, glm::vec4(0.f), best);
        }

    };

    virtual Box& GetAABoundingBox() override {
        return BoundingBox;
    }

    virtual glm::vec4& GetWorldCenter() override {
        return center;
    };

    virtual void GetType() override {
        std::cout << "primitive" << std::endl;
    }

};


// A class representing a bounding volume hierarchy tree.
class BVH
{
    public:
        BoundingVolume* root = NULL;

        std::vector<BoundingVolume*> allPrimitives;
        std::vector<std::vector<PrimitiveSolver>*> allSolvers;

        // deallocate all malloc'd memory for BVH
        ~BVH() {
            for (auto& p : allPrimitives) {
                delete p;
            }

            for (auto& s : allSolvers) {
                delete s;
            }
        }

        void ConstructBVH(std::vector<RenderShapeData>* allShapes, std::map<std::string, TextureMap*> filepathToTexture, bool useTexture) {

            if (allShapes->size() == 0) {
                return;
            }

            // construct list of all bounding volume primitives;
            std::list<BoundingVolume*> BVList;
            for (auto& shp : *allShapes) {
                std::vector<PrimitiveSolver>* p = new std::vector<PrimitiveSolver>(); // init primitives collection
                allSolvers.emplace_back(p);
                CompilePrimitives(shp, *p, filepathToTexture, useTexture); // get all the primitives based on type
                BoundingVolume* newPrim = new BoundingVolumePrimitive(p, &shp);
                allPrimitives.emplace_back(newPrim); // for deletion later

                BVList.emplace_back(newPrim);
            }

            BoundingVolume* currBV = BVList.front(); // get first element as basis for comparison
            // if there is only 1 item, this is the only bounding volume

            // while there are BV's in the list, merge them to form a BVH

            while (BVList.size() > 1) {

                BVList.pop_front(); // remove the comparison item

                // find the closest BV.
                float minDistance = INFINITY;
                BoundingVolume* bestBV;
                int indexToRemove = -1;

                int i = 0;
                for (auto& BV : BVList) {
                    // distance between current BV and BV of comparison

                    glm::vec4 c1 = currBV->GetWorldCenter();
                    glm::vec4 c2 = BV->GetWorldCenter();

                    auto d = glm::length(glm::vec3(c1) - glm::vec3(c2));


                    if (d < minDistance) {
                        bestBV = BV;
                        indexToRemove = i;
                    }

                    i++;
                }

                // remove this element from the list
                std::list<BoundingVolume*>::iterator it = BVList.begin();
                std::advance(it, indexToRemove);
                BVList.erase(it);

                // merge the two BVs, bestBV and currBV
                // create a new Box
                Box NewBox;
                auto currBox = currBV->GetAABoundingBox();
                auto bestBox = bestBV->GetAABoundingBox();
                NewBox.x_max = std::fmax(currBox.x_max, bestBox.x_max);
                NewBox.y_max = std::fmax(currBox.y_max, bestBox.y_max);
                NewBox.z_max = std::fmax(currBox.z_max, bestBox.z_max);

                NewBox.x_min = std::fmin(currBox.x_min, bestBox.x_min);
                NewBox.y_min = std::fmin(currBox.y_min, bestBox.y_min);
                NewBox.z_min = std::fmin(currBox.z_min, bestBox.z_min);

                BoundingVolume* newBVMerged = new BoundingVolumeBox(currBV, bestBV, NewBox);
                allPrimitives.emplace_back(newBVMerged); // for deletion later

                BVList.push_back(newBVMerged);

                currBV = BVList.front(); // reset our comparison item, this should always return an item.
            }


            root = currBV;

        };

        std::tuple<float, glm::vec4, PrimitiveSolver*> BVHIntersect(glm::vec4& eye, glm::vec4& d) {
            return root->FindIntersection(eye, d);
        }
};
