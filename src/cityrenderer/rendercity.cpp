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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    // Enable and define attribute 1 to store vertex normals (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

    // Enable and define attribute 2 to store uv coordinate (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

    // Clean-up bindings for VAO
    glBindVertexArray(0);


    for (CityMeshObject& m : CopiedCity::city.leftFacade.data) {
        // Generate, and bind VAO
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Enable and define attribute 2 to store uv coordinate (vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Enable and define attribute 2 to store uv coordinate (vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

        // Clean-up bindings for VAO
        glBindVertexArray(0);

        // add to city data (should this be here?)
        CopiedCity::city.cityData.emplace_back(&m);
    }

    for (CityMeshObject& m : CopiedCity::city.backFacade.data) {
        // Generate, and bind VAO
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Enable and define attribute 2 to store uv coordinate (vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

        // Clean-up bindings for VAO
        glBindVertexArray(0);

        // add to city data (should this be here?)
        CopiedCity::city.cityData.emplace_back(&m);
    }

    // cube growths
    for (CityMeshObject& m : CopiedCity::city.plane.planeProtrusions) {
        // Generate, and bind VAO
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        // Enable and define attribute 0 to store vertex positions (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));

        // Enable and define attribute 2 to store uv coordinate (vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));

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

}

/*
 * initialize the uniform associated with the camera in the scene that are separate from each individual object
*/
void CopiedCity::InitializeCameraUniforms() {

    GLint P_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "proj_matrix");
    glUniformMatrix4fv(P_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getProjMatrix()[0][0]);


    GLint PV_mat_loc = glGetUniformLocation(CopiedCity::shaderRender, "view_matrix");
    glUniformMatrix4fv(PV_mat_loc, 1, GL_FALSE, &CopiedCity::sceneCamera->getViewMatrix()[0][0]);

    GLint cam_pos_loc = glGetUniformLocation(CopiedCity::shaderRender, "cam_pos");
    glUniform3fv(cam_pos_loc, 1, &CopiedCity::sceneCamera->pos[0]);

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

    // bind depth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // pass in shadow map sampler
    glUniform1i(glGetUniformLocation(CopiedCity::shaderRender, "shadowMap"), 0);

    // bind the block texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, CopiedCity::blockTexture);

    // pass in the texture sampler uniform for block
    glUniform1i(glGetUniformLocation(CopiedCity::shaderRender, "block_texture"), 1);

    // add extra SSAO texture to lighting pass
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

    // pass in ambient occlusion map
    glUniform1i(glGetUniformLocation(CopiedCity::shaderRender, "ambient_occ"), 2);

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
        glDrawArrays(GL_TRIANGLES, 0, CopiedCity::city.cube->m_vertexData.size() / 8);

        // light space matrix uniform
        GLint lsm = glGetUniformLocation(CopiedCity::shaderRender, "lightSpaceMatrix");
        glUniformMatrix4fv(lsm, 1, GL_FALSE, &(CopiedCity::lightSpaceMatrix[0][0]));

        // Unbind Vertex Array
        glBindVertexArray(0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

}
