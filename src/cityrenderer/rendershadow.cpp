#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "realtime.h"
#include <iostream>
#include <ostream>
#include <map>

void CopiedCity::InitializeShadow() {

    glGenFramebuffers(1, &(CopiedCity::depthMapFBO));

    glGenTextures(1, &(CopiedCity::depthMap));
    glBindTexture(GL_TEXTURE_2D, CopiedCity::depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


    glBindFramebuffer(GL_FRAMEBUFFER, (CopiedCity::depthMapFBO));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);
}


void CopiedCity::SetShadowFBO() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, (CopiedCity::depthMapFBO));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

}

void CopiedCity::RenderLightDepthFBO(){

    glEnable(GL_DEPTH_TEST);
    glUseProgram(CopiedCity::shaderDepth);
    glm::vec3 lightPos(50.f, 10.f, -150.f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 lightProjection, lightView;

//    float near_plane = 1.0f, far_plane = 400.f;

    // look
    // -0.417944, -0.150892, -0.895852
    // pos
    // 61.7784, 39.1123, 35.9149

//    lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f,  400.0f, 0.01f);
    lightProjection = glm::perspective(50.f, CopiedCity::sceneCamera->aspectRatio, 0.1f, 400.f);
//    lightView = glm::lookAt(glm::vec3(-0.7f,-1.f, 0.5f), lightPos, CopiedCity::sceneCamera->up); // this is wrong for now
    lightView = glm::lookAt(glm::vec3(50.f,25.f,0.f), glm::vec3(50.f,25.f,0.f) + glm::vec3(-0.65f,-1.f, 0.5f), CopiedCity::sceneCamera->up); // this is wrong for now

    CopiedCity:lightSpaceMatrix = lightProjection * lightView;

//    CopiedCity:lightSpaceMatrix = CopiedCity::sceneCamera->getProjViewMatrix();

    // render scene from light's point of view

    GLint lsm = glGetUniformLocation(CopiedCity::shaderDepth, "lightSpaceMatrix");
    glUniformMatrix4fv(lsm, 1, GL_FALSE, &(CopiedCity::lightSpaceMatrix[0][0]));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//    glViewport(0, 0, CopiedCity::screenWidth, CopiedCity::screenHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, (CopiedCity::depthMapFBO));

    glClear(GL_DEPTH_BUFFER_BIT);

    for (CityMeshObject* mesh : CopiedCity::city.cityData) {
        glBindVertexArray(mesh->vao);

        // pass in model matrix as a uniform
        GLint model_mat_loc = glGetUniformLocation(CopiedCity::shaderDepth, "model");
        glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, &mesh->modelMatrix[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, CopiedCity::city.cube->m_vertexData.size() / 8);
        // Unbind Vertex Array
        glBindVertexArray(0);

    }

    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);
//    glDepthFunc(GL_LESS);
    glUseProgram(0);
}

