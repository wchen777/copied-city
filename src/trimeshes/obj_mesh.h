#pragma once

#include "trimesh.h"
#include <string>
#include <array>
#include <iostream>
#include <ostream>
#include <cstring>

class OBJMesh : public TrimeshData {
public:
    OBJMesh(std::string filepath) {
        LoadOBJ(filepath);
    }
    // unused
    virtual inline glm::vec3 ShapeNormal(glm::vec3& xyz) override;

    virtual void SetVertexData() override;

    virtual PrimitiveType GetType() override;

private:
    bool LoadOBJ(std::string filepath);
};
