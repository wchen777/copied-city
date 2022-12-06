#include "cone.h"

/*
 *  get type
*/
PrimitiveType Cone::GetType() {
    return PrimitiveType::PRIMITIVE_CONE;
}

/*
 *  cone's normal formula
*/
inline glm::vec3 Cone::ShapeNormal(glm::vec3& xyz) {
    return glm::normalize(glm::vec3(xyz[0], 0.5 * std::sqrt(std::pow(xyz[0], 2) + std::pow(xyz[2], 2)), xyz[2]));
}


/*
 *  set vertex data override for derived class
*/
void Cone::SetVertexData() {
    // TODO for Project 5: Lights, Camera

    float thetaStep = glm::radians(360.f / m_param2);

    // make each wedge around the cone
    for (int t = 0; t < m_param2; ++t) {
        Cone::MakeWedge(thetaStep * t, thetaStep * (t + 1));
    }

}

/*
 *  special case the top part of the cone
*/
void Cone::MakeTopTriangles(float xStart, float zStart, float xEnd, float zEnd, float y, float middleTheta) {
    // for the side face
    glm::vec3 l = glm::vec3(xStart, y, zStart);
    glm::vec3 r = glm::vec3(xEnd, y, zEnd);
    glm::vec3 top = glm::vec3(0.f, HEIGHT_MAX, 0.f);
    glm::vec3 adjustedTop = glm::vec3(Cone::m_radius * glm::cos(middleTheta) * EPSILON_OFF, HEIGHT_MAX, Cone::m_radius * glm::sin(middleTheta) * EPSILON_OFF);

    glm::vec3 lNorm = Cone::ShapeNormal(l);
    glm::vec3 rNorm = Cone::ShapeNormal(r);
    glm::vec3 topNorm = Cone::ShapeNormal(adjustedTop);

    Cone::insertVec3(m_vertexData, l);
    Cone::insertVec3(m_vertexData, lNorm);
    Cone::insertVec3(m_vertexData, top);
    Cone::insertVec3(m_vertexData, topNorm);
    Cone::insertVec3(m_vertexData, r);
    Cone::insertVec3(m_vertexData, rNorm);

    // for the bottom face
    glm::vec3 lBottom = glm::vec3(xStart, HEIGHT_MIN, zStart);
    glm::vec3 rBottom = glm::vec3(xEnd, HEIGHT_MIN, zEnd);
    glm::vec3 bottom = glm::vec3(0.f, HEIGHT_MIN, 0.f);

    auto lBNorm = glm::normalize(glm::cross(rBottom - lBottom, bottom - lBottom));
    auto bottomNorm = glm::normalize(glm::cross(lBottom - bottom, rBottom - bottom));
    auto rBNorm = glm::normalize(glm::cross(bottom - rBottom, lBottom - rBottom));

    Cone::insertVec3(m_vertexData, lBottom);
    Cone::insertVec3(m_vertexData, lBNorm);
    Cone::insertVec3(m_vertexData, rBottom);
    Cone::insertVec3(m_vertexData, rBNorm);
    Cone::insertVec3(m_vertexData, bottom);
    Cone::insertVec3(m_vertexData, bottomNorm);

}

/*
 *  make a section of the vertical side of the cone
*/
void Cone::MakeWedge(float currentTheta, float nextTheta) {
    float faces = static_cast<float>(m_param1);

    auto zStart = Cone::m_radius * glm::sin(currentTheta);
    auto xStart = Cone::m_radius * glm::cos(currentTheta);
    auto zEnd = Cone::m_radius * glm::sin(nextTheta);
    auto xEnd = Cone::m_radius * glm::cos(nextTheta);
    auto middleTheta = currentTheta + ((nextTheta - currentTheta) / 2);

    // make the top and bottom caps for this wedge


    // for the Cone faces, y will range from 0.5 to -0.5
    // x and z will depend on the start theta and the next theta and the current height

    // there will be m_param1 - 1 tile tesselations, and 1 triangle for the top

    float heightInc = 1.f / faces;
    float height = HEIGHT_MIN;
    float radiusScale = 1.f;

    for (int p = 0; p < m_param1-1; ++p) {
        float newHeight = height + heightInc;
        float newRadiusScale = radiusScale - heightInc;

        // this is for the side face
        Cone::MakeSideTile(glm::vec3(xStart * radiusScale, height, zStart * radiusScale),
                           glm::vec3(xEnd * radiusScale, height, zEnd * radiusScale),
                           glm::vec3(xStart * newRadiusScale, newHeight, zStart * newRadiusScale),
                           glm::vec3(xEnd * newRadiusScale, newHeight, zEnd * newRadiusScale));

        // this is for the bottom face
        Cone::MakeFlatCapTile(glm::vec3(xStart * newRadiusScale, HEIGHT_MIN, zStart * newRadiusScale),
                          glm::vec3(xEnd * newRadiusScale, HEIGHT_MIN, zEnd * newRadiusScale),
                          glm::vec3(xStart * radiusScale, HEIGHT_MIN, zStart * radiusScale),
                          glm::vec3(xEnd * radiusScale, HEIGHT_MIN, zEnd * radiusScale));

        height = newHeight;
        radiusScale = newRadiusScale;
    }

    // build top triangles
    Cone::MakeTopTriangles(xStart * radiusScale, zStart * radiusScale, xEnd * radiusScale, zEnd * radiusScale, height, middleTheta);

}
