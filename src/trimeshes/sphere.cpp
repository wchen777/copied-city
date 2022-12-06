#include "sphere.h"

/*
 *  get type
*/
PrimitiveType Sphere::GetType() {
    return PrimitiveType::PRIMITIVE_SPHERE;
}


/*
 * shape specific normal
*/
inline glm::vec3 Sphere::ShapeNormal(glm::vec3& xyz) {
    return glm::normalize(xyz);
}


/*
 * call the create sphere function
*/
void Sphere::SetVertexData(){
    Sphere::MakeSphere();
}


/*
 * create the sphere by filling m vertex data
*/
void Sphere::MakeSphere() {
    // per angle step
    float thetaStep = glm::radians(360.f / m_param2);

    // create a slice
    for (int t = 0; t < m_param2; ++t) {
        Sphere::MakeWedge(thetaStep * t, thetaStep * (t + 1));
    }

}

/*
 * createa single wedge of the sphere
*/
void Sphere::MakeWedge(float currentTheta, float nextTheta) {
    m_param1 = std::max(2, m_param1);
    float phiStep = glm::radians(180.f / m_param1);

    for (int p = 0; p < m_param1; ++p) {

        auto startPhi = phiStep * p;
        auto endPhi = phiStep * (p+1);

        auto sinStartPhi = glm::sin(startPhi);
        auto sinEndPhi = glm::sin(endPhi);
        auto cosStartPhi = glm::cos(startPhi);
        auto cosEndPhi = glm::cos(endPhi);

        auto sinCurrentTheta = glm::sin(currentTheta);
        auto cosCurrentTheta = glm::cos(currentTheta);
        auto sinNextTheta = glm::sin(nextTheta);
        auto cosNextTheta = glm::cos(nextTheta);

        auto tlX = m_radius * sinStartPhi * sinCurrentTheta;
        auto tlY = m_radius * cosStartPhi;
        auto tlZ = m_radius * sinStartPhi * cosCurrentTheta;

        auto trX = m_radius * sinStartPhi * sinNextTheta;
        auto trY = m_radius * cosStartPhi;
        auto trZ = m_radius * sinStartPhi * cosNextTheta;

        auto blX = m_radius * sinEndPhi * sinCurrentTheta;
        auto blY = m_radius * cosEndPhi;
        auto blZ = m_radius * sinEndPhi * cosCurrentTheta;

        auto brX = m_radius * sinEndPhi * sinNextTheta;
        auto brY = m_radius * cosEndPhi;
        auto brZ = m_radius * sinEndPhi * cosNextTheta;

        // this uses the sphere's custom normal
        Sphere::MakeSideTile(glm::vec3(tlX, tlY, tlZ), glm::vec3(trX, trY, trZ), glm::vec3(blX, blY, blZ), glm::vec3(brX, brY, brZ));

    }
}
