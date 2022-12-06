#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <ostream>
#include <tuple>
#include <functional>

#define RADIUS_SQ 0.25f
#define HEIGHT_MAX 0.5f
#define HEIGHT_MIN -0.5f
#define EPSILON 0.001f // gives a range of error for intersections

enum FlatSide {
    TOP_Y,
    BOTTOM_Y,
    TOP_X,
    BOTTOM_X,
    TOP_Z,
    BOTTOM_Z
};

// function defs -- these point and directions are in object space
using ImplicitSignature = auto(glm::vec4&& EyePoint, glm::vec4&& RayDirection)->float;
using ObjectNormalSignature = auto(float t, glm::vec4&& EyePoint, glm::vec4&& RayDirection)->glm::vec4;
using ObjectUVMappingSignature = auto(glm::vec4&& IntersectionPoint)->glm::vec2;

// different methods for finding the surface normals of implicit surface equations (world space)
namespace Normals {

    // sphere's normal is just <2x, 2y, 2z> -> normalized
    constexpr auto Sphere() {
        return [](float t, glm::vec4&& eyePoint, glm::vec4&& rayDirection) {
            auto xyz = eyePoint + rayDirection * t;
            xyz[3] = 0.f;
            return glm::normalize(xyz);
        };

    }

    // cone body normal is <2x, ((0.5 - y) / 2), 2z> -> <x, 0.25(0.5 - y), z>
    constexpr auto Cone() {

        return [](float t, glm::vec4&& eyePoint, glm::vec4&& rayDirection) {
            auto xyz = eyePoint + rayDirection * t;
            xyz[3] = 0.f; // change to vector
            // change the y
            xyz[1] = 0.5 * std::sqrt(std::pow(xyz[0], 2) + std::pow(xyz[2], 2));
            return glm::normalize(xyz);
        };

    }

    // cylinder body normal is
    constexpr auto Cylinder() {
        return [](float t, glm::vec4&& eyePoint, glm::vec4&& rayDirection) {
            auto xyz = eyePoint + rayDirection * t;
            xyz[3] = 0.f; // change to vector
            // change the y
            xyz[1] = 0.f;
            return glm::normalize(xyz);
        };
    }


    constexpr auto Flat(FlatSide Side) {

        return [=](float t, glm::vec4&& eyePoint, glm::vec4&& rayDirection) {
            glm::vec4 norm(0.f, 0.f, 0.f, 0.f);
            switch (Side) {
            case TOP_X:
                norm[0] = 1.f;
                break;
            case BOTTOM_X:
                norm[0] = -1.f;
                break;
            case TOP_Y:
                norm[1] = 1.f;
                break;
            case BOTTOM_Y:
                norm[1] = -1.f;
                break;
            case TOP_Z:
                norm[2] = 1.f;
                break;;
            case BOTTOM_Z:
                norm[2] = -1.f;
                break;
            }
            return glm::normalize(norm);
        };

    }

}


// Different methods for finding the roots of implicit surface equations.
// For the surfaces you'll deal with in your ray tracer, the quadratic formula is sufficient.
namespace Solvers {
    // The quadratic formula solver takes two functions as arguments:
    // - CoefficientGenerator: returns the quadratic coefficients a, b, c for a given ray
    // - Constraint: arbitrary function that tells us whether an intersection point is valid or not (e.g. cylinders of bounded height)
    auto Quadratic(auto&& CoefficientGenerator, auto&& Constraint) {
        return [=](glm::vec4&& EyePoint, glm::vec4&& RayDirection) {
            auto [a, b, c] = CoefficientGenerator(EyePoint, RayDirection);

            float discr = (b * b) - (4 * a * c);

            if (discr < 0) {
                return INFINITY; // no real solutions
            }

            discr = std::sqrt(discr); // modify this term

            float denom = 2 * a;

            if (denom == 0.f) {
                return INFINITY; // check divide by -
            }

            // find non-negative sol
            float r1 = ((-1 * b) - discr) / denom; // - sol
            float r2 = ((-1 * b) + discr) / denom; // + sol

            if (r1 < 0.f) r1 = INFINITY;
            if (r2 < 0.f) r2 = INFINITY;

            if (r1 == INFINITY && r2 == INFINITY) {
                return INFINITY; // no roots available
            }

            float RootMin = r1 < r2 ? r1 : r2; // choose the lower t that is non-negative
            float RootMax = r1 < r2 ? r2 : r1; // the "greater" root that could still be valid.

            glm::vec4 IntersectionPosition = EyePoint + (RootMin * RayDirection); // find intersect point

            if (Constraint(IntersectionPosition[0], IntersectionPosition[1], IntersectionPosition[2])) {
                 return RootMin;
            } else {
                // check the other root
                glm::vec4 OtherIntersectionPosition = EyePoint + (RootMax * RayDirection);

                // if it is valid, return it
                if (Constraint(OtherIntersectionPosition[0], OtherIntersectionPosition[1], OtherIntersectionPosition[2])) {
                    return RootMax;
                }

                return INFINITY; // neither root follows constraint, no intersection
            }

        };
    }

    auto Flat(FlatSide Flat, auto&& Constraint) {

        return [=](glm::vec4&& EyePoint, glm::vec4&& RayDirection) {

            float p, d, h;

            switch(Flat) {
            case TOP_X:
                p = EyePoint[0];
                d = RayDirection[0];
                h = HEIGHT_MAX;
                break;
            case TOP_Y:
                p = EyePoint[1];
                d = RayDirection[1];
                h = HEIGHT_MAX;
                break;
            case TOP_Z:
                p = EyePoint[2];
                d = RayDirection[2];
                h = HEIGHT_MAX;
                break;
            case BOTTOM_X:
                p = EyePoint[0];
                d = RayDirection[0];
                h = HEIGHT_MIN;
                break;
            case BOTTOM_Y:
                p = EyePoint[1];
                d = RayDirection[1];
                h = HEIGHT_MIN;
                break;
            case BOTTOM_Z:
                p = EyePoint[2];
                d = RayDirection[2];
                h = HEIGHT_MIN;
                break;
            }

            if (d == 0.f) return INFINITY;

            float t = (h - p) / d;

            if (t <= 0.f) return INFINITY;

            glm::vec4 IntersectionPosition = EyePoint + (t * RayDirection); // find intersect point

            if (Constraint(IntersectionPosition[0], IntersectionPosition[1], IntersectionPosition[2])) {
                 return t;
            } else {
                return INFINITY; // does not follow constraint, no intersection
            }

        };

    }

}

// Different quadratic implicit surfaces
// Each function implements that surface's way of generating the quadratic coefficients a, b, c.
namespace CoefficientGenerators {
    // we only care about the first 3 elements x, y, z in the vec4
    constexpr auto Sphere = [](auto&& eyePoint, auto&& rayDirection) {

        float a = std::pow(rayDirection[0], 2) + std::pow(rayDirection[1], 2)  + std::pow(rayDirection[2], 2);

        float b = 2 * (rayDirection[0] * eyePoint[0] + rayDirection[1] * eyePoint[1] + rayDirection[2] * eyePoint[2]);

        float c = std::pow(eyePoint[0], 2) + std::pow(eyePoint[1], 2)  + std::pow(eyePoint[2], 2) - RADIUS_SQ;

        return std::make_tuple(a, b, c);
    };

    constexpr auto Cylinder = [](auto&& eyePoint, auto&& rayDirection) {
        float a = std::pow(rayDirection[0], 2) + std::pow(rayDirection[2], 2);

        float b = 2 * (rayDirection[0] * eyePoint[0] + rayDirection[2] * eyePoint[2]);

        float c = std::pow(eyePoint[0], 2) + std::pow(eyePoint[2], 2) - RADIUS_SQ;

        return std::make_tuple(a, b, c);
    };


    constexpr auto Cone = [](auto&& eyePoint, auto&& rayDirection) {

        float a = std::pow(rayDirection[0], 2) + std::pow(rayDirection[2], 2) - RADIUS_SQ*std::pow(rayDirection[1], 2);

        float b = 2 * (rayDirection[0] * eyePoint[0] + rayDirection[2] * eyePoint[2])
            - (2 * RADIUS_SQ * rayDirection[1] * eyePoint[1]) + (RADIUS_SQ * rayDirection[1]);

        float c = std::pow(eyePoint[0], 2) + std::pow(eyePoint[2], 2)
            - (RADIUS_SQ * std::pow(eyePoint[1], 2)) +  (RADIUS_SQ *eyePoint[1]) - (std::pow(RADIUS_SQ, 2));

        return std::make_tuple(a, b, c);
    };


}

// Standard types of constraints we might like to place on the validity of an intersection point.
namespace StandardConstraints {
    constexpr auto None = [](auto...) { return true; };

    // bounded height for shape bodies

    constexpr auto BoundedHeight = [](auto x, auto y, auto z) {
        return (y <= HEIGHT_MAX + EPSILON) && (y >= HEIGHT_MIN - EPSILON); // y in a certain range
    };

    // shapes of the flats

    constexpr auto CircleY = [](auto x, auto y, auto z) {
        // TODO: do we need to check if y is within a certain epsilon of the correct value?
        return std::pow(x, 2) + std::pow(z, 2) <= RADIUS_SQ; // x^2 + y^2 <= r^2
    };

    constexpr auto SquareX = [](auto x, auto y, auto z) {
        return y <= HEIGHT_MAX + EPSILON && y >= HEIGHT_MIN - EPSILON && z <= HEIGHT_MAX + EPSILON && z >= HEIGHT_MIN - EPSILON;
    };

    constexpr auto SquareY = [](auto x, auto y, auto z) {
        return x <= HEIGHT_MAX + EPSILON && x >= HEIGHT_MIN - EPSILON && z <= HEIGHT_MAX + EPSILON && z >= HEIGHT_MIN - EPSILON;
    };

    constexpr auto SquareZ = [](auto x, auto y, auto z) {
        return x <= HEIGHT_MAX + EPSILON && x >= HEIGHT_MIN - EPSILON && y <= HEIGHT_MAX + EPSILON && y >= HEIGHT_MIN - EPSILON;
    };

}

namespace SurfaceParamaterization {

    // use spherical coordinates

    constexpr auto Sphere() {
        return [](glm::vec4&& xyz) {
            float u;

            auto phi = glm::asin(xyz[1]/0.5f);
            float v = (phi / M_PI) + 0.5f;

            if ((v <= 0.f+EPSILON && v >= 0.f-EPSILON) || (v <= 1.f+EPSILON && v >= 1.f-EPSILON)) {
                u = 0.5f;
            } else {
                float theta = glm::atan(xyz[2], xyz[0]);

                if (theta < 0.f) { // [-PI,PI] -> [0, 1]
                    u = (-1.f * theta) / (2 * M_PI);
                } else {
                    u = 1 - ((theta) / (2 * M_PI));
                }
            }

            return glm::vec2(u,v);

        };

    }


    // note that the cylinder and the cone have the same uv mapping formula.

    constexpr auto Cone() {

        return [](glm::vec4&& xyz) {
            float u;
            float theta = glm::atan(xyz[2], xyz[0]);

            if (theta < 0.f) { // [-PI,PI] -> [0, 1]
                u = (-1.f * theta) / (2 * M_PI);
            } else {
                u = 1 - ((theta) / (2 * M_PI));
            }

            float v = xyz[1] + 0.5f;

            return glm::vec2(u, v);
        };

    }


    constexpr auto Cylinder() {
        return [](glm::vec4&& xyz) {

            float u;
            float theta = glm::atan(xyz[2], xyz[0]);

            if (theta < 0.f) { // [-PI,PI] -> [0, 1]
                u = (-1.f * theta) / (2 * M_PI);
            } else {
                u = 1 - ((theta) / (2 * M_PI));
            }

            float v = xyz[1] + 0.5f;

            return glm::vec2(u,v);

        };
    }

    // flat side -> use normalized flat plane as index into uv map

    constexpr auto Flat(FlatSide Side) {

        return [=](glm::vec4&& xyz) {

            float u, v;

            switch (Side) {
            case BOTTOM_X:
                u = xyz[2] + 0.5;
                v = xyz[1] + 0.5;
                break;
            case TOP_X:
                u = 1-(xyz[2] + 0.5);
                v = xyz[1] + 0.5;
                break;
            case TOP_Y:
                u = xyz[0] + 0.5;
                v = 1-(xyz[2] + 0.5);
                break;
            case BOTTOM_Y:
                u = xyz[0] + 0.5;
                v = xyz[2] + 0.5;
                break;
            case TOP_Z:
                u = xyz[0] + 0.5;
                v = xyz[1] + 0.5;
                break;
            case BOTTOM_Z:
                u = 1-(xyz[0] + 0.5);
                v = xyz[1] + 0.5;
                break;
            }
            return glm::vec2(u,v);
        };

    }
}

// e.g. Sphere is a function that takes a ray as input and returns the intersection between that ray and the sphere (if there is one).

// SPHERE
inline std::function<ImplicitSignature> Sphere = Solvers::Quadratic(CoefficientGenerators::Sphere, StandardConstraints::None);
inline std::function<ObjectNormalSignature> SphereNormal = Normals::Sphere();
inline std::function<ObjectUVMappingSignature> SphereUVMap = SurfaceParamaterization::Sphere();

// CYLINDER BODY
inline std::function<ImplicitSignature>  CylinderSide = Solvers::Quadratic(CoefficientGenerators::Cylinder, StandardConstraints::BoundedHeight);
inline std::function<ObjectNormalSignature> CylinderSideNormal = Normals::Cylinder();
inline std::function<ObjectUVMappingSignature> CylinderSideUVMap = SurfaceParamaterization::Cylinder();
// CYLINDER TOP
inline std::function<ImplicitSignature>  CylinderTop = Solvers::Flat(TOP_Y, StandardConstraints::CircleY);
inline std::function<ObjectNormalSignature> CylinderTopNormal = Normals::Flat(TOP_Y);
inline std::function<ObjectUVMappingSignature> CylinderTopUVMap = SurfaceParamaterization::Flat(TOP_Y);
// CYLINDER BOTTOM
inline std::function<ImplicitSignature>  CylinderBottom = Solvers::Flat(BOTTOM_Y, StandardConstraints::CircleY);
inline std::function<ObjectNormalSignature> CylinderBottomNormal = Normals::Flat(BOTTOM_Y);
inline std::function<ObjectUVMappingSignature> CylinderBottomUVMap = SurfaceParamaterization::Flat(BOTTOM_Y);

// CONE BODY
inline std::function<ImplicitSignature>  ConeSide = Solvers::Quadratic(CoefficientGenerators::Cone, StandardConstraints::BoundedHeight);
inline std::function<ObjectNormalSignature> ConeSideNormal = Normals::Cone();
inline std::function<ObjectUVMappingSignature> ConeSideUVMap = SurfaceParamaterization::Cone();
// CONE BOTTOM
inline std::function<ImplicitSignature>  ConeBottom = Solvers::Flat(BOTTOM_Y, StandardConstraints::CircleY);
inline std::function<ObjectNormalSignature> ConeBottomNormal = Normals::Flat(BOTTOM_Y);
inline std::function<ObjectUVMappingSignature> ConeBottomUVMap = SurfaceParamaterization::Flat(BOTTOM_Y);

// CUBE 6 FACES
inline std::function<ImplicitSignature>  CubeTopX = Solvers::Flat(TOP_X, StandardConstraints::SquareX);
inline std::function<ObjectNormalSignature> CubeTopXNormal = Normals::Flat(TOP_X);
inline std::function<ObjectUVMappingSignature> CubeTopXUVMap = SurfaceParamaterization::Flat(TOP_X);

inline std::function<ImplicitSignature>  CubeBottomX = Solvers::Flat(BOTTOM_X, StandardConstraints::SquareX);
inline std::function<ObjectNormalSignature> CubeBottomXNormal = Normals::Flat(BOTTOM_X);
inline std::function<ObjectUVMappingSignature> CubeBottomXUVMap = SurfaceParamaterization::Flat(BOTTOM_X);

inline std::function<ImplicitSignature>  CubeTopY = Solvers::Flat(TOP_Y, StandardConstraints::SquareY);
inline std::function<ObjectNormalSignature> CubeTopYNormal = Normals::Flat(TOP_Y);
inline std::function<ObjectUVMappingSignature> CubeTopYUVMap = SurfaceParamaterization::Flat(TOP_Y);

inline std::function<ImplicitSignature>  CubeBottomY = Solvers::Flat(BOTTOM_Y, StandardConstraints::SquareY);
inline std::function<ObjectNormalSignature> CubeBottomYNormal = Normals::Flat(BOTTOM_Y);
inline std::function<ObjectUVMappingSignature> CubeBottomYUVMap = SurfaceParamaterization::Flat(BOTTOM_Y);

inline std::function<ImplicitSignature>  CubeTopZ = Solvers::Flat(TOP_Z, StandardConstraints::SquareZ);
inline std::function<ObjectNormalSignature> CubeTopZNormal = Normals::Flat(TOP_Z);
inline std::function<ObjectUVMappingSignature> CubeTopZUVMap = SurfaceParamaterization::Flat(TOP_Z);

inline std::function<ImplicitSignature>  CubeBottomZ = Solvers::Flat(BOTTOM_Z, StandardConstraints::SquareZ);
inline std::function<ObjectNormalSignature> CubeBottomZNormal = Normals::Flat(BOTTOM_Z);
inline std::function<ObjectUVMappingSignature> CubeBottomZUVMap = SurfaceParamaterization::Flat(BOTTOM_Z);



