#include "realtime.h"
#define GL_SILENCE_DEPRECATION

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

#define UNITS_PER_SECOND 5.f

// ================== Project 5: Lights, Camera

CopiedCity::CopiedCity(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this

    m_keyMap[Qt::Key_Meta]   = false;
}

void CopiedCity::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    // delete shaders
    glDeleteProgram(CopiedCity::shaderTexture);
    glDeleteProgram(CopiedCity::shaderRender);

    // delete fullscreen vao/vbo data
    glDeleteVertexArrays(1, &fullscreen_vao);
    glDeleteBuffers(1, &fullscreen_vbo);

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    CopiedCity::DestroyMeshes();

    // destroy all buffers
    CopiedCity::DestroyBuffers(true);

    // delete the camera is the scene is initialized
    if (CopiedCity::isInitialized) {
        delete CopiedCity::sceneCamera;
    }

    // destroy FBO
    CopiedCity::DestroyFBO();

    this->doneCurrent();
}

void CopiedCity::initializeGL() {

//    std::cout << "initialize" << std::endl;

    m_devicePixelRatio = this->devicePixelRatio();
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);

    // resizing
    CopiedCity::screenHeight = size().height() * m_devicePixelRatio;
    CopiedCity::screenWidth = size().width() * m_devicePixelRatio;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, CopiedCity::screenWidth, CopiedCity::screenHeight);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    // initialize the shader
    CopiedCity::shaderRender = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    CopiedCity::shaderTexture = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    CopiedCity::shaderSky = ShaderLoader::createShaderProgram(":/resources/shaders/sky.vert", ":/resources/shaders/sky.frag");

    // initialize the default FBO
    // CopiedCity::defaultFBO = 0;
    CopiedCity::defaultFBO = 2; // UNCOMMENT TO CHANGE DEFAULT FBO VALUE

    // setup the texture shader for post-processing effects
    CopiedCity::SetupTextureShader();

    // create FBO
//    CopiedCity::MakeFBO();

    // setup city

    // set current params (for on startup)
    CopiedCity::currentParam1 = 10;
    CopiedCity::currentParam2 = 10;

    SceneCameraData camData = {.pos=glm::vec4(0,0,16,1), .look=glm::vec4(0,0,-1,0), .up=glm::vec4(0,1,0,0), .heightAngle=0.863938, .aperture=0.008, .focalLength=3};
    Camera* cam = new Camera(camData, size().height(), size().width(), 500.0, 0.01);
    CopiedCity::sceneCamera = cam;

    CopiedCity::GenerateCity(); // generate the city
    CopiedCity::InitializeBuffers(); // initialize buffers for the city

    // initialize sky stuff
    CopiedCity::InitializeSkyShader();
    CopiedCity::InitializeSkyBox();

}

void CopiedCity::paintGL() {

    // Students: anything requiring OpenGL calls every frame should be done here

//    if (!CopiedCity::isInitialized) {
//        return;
//    }

//    // if we have freshly loaded in a scene, initailize the buffers only once
//    if (CopiedCity::changedScene) {

//        // initilize all VBO and VAO data into the mesh objects
//        CopiedCity::InitializeBuffers();
//        CopiedCity::changedScene = false;
//    }

    // otherwise, draw the CopiedCity objects into the render buffer

    // set the current draw buffer to be render buffer
//    CopiedCity::SetRenderFBO();

    // Bind the render shader
    glUseProgram(CopiedCity::shaderRender);

    // initialize uniforms, not per object
    CopiedCity::InitializeCameraUniforms();
    CopiedCity::InitializeLightUniforms();

    // initilialize uniforms per object, draw object
    CopiedCity::DrawBuffers();
    glUseProgram(0);

    CopiedCity::RenderSkyBox();

    // unbind render shader
//    glUseProgram(0);




//    // draw the texture buffer (objects or ray tracer image) with post-processing effects, set uniforms as necessary
//    CopiedCity::DrawTextureFBO();

    // Unbind the texture shader
//    glUseProgram(0);

}

void CopiedCity::resizeGL(int w, int h) {

    CopiedCity::screenHeight = size().height() * m_devicePixelRatio;
    CopiedCity::screenWidth = size().width() * m_devicePixelRatio;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, CopiedCity::screenWidth, CopiedCity::screenHeight);

    // if the scene isn't initialized yet
    if (!CopiedCity::isInitialized) {
        return;
    }

    // Students: anything requiring OpenGL calls when the program starts should be done here

    float aspectRatio = static_cast<float>(w) / static_cast<float>(h);

    if (CopiedCity::sceneCamera->getAspectRatio() != aspectRatio) {
        CopiedCity::sceneCamera->updateAspectRatio(aspectRatio);
    }

    // destroy old FBO
    CopiedCity::DestroyFBO();

    // make new FBO
    CopiedCity::MakeFBO();

}

void CopiedCity::sceneChanged() {

    // destroy old meshes
    CopiedCity::DestroyMeshes();

    // destroy old buffers
    CopiedCity::DestroyBuffers(true);

    // delete the camera and old FBOs if the scene is initialized
    if (CopiedCity::isInitialized) {
        delete CopiedCity::sceneCamera;
    }

    // build each primitive into a composite struct that contains the class for the trimesh, etc.
    // apply it to the CopiedCity class
//    CopiedCity::CompilePrimitiveMeshes();

    CopiedCity::isInitialized = true;
    CopiedCity::changedScene = true;

    update(); // asks for a PaintGL() call to occur
}

void CopiedCity::settingsChanged() {

    // if the scene isn't initialized yet
    if (!CopiedCity::isInitialized) {
        return;
    }

    // near plane and far plane updates
    if (CopiedCity::sceneCamera->nearPlane != settings.nearPlane || CopiedCity::sceneCamera->farPlane != settings.farPlane) {

        CopiedCity::sceneCamera->updateViewPlanes(settings.farPlane, settings.nearPlane);
    }

    // updates for tesselation params
    if (settings.shapeParameter1 != CopiedCity::currentParam1 || settings.shapeParameter2 != CopiedCity::currentParam2) {

        // set current params (for on startup)
        CopiedCity::currentParam1 = settings.shapeParameter1;
        CopiedCity::currentParam2 = settings.shapeParameter2;

        // update tesselation parameters
        CopiedCity::UpdateTesselations();
        // destroy old buffers
        CopiedCity::DestroyBuffers(false);

        // create new buffers
        CopiedCity::InitializeBuffers();

    }

    // set the current filters
    CopiedCity::perPixelFilter = settings.perPixelFilter;
    CopiedCity::kernelBasedFilter = settings.kernelBasedFilter;
    CopiedCity::perPixelFilterExtra = settings.perPixelFilterExtra;
    CopiedCity::kernelBasedFilterExtra = settings.kernelBasedFilterExtra;

    update(); // asks for a PaintGL() call to occur
}

void CopiedCity::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void CopiedCity::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void CopiedCity::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void CopiedCity::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void CopiedCity::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        if (CopiedCity::sceneCamera == nullptr) {
            return;
        }

        CopiedCity::sceneCamera->RotateX(deltaX);
        CopiedCity::sceneCamera->RotateY(deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void CopiedCity::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    // no camera -> return
    if (CopiedCity::sceneCamera == nullptr) {
        return;
    }

    // delta time returns time in seconds elapsed

    float speed = deltaTime * UNITS_PER_SECOND;

    glm::vec3 movement = glm::vec3(0.f);

    // W key pressed
    if (CopiedCity::m_keyMap[Qt::Key_W]) {
        movement += CopiedCity::sceneCamera->WPressed();
    }

//    // A key pressed
//    if (CopiedCity::m_keyMap[Qt::Key_A]) {
//        movement += CopiedCity::sceneCamera->APressed();
//    }

//    // D key pressed
//    if (CopiedCity::m_keyMap[Qt::Key_D]) {
//        movement += CopiedCity::sceneCamera->DPressed();
//    }

//    // S key pressed
    if (CopiedCity::m_keyMap[Qt::Key_S]) {
        movement += CopiedCity::sceneCamera->SPressed();
    }

//    // Space key pressed
//    if (CopiedCity::m_keyMap[Qt::Key_Space]) {
//        movement += CopiedCity::sceneCamera->SpacePressed();
//    }

//    // Ctrl key pressed
//    if (CopiedCity::m_keyMap[Qt::Key_Control] || CopiedCity::m_keyMap[Qt::Key_Meta]) {
//       movement += CopiedCity::sceneCamera->CtrlPressed();
//    }

    if (glm::length(movement) != 0.f) {
        // apply the accumulated translation
        movement *= speed;
        CopiedCity::sceneCamera->ApplyTranslation(movement);
    }

    update(); // asks for a PaintGL() call to occur
}
