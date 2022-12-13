#pragma once

#include "citygenerator/staticposdata.h"
#include "citygenerator/objutils.h"

// constants

#define PROGENITOR_HEIGHT 110.f
#define PROGENITOR_LENGTH 250.f
#define PROGENITOR_WIDTH 20.f

// MIN Z SIZE
#define MIN_Z 5.f
// MIN X SIZE
#define MIN_X 5.f
// MIN HEIGHT
#define MIN_HEIGHT 3.f

// MIN/MAX X TRANSLATE, should be small, for perturbations
#define MIN_X_TRANS -1.5f
#define MAX_X_TRANS 1.5f
// MIN/MAX X SCALE
#define MIN_X_SCALE 1.05f
#define MAX_X_SCALE 0.95f

// HEIGHT VARIANCE RANGE
#define MIN_HEIGHT_SCALE 0.3
#define MAX_HEIGHT_SCALE 0.9

struct Block {

    // height parameter (y-axis)
    float height;

    // length parameters (z-axis) FIRST SUBDIVIDE PHASE
    float zStart;
    float zEnd;

    // width parameters (x-axis)  SECOND SUBDIVIDE PHASE
    float xStart;
    float xEnd;

    // x perturbations
    float xTranslate;
    float xScale;

    // TODO: account for back facade by adding a rotate parameter

    // fill a mesh object given a block.
    void BlockToMeshObject(CityMeshObject* meshToInit, float xOffset, float zOffset) {
        // get center of the block
        glm::vec3 centerXYZ = Block::GetCenter();

        int randDiffuse = arc4random() % 3;
        int randSpec = arc4random() % 2;

        ObjSceneData blockData = ObjSceneData{
                // translate using the center of the block, and the starting xOffset
                .translation = glm::vec3(centerXYZ[0] + xOffset + xTranslate, -4.f + height/2.f, centerXYZ[2] + zOffset - Z_SHIFT),
                // scale based off the size of the block, or its z and x length, and height
                // MAY NEED TO MODIFY THIS
                .scale = glm::vec3((Block::xEnd - Block::xStart)*xScale, height, Block::zEnd - Block::zStart),
                // randomly choose shades of color
                .diffuse = randDiffuse == 0 ? DIFFUSE_WHITE_1 : randDiffuse == 1 ? DIFFUSE_WHITE_2 : DIFFUSE_WHITE_3,
                .specular = randSpec == 0 ? SPECULAR_WHITE_1 : SPECULAR_WHITE_2,
                .ambient = AMBIENT_WHITE_1,
                .shininess = 0.f
        };

        // fill the mesh's properties
        InitializeSpaceConversions(meshToInit, &blockData);
        InitializeMaterial(meshToInit, &blockData);
    }

    glm::vec3 GetCenter() {
        return glm::vec3(xStart + (xEnd - xStart), height / 2.f, zStart + (zEnd - zStart));
    }
};

// IDEA, either y perturbations or, create a block on the top part of the screen
