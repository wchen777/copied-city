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

Realtime::Realtime(QWidget *parent)
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

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    // delete shaders
    glDeleteProgram(Realtime::shaderTexture);
    glDeleteProgram(Realtime::shaderRender);

    // delete fullscreen vao/vbo data
    glDeleteVertexArrays(1, &fullscreen_vao);
    glDeleteBuffers(1, &fullscreen_vbo);

    Realtime::DestroyMeshes();

    // destroy all buffers
    Realtime::DestroyBuffers(true);

    // delete the camera is the scene is initialized
    if (Realtime::isInitialized) {
        delete Realtime::sceneCamera;
    }

    // destroy FBO
    Realtime::DestroyFBO();

    this->doneCurrent();
}

void Realtime::initializeGL() {

    std::cout << "initialize" << std::endl;

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
    Realtime::screenHeight = size().height() * m_devicePixelRatio;
    Realtime::screenWidth = size().width() * m_devicePixelRatio;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, Realtime::screenWidth, Realtime::screenHeight);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    // initialize the shader
    Realtime::shaderRender = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    Realtime::shaderTexture = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
    // initialize the default FBO

    // Realtime::defaultFBO = 0;
    Realtime::defaultFBO = 2; // UNCOMMENT TO CHANGE DEFAULT FBO VALUE

    // setup the texture shader for post-processing effects
    Realtime::SetupTextureShader();

    // create FBO
    Realtime::MakeFBO();
}

void Realtime::paintGL() {

    // Students: anything requiring OpenGL calls every frame should be done here

    if (!Realtime::isInitialized) {
        return;
    }

    // if we have freshly loaded in a scene, initailize the buffers only once
    if (Realtime::changedScene) {

        // initilize all VBO and VAO data into the mesh objects
        Realtime::InitializeBuffers();
        Realtime::changedScene = false;
    }


    // if we have ran the ray tracer

        // otherwise, draw the realtime objects into the render buffer

        // set the current draw buffer to be render buffer
        Realtime::SetRenderFBO();

        // Bind the render shader
        glUseProgram(Realtime::shaderRender);

        // initialize uniforms, not per object
        Realtime::InitializeCameraUniforms();
        Realtime::InitializeLightUniforms();

        // initilialize uniforms per object, draw object
        Realtime::DrawBuffers();

        // unbind render shader
        glUseProgram(0);


    // draw the texture buffer (objects or ray tracer image) with post-processing effects, set uniforms as necessary
    Realtime::DrawTextureFBO();

    // Unbind the texture shader
    glUseProgram(0);

}

void Realtime::resizeGL(int w, int h) {

    Realtime::screenHeight = size().height() * m_devicePixelRatio;
    Realtime::screenWidth = size().width() * m_devicePixelRatio;

    // Tells OpenGL how big the screen is
    glViewport(0, 0, Realtime::screenWidth, Realtime::screenHeight);

    // if the scene isn't initialized yet
    if (!Realtime::isInitialized) {
        return;
    }

    // Students: anything requiring OpenGL calls when the program starts should be done here

    float aspectRatio = static_cast<float>(w) / static_cast<float>(h);

    if (Realtime::sceneCamera->getAspectRatio() != aspectRatio) {
        Realtime::sceneCamera->updateAspectRatio(aspectRatio);
    }

    // destroy old FBO
    Realtime::DestroyFBO();

    // destroy old FBO
    Realtime::MakeFBO();

//    // destroy old buffers
//    Realtime::DestroyBuffers();
//    // reinitialize them
//    Realtime::InitializeBuffers();

}

void Realtime::sceneChanged() {

    // destroy old meshes
    Realtime::DestroyMeshes();

    // destroy old meshes
    Realtime::DestroyBuffers(true);

    // delete the camera and old FBOs if the scene is initialized
    if (Realtime::isInitialized) {
        delete Realtime::sceneCamera;
    }

    // set current params (for on startup)
    Realtime::currentParam1 = settings.shapeParameter1;
    Realtime::currentParam2 = settings.shapeParameter2;

    SceneCameraData camData = {.pos=glm::vec4(0,0,16,1), .look=glm::vec4(0,0,-1,0), .up=glm::vec4(0,1,0,0), .heightAngle=0.863938, .aperture=0.008, .focalLength=3};
    Camera* cam = new Camera(camData, size().height(), size().width(), 100.0, 0.01);
    Realtime::sceneCamera = cam;

    // build each primitive into a composite struct that contains the class for the trimesh, etc.
    // apply it to the realtime class
    Realtime::CompilePrimitiveMeshes();

    Realtime::isInitialized = true;
    Realtime::changedScene = true;

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    // if the scene isn't initialized yet
    if (!Realtime::isInitialized) {
        return;
    }

    // near plane and far plane updates
    if (Realtime::sceneCamera->nearPlane != settings.nearPlane || Realtime::sceneCamera->farPlane != settings.farPlane) {

        Realtime::sceneCamera->updateViewPlanes(settings.farPlane, settings.nearPlane);
    }

    // updates for tesselation params
    if (settings.shapeParameter1 != Realtime::currentParam1 || settings.shapeParameter2 != Realtime::currentParam2) {

        // set current params (for on startup)
        Realtime::currentParam1 = settings.shapeParameter1;
        Realtime::currentParam2 = settings.shapeParameter2;

        // update tesselation parameters
        Realtime::UpdateTesselations();
        // destroy old buffers
        Realtime::DestroyBuffers(false);

        // create new buffers
        Realtime::InitializeBuffers();

    }

    // set the current filters
    Realtime::perPixelFilter = settings.perPixelFilter;
    Realtime::kernelBasedFilter = settings.kernelBasedFilter;
    Realtime::perPixelFilterExtra = settings.perPixelFilterExtra;
    Realtime::kernelBasedFilterExtra = settings.kernelBasedFilterExtra;

    update(); // asks for a PaintGL() call to occur
}

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        if (Realtime::sceneCamera == nullptr) {
            return;
        }

        Realtime::sceneCamera->RotateX(deltaX);
        Realtime::sceneCamera->RotateY(deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    // no camera -> return
    if (Realtime::sceneCamera == nullptr) {
        return;
    }

    // delta time returns time in seconds elapsed

    float speed = deltaTime * UNITS_PER_SECOND;

    glm::vec3 movement = glm::vec3(0.f);

    // W key pressed
    if (Realtime::m_keyMap[Qt::Key_W]) {
        movement += Realtime::sceneCamera->WPressed();
    }

    // A key pressed
    if (Realtime::m_keyMap[Qt::Key_A]) {
        movement += Realtime::sceneCamera->APressed();
    }

    // D key pressed
    if (Realtime::m_keyMap[Qt::Key_D]) {
        movement += Realtime::sceneCamera->DPressed();
    }

    // S key pressed
    if (Realtime::m_keyMap[Qt::Key_S]) {
        movement += Realtime::sceneCamera->SPressed();
    }

    // Space key pressed
    if (Realtime::m_keyMap[Qt::Key_Space]) {
        movement += Realtime::sceneCamera->SpacePressed();
    }

    // Ctrl key pressed
    if (Realtime::m_keyMap[Qt::Key_Control] || Realtime::m_keyMap[Qt::Key_Meta]) {
       movement += Realtime::sceneCamera->CtrlPressed();
    }

    if (glm::length(movement) != 0.f) {
        // apply the accumulated translation
        movement *= speed;
        Realtime::sceneCamera->ApplyTranslation(movement);
    }

    update(); // asks for a PaintGL() call to occur
}
