#version 330 core

// Create a UV coordinate in variable
in vec2 uv_coord;

// Add a sampler2D uniform
uniform sampler2D texture_samp;

out vec4 fragColor;

void main()
{
    // set fragColor using the sampler2D at the UV coordinate
    vec4 c = texture(texture_samp, uv_coord);
//    fragColor = vec4(c.r, c.g, c.b, 1.f);
    fragColor = vec4(c.x, c.x, c.x, 1.f);
}
