#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <glm/glm.hpp>
#include <QImage>
#include <vector>


struct TextureMap {
    std::vector<glm::vec3> data;
    int height;
    int width;

    glm::vec3 NearestNeighborSampling(float u, float v, float repeatU, float repeatV);
    glm::vec3 BilinearSampling(float u, float v, float repeatU, float repeatV);
};


// load the texture into the texture map struct given a file path.
bool loadTextureFromFile(TextureMap *texture, std::string file);
