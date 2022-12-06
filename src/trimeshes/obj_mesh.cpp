#include "obj_mesh.h"


/*
 *  unused normal
*/
inline glm::vec3 OBJMesh::ShapeNormal(glm::vec3& xyz) {
    return xyz;
}


/*
 *  get type
*/
PrimitiveType OBJMesh::GetType() {
    return PrimitiveType::PRIMITIVE_MESH;
}

bool OBJMesh::LoadOBJ(std::string filepath) {
    OBJMesh::filepath = filepath; // set filepath
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    // temporary vectors for data read in
    std::vector<glm::vec3> verticesRead;
    std::vector< glm::vec2> uvRead;
    std::vector<glm::vec3> normalsRead;

    // read in the obj file (i only know how to do this in C, lol)
    FILE* file = fopen(filepath.c_str(), "r");
    if (file == NULL){
        std::cout <<  "Could not open OBJ file." << std::endl;
        return false;
    }

    int line = 1;

    // keep going throughout the file until we reach EOF, read in all data
    while (1) {

        char firstWord[128]; // this is the indicator for what type of data is specified on the line of the obj file
        int res = fscanf(file, "%s", firstWord);

        if (res == EOF) break; // we have reached the end of the file

        if (!strcmp(firstWord, "v")) { // a vertex

            glm::vec3 vertexToAdd;
            fscanf(file, "%f %f %f\n", &vertexToAdd[0], &vertexToAdd[1], &vertexToAdd[2]);
            verticesRead.push_back(vertexToAdd);

        } else if (!strcmp(firstWord, "vt")){ // a uv texture map coordinate, this is prob unused as of now

            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv[0], &uv[1]);
            uvRead.push_back(uv);

        } else if (!strcmp(firstWord, "vn")){ // a vertex normal

            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
            normalsRead.push_back(normal);

        } else if (!strcmp(firstWord, "f")){ // a face
            // three vertices
            std::string v1;
            std::string v2;
            std::string v3;

            // holders for each of the read in indices
            std::array<unsigned int, 3> vertexIndex, uvIndex, normalIndex;

            // read each vertex/norma/uv index
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1],
                                 &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9){
                matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
                                     &vertexIndex[0], &normalIndex[0], &vertexIndex[1],
                                     &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 4) {
                    std::cout << "Could not parse line with face data, matches: " << matches << std::endl;
                    std::cout << "Line #" << line << std::endl;
                    line++;
                    continue;
                }

            }

            // add to our indices vector

            // these indices grouped by triangle with the correct ordering!
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            //            uvIndices.push_back(uvIndex[0]);
            //            uvIndices.push_back(uvIndex[1]);
            //            uvIndices.push_back(uvIndex[2]);

        }

        line++;
    }

    assert(vertexIndices.size() == normalIndices.size());

    // once data is all read, go through each vertex of each triangle and fill vectors, same for normals
    for( unsigned int i=0;  i< vertexIndices.size(); ++i){
        auto vertexIndex = vertexIndices[i];
        glm::vec3 vertex = verticesRead[vertexIndex-1]; // .obj is 1-indexed

        auto normalIndex = normalIndices[i];
        glm::vec3 normal = verticesRead[normalIndex-1]; // .obj is 1-indexed

        // add to our buffer
        OBJMesh::insertVec3(OBJMesh::m_vertexData, vertex);
        OBJMesh::insertVec3(OBJMesh::m_vertexData, normal);
    }

    std::cout << "Successfully read in " << m_vertexData.size() << " indices" << std::endl;

    return true;
}

/*
 *  unused
*/
void OBJMesh::SetVertexData() {
    return;
}
