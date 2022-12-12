#include "realtime.h"


inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}


void CopiedCity::InitializeGBuffer() {
      glGenFramebuffers(1, &gBuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

      // position color buffer
      glGenTextures(1, &gPosition);
      glBindTexture(GL_TEXTURE_2D, gPosition);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

      // normal color buffer
      glGenTextures(1, &gNormal);
      glBindTexture(GL_TEXTURE_2D, gNormal);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

      // color + specular color buffer
      glGenTextures(1, &gAlbedo);
      glBindTexture(GL_TEXTURE_2D, gAlbedo);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

      // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
      unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, attachments);
}


void CopiedCity::InitializeDepthBuffer(){
    glGenRenderbuffers(1, &ssaoDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, ssaoDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, CopiedCity::screenWidth, CopiedCity::screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ssaoDepth);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CopiedCity::InitializeSSAOBuffer(){
    // generate FBOs
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CopiedCity::InitializeSampleAndNoise() {
    // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;


}

void CopiedCity::GenerateSampleKernel(std::uniform_real_distribution<GLfloat>& randomFloats,
                                      std::default_random_engine& generator) {

    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

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
       unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
       glBindTexture(GL_TEXTURE_2D, noiseTexture);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
