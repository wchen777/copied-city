#include "trimesh.h"

// derived class for a cube from trimesh implicit
class Cylinder : public TrimeshData {
public:
    virtual inline glm::vec3 ShapeNormal(glm::vec3& xyz) override;
    virtual void SetVertexData() override;
    virtual PrimitiveType GetType() override;

private:
    float m_radius = 0.5f;
    void MakeWedge(float currTheta, float nextTheta);
    void MakeTriangleCaps(float xStart, float zStart, float xEnd, float zEnd);
    void MakeCylinder();
};
