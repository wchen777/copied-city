#version 330 core

// Create a UV coordinate in variable
in vec2 uv_coord;

// Add a sampler2D uniform
uniform sampler2D texture_samp;

// dimensions
uniform int height;
uniform int width;

// filter settings
uniform bool perPixel;
uniform bool kernelBased;
uniform bool perPixelExtra;
uniform bool kernelBasedExtra;
uniform bool perPixelExtra2;
uniform bool kernelBasedExtra2;

out vec4 fragColor;


void main()
{
    fragColor = vec4(0.f);

    // set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(texture_samp, uv_coord);

//    fragColor = vec4(uv_coord[0], uv_coord[1], 0.f, 0.f);


    if (perPixel) { // PER PIXEL INVERT FILTER
        fragColor = 1 - fragColor;
    }

    if (perPixelExtra) { // PER PIXEL EXTRA COLOR SWAP FILTER
        fragColor = vec4(0.56 * fragColor[1], 1.3 * fragColor[2], 0.9 * fragColor[0], 1.f);
    }

    if (perPixelExtra2) {
        fragColor *= 0.85;
    }

}
