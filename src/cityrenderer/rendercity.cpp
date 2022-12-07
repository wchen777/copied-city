#include "realtime.h"
#include "settings.h"
#include <iostream>
#include <ostream>
#include <map>

#define MAX_LIGHTS 8

/*
 * go through each mesh object struct and generate and bind the VBOs and VAOs
*/
void CopiedCity::InitializeBuffers() {



//    // map from string key to VBO
//    std::unordered_map<std::string, GLuint> keyToVBO;

//    // generate VBO and VAO for every mesh
//    for (MeshPrimitive& mesh : CopiedCity::objectMeshes) {

//        auto key = mesh.trimesh->GetKey(); // unique key for a trimesh with parameter and type

//        // check to see the map if a tesselation of the same parameters and type exists.
//        if (keyToVBO.find(key) != keyToVBO.end()) {
//            // if it exists in the map, our mesh vbo is the one in the map
//            mesh.vbo = keyToVBO[key];
//        } else {
//            // if not, generate a new VBO
//            glGenBuffers(1, &mesh.vbo);
//        }

        glGenBuffers(1, &cubeVBO);
        // mesh.vbo is now populated with the correct mesh

        // bind that VBO to the state machine
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

        // Send data to VBO. use the city's single cube as vertex data since it will be constant for all cubes.
        glBufferData(GL_ARRAY_BUFFER, CopiedCity::city.cube->m_vertexData.size() * sizeof(GLfloat),
                     CopiedCity::city.cube->m_vertexData.data(), GL_STATIC_DRAW);

        // go through each part of the city and create a vao for each object


        // --------- PLANE ---------- //
        CopiedCity::city.plane.planeData.vbo = cubeVBO;

        // Generate, and bind VAO
        glGenVertexArrays(1, &city.plane.planeData.vao);
        glBindVertexArray(city.plane.planeData.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Clean-up bindings for VAO
        glBindVertexArray(0);

        // TODO: boundary box and facades.

        // clean up VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}
