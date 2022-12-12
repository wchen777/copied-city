#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera/camera.h"
#include "renderer/primitive.h"
#include "utils/scenedata.h"
#include "citygenerator/generator.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include <random>

class CopiedCity : public QOpenGLWidget
{
public:
    CopiedCity(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

    int screenWidth;
    int screenHeight;

    // necessary for scene and views
    RenderData sceneRenderData;
    Camera* sceneCamera = NULL;
    GLuint shaderRender;
    GLuint shaderTexture;
    GLuint shaderDepth;

    // mesh related fields
    void CompilePrimitiveMeshes();
    void UpdateTesselations();
    void DestroyMeshes();
    std::vector<MeshPrimitive> objectMeshes; // for old object types

    // tesselation params
    int currentParam1;
    int currentParam2;

    // VBO/VAO, OpenGL stuff
    void InitializeBuffers();
    void InitializeLightUniforms();
    void InitializeCameraUniforms();
    void DrawBuffers();
    void DestroyBuffers(bool isExit);

    bool isInitialized = false;
    bool changedScene = false;

    // extra credit
    float GetParamMultiple(glm::mat4 ctm);

    // FBO
    GLuint defaultFBO;
    GLuint fbo;
    GLuint fbo_renderbuffer;
    GLuint fbo_texturebuffer;
    GLuint m_ray_texture;

    // texture shader stuff
    GLuint fullscreen_vbo;
    GLuint fullscreen_vao;
    void SetupTextureShader();

    // COPIED CITY STUFF
    GLuint cubeVBO;
    void GenerateCity();
    CopiedCityData city;

    // sky
    void RenderSkyBox();
    void InitializeSkyShader();
    void InitializeSkyBox();
    GLuint skyTextureID;
    GLuint shaderSky;
    GLuint skyboxVAO, skyboxVBO;

    // shadow
    void InitializeShadow();
    void SetShadowFBO();
    void RenderLightDepthFBO();
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightSpaceMatrix;

    // block texture
    GLuint blockTexture;
    void InitializeBlockTexture();

    // SSAO

    // gBuffer
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedo;

    GLuint ssaoDepth;
    GLuint ssaoFBO, ssaoBlurFBO;
    GLuint ssaoColorBuffer, ssaoColorBufferBlur;

    GLuint shaderSSAO;
    GLuint shaderSSAOBlur;
    GLuint shaderSSAOGBuffer;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;

    void InitializeGBuffer();
    void InitializeDepthBuffer();
    void InitializeSSAOBuffer();

    void InitializeSampleAndNoise();
    void GenerateSampleKernel(std::uniform_real_distribution<GLfloat>& randomFloats,
                              std::default_random_engine& generator);
    void GenerateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats,
                              std::default_random_engine& generator);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
};
