#include "realtime.h"


#define BLOCK_TEXTURE_PATH ":/resources/textures/block1.png"

void CopiedCity::InitializeBlockTexture() {

    // generate texture
    glGenTextures(1, &blockTexture);
    glBindTexture(GL_TEXTURE_2D, blockTexture);

    // obtain image from filepath
    auto textBlock = QImage(QString(BLOCK_TEXTURE_PATH));

    // format image to fit OpenGL
    textBlock = textBlock.convertToFormat(QImage::Format_RGBA8888).mirrored();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textBlock.width(),
                 textBlock.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textBlock.bits());

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

}
