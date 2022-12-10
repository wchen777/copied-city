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

    for (CityMeshObject& m : CopiedCity::city.leftFacade.data) {
        // Generate, and bind VAO
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Clean-up bindings for VAO
        glBindVertexArray(0);

        // add to city data (should this be here?)
        CopiedCity::city.cityData.emplace_back(&m);
    }

    for (CityMeshObject& m : CopiedCity::city.rightFacade.data) {
        // Generate, and bind VAO
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Clean-up bindings for VAO
        glBindVertexArray(0);

        // add to city data (should this be here?)
        CopiedCity::city.cityData.emplace_back(&m);
    }

    // clean up VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
 * initialize the uniform associated with the lights in the scene that are separate from each individual object
*/
void CopiedCity::InitializeLightUniforms() {
//    this->makeCurrent();

    // GLOBAL WEIGHTS

    // pass m_ka into the fragment shader as a uniform
    GLint ka_loc = glGetUniformLocation(CopiedCity::shaderRender, "k_a");
    glUniform1f(ka_loc, CopiedCity::city.globals.ka);

    // pass m_kd into the fragment shader as a uniform
    GLint kd_loc = glGetUniformLocation(CopiedCity::shaderRender, "k_d");
    glUniform1f(kd_loc, CopiedCity::city.globals.kd);

    // pass k_s as a uniform
    GLint ks_loc = glGetUniformLocation(CopiedCity::shaderRender, "k_s");
    glUniform1f(ks_loc, CopiedCity::city.globals.ks);

    // add a small point light at the camera
    SceneLightData camLight = {.id = 0, .type = LightType::LIGHT_POINT, .color = SceneColor(0.8f,0.8f,0.8f,1.f),
                                       .function = glm::vec3(0.f,0.2f,0.2f), .pos = glm::vec4(CopiedCity::sceneCamera->pos, 1.f),
                                        .dir = glm::vec4(0.f),
                                       .penumbra = 0.f, .angle = 0.f, .width =0, .height = 0};

    CopiedCity::city.lights.emplace_back(camLight);

    GLint samplerLoc = glGetUniformLocation(CopiedCity::shaderRender, "shadowMap");
    glUniform1i(samplerLoc, 0);

    GLint lsm = glGetUniformLocation(CopiedCity::shaderRender, "lightSpaceMatrix");
    glUniformMatrix4fv(lsm, 1, GL_FALSE, &(CopiedCity::lightSpaceMatrix[0][0]));

    int count = 0;
    // for each light data, up until 8
    for (auto& light : CopiedCity::city.lights) {
        if (count == MAX_LIGHTS) {
            break;
        }

        // light pos uniform
        GLint light_pos_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_positions[" + std::to_string(count) + "]").c_str());
        glUniform3fv(light_pos_loc, 1, &light.pos[0]);

        // light dir uniform
        GLint light_dir_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_dirs[" + std::to_string(count) + "]").c_str());
        glUniform3fv(light_dir_loc, 1, &light.dir[0]);

        // light color uniform
        GLint light_colors_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_colors[" + std::to_string(count) + "]").c_str());
        glUniform3fv(light_colors_loc, 1, &light.color[0]);

        // light attenuation functions uniform
        GLint light_functions_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_functions[" + std::to_string(count) + "]").c_str());
        glUniform3fv(light_functions_loc, 1, &light.function[0]);

        // light angle uniform
        GLint light_angles_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_angles[" + std::to_string(count) + "]").c_str());
        glUniform1f(light_angles_loc, light.angle);

        // light penumbra uniform
        GLint light_penumbras_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_penumbras[" + std::to_string(count) + "]").c_str());
        glUniform1f(light_penumbras_loc, light.penumbra);

        // light type uniform
        GLint light_type_loc = glGetUniformLocation(CopiedCity::shaderRender, ("light_types[" + std::to_string(count) + "]").c_str());
        glUniform1i(light_type_loc, static_cast<std::underlying_type_t<LightType>>(light.type));

        count++;
    }

    // num lights uniform
    GLint num_lights_loc = glGetUniformLocation(CopiedCity::shaderRender, "num_lights");
    glUniform1i(num_lights_loc, count);

    // remove camera light
    CopiedCity::city.lights.pop_back();

//    this->doneCurrent();
}

/*
 * initialize the uniform associated with the camera in the scene that are separate from each individual object
*/
void CopiedCity::InitializeCameraUniforms() {
//    this->makeCurrent();

//     pass in VP matrix as a uniform (VP is already calculated in camera)
//    std::cout << CopiedCity::sceneCamera->getViewProjMatrix()[0][0] << std::endl;

//    GLint PV_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "PV_matrix");
//    glUniformMatrix4fv(PV_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getProjViewMatrix()[0][0]);

    GLint P_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "proj_matrix");
    glUniformMatrix4fv(P_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getProjMatrix()[0][0]);


    GLint PV_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "view_matrix");
    glUniformMatrix4fv(PV_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getViewMatrix()[0][0]);

    GLint cam_pos_loc = glGetUniformLocation(CopiedCity::shaderRender, "cam_pos");
    glUniform3fv(cam_pos_loc, 1, &CopiedCity::sceneCamera->pos[0]);

//    this->doneCurrent();
}


/*
 * go through each buffer and destroy them. clean up meshes only if we are exiting the scene
*/
void CopiedCity::DestroyBuffers(bool isExit) {
    // destroy old buffers
    for (MeshPrimitive& mesh : CopiedCity::objectMeshes) {
        // if we are exiting, always clean up
        // otherwise, only cleanup the non-obj mesh primitives
        if (isExit || mesh.type != PrimitiveType::PRIMITIVE_MESH) {
            glDeleteBuffers(1, &mesh.vbo);
            glDeleteVertexArrays(1, &mesh.vao);
        }
    }
}


/*
 * go through each mesh object struct and draw them on the screen, initializing the correct uniform variables (per object)
*/
void CopiedCity::DrawBuffers() {

    // initialize uniforms, draw the object
    for (CityMeshObject* mesh : CopiedCity::city.cityData) {

        // bind the object's vao
        glBindVertexArray(mesh->vao);

        // pass in model matrix as a uniform
        GLint model_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "model_matrix");
        glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, &mesh->modelMatrix[0][0]);

        // pass in inv transpose model matrix as a uniform
        GLint inv_transpose_model_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "inv_trans_model_matrix");
        glUniformMatrix4fv(inv_transpose_model_mat_loc, 1, GL_FALSE, &mesh->invTransposeModelMatrix[0][0]);

        // pass in shininess
        GLint shininess_loc = glGetUniformLocation(CopiedCity::shaderRender, "shininess");
        glUniform1f(shininess_loc, mesh->material.shininess);

        // pass in cAmbient
        GLint cAmbient_loc = glGetUniformLocation(CopiedCity::shaderRender, "cAmbient");
        glUniform3fv(cAmbient_loc, 1, &mesh->material.cAmbient[0]);

        // pass in cDiffuse
        GLint cDiffuse_loc = glGetUniformLocation(CopiedCity::shaderRender, "cDiffuse");
        glUniform3fv(cDiffuse_loc, 1, &mesh->material.cDiffuse[0]);

        // pass in cSpecular
        GLint cSpecular_loc = glGetUniformLocation(CopiedCity::shaderRender, "cSpecular");
        glUniform3fv(cSpecular_loc, 1, &mesh->material.cSpecular[0]);

        // draw command for this object
        glDrawArrays(GL_TRIANGLES, 0, CopiedCity::city.cube->m_vertexData.size() / 6);
        // Unbind Vertex Array
        glBindVertexArray(0);
    }

}



void CopiedCity::MakeFBO() {

    // generate texture buffer as our output buffer
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &(CopiedCity::fbo_texturebuffer));
    // bind it
    glBindTexture(GL_TEXTURE_2D, CopiedCity::fbo_texturebuffer);

//    std::cout << "after tbuf" << CopiedCity::fbo_texturebuffer << std::endl;

    // set format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CopiedCity::screenWidth, CopiedCity::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    // -------------------------------

    // generate and bind renderbuffer
    glGenRenderbuffers(1, &fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_renderbuffer);

    // set format
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, CopiedCity::screenWidth, CopiedCity::screenHeight);

    // unbind
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CopiedCity::fbo_texturebuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);

}

void CopiedCity::DestroyFBO() {

//    this->makeCurrent();

    glDeleteTextures(1, &fbo_renderbuffer);
    glDeleteTextures(1, &fbo_texturebuffer);
    glDeleteTextures(1, &fbo);

//    this->doneCurrent();
}


/*
 * sets the framebuffer to be drawn upon
*/
void CopiedCity::SetRenderFBO() {


//    this->makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind current framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::fbo);

    // adjust viewport
    glViewport(0, 0, CopiedCity::screenWidth, CopiedCity::screenHeight);

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    this->doneCurrent();
}


/*
    draw to the viewport the data stored in the render buffer using the texture shader
*/
void CopiedCity::DrawTextureFBO() {

//    this->makeCurrent();

    // bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, CopiedCity::defaultFBO);

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the shader to be the texture shader
    glUseProgram(CopiedCity::shaderTexture);

    // set uniforms depending on the current settings

//    std::cout << CopiedCity::perPixelFilter << std::endl;

    // pass in the uniforms for the filters
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "perPixel"), CopiedCity::perPixelFilter);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "kernelBased"), CopiedCity::kernelBasedFilter);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "perPixelExtra"), CopiedCity::perPixelFilterExtra);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "kernelBasedExtra"), CopiedCity::kernelBasedFilterExtra);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "perPixelExtra2"), settings.extraCredit2);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "kernelBasedExtra2"), settings.extraCredit3);

    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "height"), CopiedCity::screenHeight);
    glUniform1i(glGetUniformLocation(CopiedCity::shaderTexture, "width"), CopiedCity::screenWidth);

    // bind the fullscreen quad
    glBindVertexArray(CopiedCity::fullscreen_vao);

//    std::cout << fbo_texturebuffer << std::endl;

    // draw the texture FBO
    glActiveTexture(GL_TEXTURE0); // set active texture to slot 0

    // bind active texture

      glBindTexture(GL_TEXTURE_2D, CopiedCity::fbo_texturebuffer);


//    std::cout << fbo_texturebuffer << std::endl;

    // draw to screen
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // unbindings
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

//    this->doneCurrent();
}


/*
 * bind and create necessary data for the texture shader
 */
void CopiedCity::SetupTextureShader() {

//    this->makeCurrent();

    glUseProgram(CopiedCity::shaderTexture);
    // bind, create 2d sampler, unbind
    GLint samplerLoc = glGetUniformLocation(CopiedCity::shaderTexture, "texture_samp");
    glUniform1i(samplerLoc, 0);
    glUseProgram(0);

    // full screen quad
    std::vector<GLfloat> fullscreen_quad_data =
    { //     POSITIONS    //
        -1.f,  1.f, 0.0f, // top left
         0.f, 1.f,
        -1.f, -1.f, 0.0f, // bottom left
         0.f, 0.f,
         1.f, -1.f, 0.0f, // bottom right
         1.f, 0.f,
         1.f,  1.f, 0.0f, // top right
         1.f, 1.f,
        -1.f,  1.f, 0.0f, // top left
         0.f, 1.f,
         1.f, -1.f, 0.0f, // bottom right
         1.f, 0.f,
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &fullscreen_vao);
    glBindVertexArray(fullscreen_vao);

    // add attributes for VAO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    this->doneCurrent();
}

