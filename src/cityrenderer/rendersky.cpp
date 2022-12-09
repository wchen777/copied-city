#include "realtime.h"
//#include "settings.h"
#include <iostream>
#include <ostream>
#include <map>

#define SKY_WHITE glm::vec3(0.871f, 0.859f, 0.855f)
#define SKY_TEXTURE_PATH ":/resources/textures/sky.png"

void CopiedCity::RenderSkyBox() {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(CopiedCity::shaderSky);

    // ... set view and projection matrix
    GLint P_mat_loc = glGetUniformLocation(CopiedCity::shaderSky, "projection");
    glUniformMatrix4fv(P_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getProjMatrix()[0][0]);

    GLint V_mat_loc = glGetUniformLocation(CopiedCity::shaderSky, "view");
    auto adjustedView = glm::mat4(glm::mat3(CopiedCity::sceneCamera->getViewMatrix()));
    glUniformMatrix4fv(V_mat_loc, 1, GL_FALSE, &adjustedView[0][0]);

    glBindVertexArray(skyboxVAO);

    std::cout << "skybox VAO" << skyboxVAO << std::endl;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CopiedCity::skyTextureID);

    std::cout << "skytexture" << CopiedCity::skyTextureID << std::endl;

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glUseProgram(0);
}


void CopiedCity::InitializeSkyBox() {
    glGenTextures(1, &skyTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureID);

    // Prepare filepath
    QString filepath = QString(SKY_TEXTURE_PATH);

    // obtain image from filepath
    auto textSky = QImage(filepath);

    // format image to fit OpenGL
    textSky = textSky.convertToFormat(QImage::Format_RGBA8888).mirrored();

    for (unsigned int i = 0; i < 6; i++)
    {

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, textSky.width(),
                     textSky.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textSky.bits());
    }


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO and VBO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CopiedCity::InitializeSkyShader() {
    glUseProgram(CopiedCity::shaderSky);
    // bind, create cube sampler, unbind
    GLint samplerLoc = glGetUniformLocation(CopiedCity::shaderSky, "skybox");
    glUniform1i(samplerLoc, 0);
    glUseProgram(0);
}
