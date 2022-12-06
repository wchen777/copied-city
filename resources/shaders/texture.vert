#version 330 core

// Task 15: add a second layout variable representing a UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

// create an "out" variable representing a UV coordinate
out vec2 uv_coord;

void main() {
    // assign the UV layout variable to the UV "out" variable
    gl_Position = vec4(position, 1.0);
    uv_coord = uv;
}
