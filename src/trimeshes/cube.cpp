#include "cube.h"


/*
 *  get type
*/
PrimitiveType Cube::GetType() {
    return PrimitiveType::PRIMITIVE_CUBE;
}

/*
 * create a single side of the cube out of the 4 given points
*/
void Cube::MakeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    float sides = static_cast<float>(m_param1);

    auto xOff = (topRight - topLeft)/sides;
    auto yOff = (bottomLeft - topLeft)/sides;

    for (int r = 0; r < m_param1; ++r) {
        for (int c = 0; c < m_param1; ++c) {
            auto offset = (float(c) * xOff) + (float(r) * yOff); // offset from real top left
            auto newTopLeft = topLeft + offset;
            Cube::MakeFlatCapTile(newTopLeft, newTopLeft+xOff, newTopLeft+yOff, newTopLeft+xOff+yOff);
        }
    }
}

/*
 * create all 6 faces of the cube
*/
void Cube::SetVertexData() {

     Cube::MakeFace(glm::vec3(-0.5f,  0.5f, 0.5f), // positive z constant
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));


     Cube::MakeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f), // negative z constant
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));

     Cube::MakeFace(glm::vec3(0.5f, 0.5f, 0.5f), // positive x constant
              glm::vec3(0.5f, 0.5f, -0.5f),
              glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3(0.5f, -0.5f, -0.5f));

     Cube::MakeFace(glm::vec3(-0.5f, 0.5f, -0.5f), // negative x constant
              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f));


     Cube::MakeFace(glm::vec3(0.5f, 0.5f, 0.5f), // positive y constant
              glm::vec3(-0.5f, 0.5f, 0.5f),
              glm::vec3(0.5f, 0.5f, -0.5f),
              glm::vec3(-0.5f, 0.5f, -0.5f));

     Cube::MakeFace(glm::vec3(-0.5f, -0.5f, 0.5f), // negative y constant
              glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3(0.5f, -0.5f, -0.5f));
}

// UNUSED FOR CUBE
inline glm::vec3 Cube::ShapeNormal(glm::vec3& xyz) {
    return xyz;
}
