#include "texture.h"


inline int indFromCoord(int r, int c, int width) {
    return r*width + c;
}

// sample the texture map using nearest neighbor sampling
glm::vec3 TextureMap::NearestNeighborSampling(float u, float v, float repeatU, float repeatV) {
    int r = int(std::floor((1-v) * (TextureMap::height-1) * repeatV)) % (TextureMap::height-1);
    int c = int(u * (TextureMap::width-1) * repeatU) % (TextureMap::width-1);

    return TextureMap::data[indFromCoord(r, c, TextureMap::width)];
}


// interpolation functions
inline float ease(float alpha) {
    return (3 * std::pow(alpha, 2)) - (2 * std::pow(alpha, 3));
}

inline glm::vec3 interpolate(glm::vec3& A, glm::vec3& B, float alpha) {
    return A + (ease(alpha) * (B - A));
}

// bilinear interpolation
glm::vec3 TextureMap::BilinearSampling(float u, float v, float repeatU, float repeatV) {
    auto imgHeight = TextureMap::height-1;
    auto imgWidth = TextureMap::width-1;

    float r_unrounded = std::fmod(((1-v) * (imgHeight) * repeatV), imgHeight);
    float c_unrounded = std::fmod(u * (imgWidth) * repeatU, imgWidth);

    int r_floor = static_cast<int>(r_unrounded);
    int c_floor = static_cast<int>(c_unrounded);

    int r_ceil = std::min(r_floor+1, imgHeight);
    int c_ceil = std::min(c_floor+1, imgWidth);

    float r_diff = float(r_ceil) - r_unrounded;
    float c_diff = c_unrounded - float(c_floor);

    auto tl = TextureMap::data[indFromCoord(r_floor, c_floor, TextureMap::width)];
    auto tr = TextureMap::data[indFromCoord(r_floor, c_ceil, TextureMap::width)];
    auto bl = TextureMap::data[indFromCoord(r_ceil, c_floor, TextureMap::width)];
    auto br = TextureMap::data[indFromCoord(r_ceil, c_ceil, TextureMap::width)];

    auto H = interpolate(tl, tr, c_diff);
    auto G = interpolate(bl, br, c_diff);

    return interpolate(G, H, r_diff);

}



bool loadTextureFromFile(TextureMap *texture, std::string file) {
    QImage myImage;
    if (!myImage.load(QString::fromStdString(file))) {
        std::cout<<"Failed to load in image"<<std::endl;
        return false;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    texture->width = myImage.width();
    texture->height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    texture->data.clear();
    texture->data.reserve(texture->width * texture->width);

    for (int i = 0; i < arr.size() / 4.f; i++){
        texture->data.push_back(glm::vec3(float(((std::uint8_t) arr[4*i]) / 255.f), float((std::uint8_t) arr[4*i+1] / 255.f), float((std::uint8_t) arr[4*i+2] / 255.f)));
    }

    return true;
}
