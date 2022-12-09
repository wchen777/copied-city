#pragma once

#include "trimesh.h"

// derived class for a cube from trimesh implicit
class Cube : public TrimeshData {
public:
    virtual inline glm::vec3 ShapeNormal(glm::vec3& xyz) override;
    virtual void SetVertexData() override;
    virtual PrimitiveType GetType() override;

private:
    void MakeWedge(float currTheta, float nextTheta);
    void MakeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

};
