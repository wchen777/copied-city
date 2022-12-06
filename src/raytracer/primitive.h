#pragma once
#include "implicit.h"
#include "../shader/texture.h"
#include "utils/sceneparser.h"
#include <functional>
#include <map>

// -------- defs for types -------- //

// the sigature for a primitive solver function that takes in a world space eye and world space ray and returns a t-value
using PrimitiveSignature = auto(glm::vec4& WorldSpaceEyePoint, glm::vec4& WorldSpaceRayDirection)->float;
// function to find a surface normal for a given shape/side that takes in a given t-value, world space eye point and world space ray
using NormalSignature = auto(float t, glm::vec4& WorldSpaceEyePoint, glm::vec4& WorldSpaceRayDirection)->glm::vec4;

// signature for a function to find a <u,v> coordinate given an intersection point (p + td)
using UVMappingSignature = auto(glm::vec4& WorldSpaceIntersection)->glm::vec2;


// a composite struct for the solver function and the surface normal functions
struct PrimitiveSolver {
    std::function<PrimitiveSignature> SolverFunction;
    std::function<NormalSignature> SurfaceNormal;
    std::function<UVMappingSignature> UVMappingFunction;
    glm::mat4 InvTransposeCTM;
    SceneMaterial MaterialData;
    TextureMap* Texture;
};


// ------- function defs --------- //

// create a primitive solver function from the inverse CTM and the implicit function
std::function<PrimitiveSignature> MakeSolver(glm::mat4& InverseCTM, std::function<ImplicitSignature>& ImplicitFunction);
std::function<NormalSignature> MakeNormal(glm::mat4& InverseCTM, std::function<ObjectNormalSignature> ObjectNormalFunction);
std::function<UVMappingSignature> MakeUVMapping(glm::mat4& InverseCTM, std::function<ObjectUVMappingSignature> ObjectUVMappingFunction);

// compile all the primitive solvers for a given shape
void CompilePrimitives(const RenderShapeData& primitive, std::vector<PrimitiveSolver>& collection, std::map<std::string, TextureMap*> filepathToTexture, bool useTexture);
