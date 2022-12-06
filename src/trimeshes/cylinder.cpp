#include "cylinder.h"


/*
 *  get type
*/
PrimitiveType Cylinder::GetType() {
    return PrimitiveType::PRIMITIVE_CYLINDER;
}


/*
 *  set vertex data override for derived class
*/
void Cylinder::SetVertexData() {
    Cylinder::MakeCylinder();
}

/*
 *  cylinder's normal function
*/
inline glm::vec3 Cylinder::ShapeNormal(glm::vec3& xyz) {
    return glm::normalize(glm::vec3(xyz[0], 0.f, xyz[2]));
}

/*
 *   make a section of the vertical side of the cylinder
*/
void Cylinder::MakeWedge(float currentTheta, float nextTheta) {
    float faces = static_cast<float>(m_param1);

    auto zStart = Cylinder::m_radius * glm::sin(currentTheta);
    auto xStart = Cylinder::m_radius * glm::cos(currentTheta);
    auto zEnd = Cylinder::m_radius * glm::sin(nextTheta);
    auto xEnd = Cylinder::m_radius * glm::cos(nextTheta);


    // for the cylinder faces, y will range from 0.5 to -0.5
    // x and z will depend on the start theta and the next theta

    float height = -0.5f;
    float heightInc = 1.f / faces;
    float radiusScale = 1.f;

    for (int p = 0; p < m_param1; ++p) {
        float nextHeight = height + heightInc;
        // the side tiles
        Cylinder::MakeSideTile(glm::vec3(xStart, height, zStart),
                           glm::vec3(xEnd, height, zEnd),
                           glm::vec3(xStart, nextHeight, zStart),
                           glm::vec3(xEnd, nextHeight, zEnd));
        height = nextHeight;
    }

     // the tesselated tiles on top and bottom caps
    for (int p = 0 ; p < m_param1 - 1; ++p) {
        float nextRadiusScale = radiusScale - heightInc;

        Cylinder::MakeFlatCapTile(glm::vec3(xStart * radiusScale, 0.5f, zStart * radiusScale),
                           glm::vec3(xEnd * radiusScale, 0.5f, zEnd * radiusScale),
                           glm::vec3(xStart * nextRadiusScale, 0.5f, zStart * nextRadiusScale),
                           glm::vec3(xEnd * nextRadiusScale, 0.5f, zEnd * nextRadiusScale));

        Cylinder::MakeFlatCapTile(glm::vec3(xStart * nextRadiusScale, -0.5f, zStart * nextRadiusScale),
                              glm::vec3(xEnd * nextRadiusScale, -0.5f, zEnd * nextRadiusScale),
                              glm::vec3(xStart * radiusScale, -0.5f, zStart * radiusScale),
                              glm::vec3(xEnd * radiusScale, -0.5f, zEnd * radiusScale));
        radiusScale = nextRadiusScale;
    }

    // special case the last triangles
    Cylinder::MakeTriangleCaps(xStart * radiusScale, zStart * radiusScale, xEnd * radiusScale, zEnd * radiusScale);
}



/*
 *  make the top triangular seciton of the flat cap of the cylinder (special case) (for the top and bottom). called within makeWedge.
*/
void Cylinder::MakeTriangleCaps(float xStart, float zStart, float xEnd, float zEnd) {
    // top triangle
    glm::vec3 centerTop = glm::vec3(0.f, 0.5f, 0.0f);
    glm::vec3 startTop = glm::vec3(xStart, 0.5f, zStart);
    glm::vec3 endTop = glm::vec3(xEnd, 0.5f, zEnd);

    glm::vec3 centerTopNorm = glm::normalize(glm::cross(endTop - centerTop, startTop - centerTop));
    glm::vec3 startTopNorm = glm::normalize(glm::cross(centerTop - startTop, endTop - startTop));
    glm::vec3 endTopNorm = glm::normalize(glm::cross(startTop - endTop, centerTop - endTop));

    // bottom triangle
    glm::vec3 centerBottom = glm::vec3(0.f, -0.5f, 0.0f);
    glm::vec3 startBottom = glm::vec3(xStart, -0.5f, zStart);
    glm::vec3 endBottom = glm::vec3(xEnd, -0.5f, zEnd);

    glm::vec3 centerBottomNorm = glm::normalize(glm::cross(startBottom - centerBottom, endBottom - centerBottom));
    glm::vec3 startBottomNorm = glm::normalize(glm::cross(endBottom - startBottom, centerBottom - startBottom));
    glm::vec3 endBottomNorm = glm::normalize(glm::cross(centerBottom - endBottom, startBottom - endBottom));


    // add to buffer,
    // top
    Cylinder::insertVec3(m_vertexData, centerTop);
    Cylinder::insertVec3(m_vertexData, centerTopNorm);
    Cylinder::insertVec3(m_vertexData, endTop);
    Cylinder::insertVec3(m_vertexData, endTopNorm);
    Cylinder::insertVec3(m_vertexData, startTop);
    Cylinder::insertVec3(m_vertexData, startTopNorm);

    // bottom (reverse top's order)
    Cylinder::insertVec3(m_vertexData, centerBottom);
    Cylinder::insertVec3(m_vertexData, centerBottomNorm);
    Cylinder::insertVec3(m_vertexData, startBottom);
    Cylinder::insertVec3(m_vertexData, startBottomNorm);
    Cylinder::insertVec3(m_vertexData, endBottom);
    Cylinder::insertVec3(m_vertexData, endBottomNorm);

}


/*
 *  make cylinder override
*/
void Cylinder::MakeCylinder() {

    float thetaStep = glm::radians(360.f / m_param2);

    // make each wedge around the cylinder
    for (int t = 0; t < m_param2; ++t) {
        Cylinder::MakeWedge(thetaStep * t, thetaStep * (t + 1));
    }
}
