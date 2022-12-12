#include "realtime.h"


inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

#define SAMPLES 16
#define SAMPLES_F 16.f

void CopiedCity::InitializeGBuffer() {
      glGenFramebuffers(1, &gBuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

      // position color buffer
      glGenTextures(1, &gPosition);
      glBindTexture(GL_TEXTURE_2D, gPosition);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      // normal color buffer
      glGenTextures(1, &gNormal);
      glBindTexture(GL_TEXTURE_2D, gNormal);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
      GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
      glDrawBuffers(2, attachments);

      glGenRenderbuffers(1, &ssaoDepth);
      glBindRenderbuffer(GL_RENDERBUFFER, ssaoDepth);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, CopiedCity::screenWidth, CopiedCity::screenHeight);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ssaoDepth);

      // finally check if framebuffer is complete
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          std::cout << "Framebuffer not complete!" << std::endl;

      glBindTexture(GL_TEXTURE_2D, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);
}


void CopiedCity::InitializeSSAOBuffer(){
    // generate FBOs
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    // SSAO color buffer, attach to its FBO
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    // and blur stage, attach to blur FBO
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void CopiedCity::InitializeSampleAndNoise() {
    // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    CopiedCity::GenerateSampleKernel(randomFloats, generator);
    CopiedCity::GenerateNoiseTexture(randomFloats, generator);
}

void CopiedCity::GenerateSampleKernel(std::uniform_real_distribution<GLfloat>& randomFloats,
                                      std::default_random_engine& generator) {

    for (unsigned int i = 0; i < SAMPLES; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / SAMPLES_F;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        CopiedCity::ssaoKernel.push_back(sample);
    }
}

void CopiedCity::GenerateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats,
                                      std::default_random_engine& generator) {
    for (unsigned int i = 0; i < 16; i++)
       {
           glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
           ssaoNoise.push_back(noise);
       }

       glGenTextures(1, &noiseTexture);
       glBindTexture(GL_TEXTURE_2D, noiseTexture);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void CopiedCity::InitializeSSAOShaders() {
    glUseProgram(CopiedCity::shaderSSAO);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAO, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAO, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAO, "texNoise"), 2);
    glUseProgram(0);

    glUseProgram(CopiedCity::shaderSSAOBlur);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAOBlur, "ssaoInput"), 0);
    glUseProgram(0);
}

void CopiedCity::RenderGeometryPass(){
    // bind geometry buffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // bind geomerty shader
    glUseProgram(CopiedCity::shaderSSAOGBuffer);

    for (CityMeshObject* mesh : CopiedCity::city.cityData) {
        glBindVertexArray(mesh->vao);

        // pass in model matrix as a uniform
        glUniformMatrix4fv(glGetUniformLocation(CopiedCity::shaderSSAOGBuffer, "model"), 1, GL_FALSE, &mesh->modelMatrix[0][0]);

        // pass in view matrix
        glUniformMatrix4fv(glGetUniformLocation(CopiedCity::shaderSSAOGBuffer, "view"), 1, GL_FALSE, &sceneCamera->viewMatrix[0][0]);

        // pass in projection matrix
         glUniformMatrix4fv(glGetUniformLocation(CopiedCity::shaderSSAOGBuffer, "projection"), 1, GL_FALSE, &sceneCamera->projMatrix[0][0]);

         // draw
        glDrawArrays(GL_TRIANGLES, 0, CopiedCity::city.cube->m_vertexData.size() / 8);

        // Unbind Vertex Array
        glBindVertexArray(0);
    }
    // unbind g buffer shader
    glUseProgram(0);

    // unbind geometry FBO
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);
}

void CopiedCity::RenderSSAOTexture() {
    // bind SSAO FBO
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    // clear just the color
    glClear(GL_COLOR_BUFFER_BIT);

    // bind ssao shader
    glUseProgram(CopiedCity::shaderSSAO);

    // Send kernel + rotation
    for (unsigned int i = 0; i < SAMPLES; ++i) {
        glUniform3fv(glGetUniformLocation(CopiedCity::shaderSSAO, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
    }

    // pass in projection
    glUniformMatrix4fv(glGetUniformLocation(CopiedCity::shaderSSAO, "projection"), 1, GL_FALSE, &sceneCamera->projMatrix[0][0]);

    // pass in screen width and height
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAOBlur, "screenHeight"), CopiedCity::screenHeight);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderSSAOBlur, "screenWidth"), CopiedCity::screenWidth);


    // pass in textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    // render the quad
    CopiedCity::RenderQuad();

    // unbind SSAO FBO
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);

    // unbind ssao shader
    glUseProgram(0);
}

void CopiedCity::RenderSSAOBlur() {
    // bind the blur FBO
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    // clear color
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(CopiedCity::shaderSSAOBlur);

    // pass in the color buffer texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    // draw it to the FBO render buffer
    CopiedCity::RenderQuad();

    // unbind shader
    glUseProgram(0);

    // unbind
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);
}


void CopiedCity::InitializeQuad() {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    // bind VAO and VBO
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CopiedCity::RenderQuad() {
    glViewport(0, 0, CopiedCity::screenWidth, CopiedCity::screenHeight);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

