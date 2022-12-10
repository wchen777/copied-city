#version 330 core


layout(location = 0) in vec3 vertex_pos_obj;
layout(location = 1) in vec3 vertex_norm_obj;
layout(location = 2) in vec2 uv;

uniform mat4 model_matrix; // CTM for obj -> world
uniform mat4 inv_trans_model_matrix; // inv transpose CTM for obj norm -> world norm
//uniform mat4 PV_matrix; // MVP matrix for obj -> clip space
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

out vec3 vertex_pos_world; // vertex world position
out vec3 vertex_norm_world; // vertex norm world position

out vec2 uv_coord;


void main() {

    // vertex world space position for lighting
    vertex_pos_world = vec3(model_matrix * vec4(vertex_pos_obj, 1.0));

    // vertex world space normal for lighting
    vertex_norm_world = vec3(inv_trans_model_matrix * vec4(vertex_norm_obj, 0.0));

//    gl_Position = vec4(vertex_pos_obj, 1.0);
    // set position on screen to be the object space position transformed to clip space
    gl_Position = (proj_matrix * view_matrix * model_matrix) * vec4(vertex_pos_obj, 1.0);

    // write uv coord to out
    uv_coord = uv;
}
