#pragma once

#include <QImage>
#include <QtCore>

#include "raytracer/raytracer.h"
#include "raytracer/raytracescene.h"
#include "utils/sceneparser.h"


void RunRayTracer(int width, int height, RenderData& renderData, RGBA* data, Camera& camera) {

    // Setting up the raytracer
    RayTracer::Config rtConfig{};
    rtConfig.enableShadow        = true;
    rtConfig.enableReflection    = true;
    rtConfig.enableRefraction    = true;
    rtConfig.enableTextureMap    = true;
    rtConfig.enableTextureFilter = true;
    rtConfig.enableParallelism   = true;
    rtConfig.enableSuperSample   = true;
    rtConfig.enableAcceleration  = true;
    rtConfig.enableDepthOfField  = true;

    RayTracer raytracer{ rtConfig };

    RayTraceScene rtScene{ width, height, renderData };

    // set the current camera
    rtScene.setCamera(camera);

    raytracer.render(data, rtScene);

}
