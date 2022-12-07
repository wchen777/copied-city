#pragma once

#include "scenedata.h"
#include <vector>
#include <string>

// Struct which contains data for a single primitive, to be used for rendering

// Struct which contains all the data needed to render a scene

class SceneParser {
public:
    // Parse the scene and store the results in renderData.
    // @param filepath    The path of the scene file to load.
    // @param renderData  On return, this will contain the metadata of the loaded scene.
    // @return            A boolean value indicating whether the parse was successful.
    static bool parse(std::string filepath, RenderData &renderData);
};

