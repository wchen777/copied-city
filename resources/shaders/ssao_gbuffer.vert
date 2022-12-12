#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

uniform mat4 view;
uniform mat4 inv_trans_model; // inv transpose CTM for obj norm -> world norm
uniform mat4 model;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main() {
    // need view space position for frag pos and normal
    FragPos = vec3(view * model * vec4(pos, 1.0));
    Normal = vec3(view * inv_trans_model * vec4(norm, 0.0));
    TexCoords = uv;
}
