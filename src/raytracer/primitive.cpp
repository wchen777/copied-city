#include "primitive.h"
#include "utils/sceneparser.h"


/*
 * function to create a primitive function based on an inverse CTM and implicit function
 * this primitive function takes in a world space eyepoint and ray direction and returns the t value and the surface normal
*/
std::function<PrimitiveSignature> MakeSolver(glm::mat4& InverseCTM, std::function<ImplicitSignature>& ImplicitFunction) {
    return [=](glm::vec4& WorldSpaceEyePoint, glm::vec4& WorldSpaceRayDirection) {
        float t = ImplicitFunction(InverseCTM * WorldSpaceEyePoint, InverseCTM * WorldSpaceRayDirection);
        return t;
    };
}


/*
 * function to create a lambda to solve for a surface normal given an intersecting point.
 * pass in an inverse ctm and the respective object's normal function
*/
std::function<NormalSignature> MakeNormal(glm::mat4& InverseCTM, std::function<ObjectNormalSignature> ObjectNormalFunction) {
    return [=](float t, glm::vec4& WorldSpaceEyePoint, glm::vec4& WorldSpaceRayDirection) {
        return ObjectNormalFunction(t, InverseCTM * WorldSpaceEyePoint, InverseCTM * WorldSpaceRayDirection);
    };
}


/*
 * function to create a lambda to solve for a u,v coordinate given an intersecting point
 */
std::function<UVMappingSignature> MakeUVMapping(glm::mat4& InverseCTM, std::function<ObjectUVMappingSignature> ObjectUVMappingFunction) {
    return [=](glm::vec4& WorldSpaceIntersection) {
        return ObjectUVMappingFunction(InverseCTM * WorldSpaceIntersection);
    };
}


/*
 * takes in shape data and a collection of primitive functions to be filled, call this for each object in order to create its primitives list
*/
void CompilePrimitives(const RenderShapeData& obj, std::vector<PrimitiveSolver>& collection, std::map<std::string, TextureMap*> filepathToTexture, bool useTexture) {
    glm::mat4 invCTM = glm::inverse(obj.ctm);
    glm::mat4 invTransposeCTM = glm::inverse(glm::transpose(obj.ctm));

    TextureMap* texture;
    bool hasTexture = obj.primitive.material.textureMap.isUsed;

    if (hasTexture && useTexture) {
        // check map if filepath exists
        if (filepathToTexture.find(obj.primitive.material.textureMap.filename) != filepathToTexture.end()) {
            texture = filepathToTexture[obj.primitive.material.textureMap.filename]; // if so, set the objects texture map to be the one in the map
        } else {
            texture = new TextureMap; // malloc new texture map
            loadTextureFromFile(texture, obj.primitive.material.textureMap.filename); // add it to map
        }
    }

    switch(obj.primitive.type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeTopX),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeTopXNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeTopXUVMap) : NULL, // don't create if not necessary
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeTopY),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeTopYNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeTopYUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeTopZ),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeTopZNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeTopZUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeBottomX),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeBottomXNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeBottomXUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeBottomY),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeBottomYNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeBottomYUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver{
                                    .SolverFunction = MakeSolver(invCTM, CubeBottomZ),
                                    .SurfaceNormal = MakeNormal(invCTM, CubeBottomZNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CubeBottomZUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, ConeSide),
                                    .SurfaceNormal = MakeNormal(invCTM, ConeSideNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, ConeSideUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, ConeBottom),
                                    .SurfaceNormal = MakeNormal(invCTM, ConeBottomNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, ConeBottomUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, Sphere),
                                    .SurfaceNormal = MakeNormal(invCTM, SphereNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, SphereUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:

        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, CylinderBottom),
                                    .SurfaceNormal = MakeNormal(invCTM, CylinderBottomNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CylinderBottomUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, CylinderSide),
                                    .SurfaceNormal = MakeNormal(invCTM, CylinderSideNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CylinderSideUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });

        collection.emplace_back(PrimitiveSolver {
                                    .SolverFunction = MakeSolver(invCTM, CylinderTop),
                                    .SurfaceNormal = MakeNormal(invCTM, CylinderTopNormal),
                                    .UVMappingFunction = hasTexture ? MakeUVMapping(invCTM, CylinderTopUVMap) : NULL,
                                    .InvTransposeCTM = invTransposeCTM,
                                    .MaterialData = obj.primitive.material,
                                    .Texture = texture
                                });
        break;
    default:
        break;
    }

}
