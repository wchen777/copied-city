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

// filters
uniform mat3 sharpen;
uniform mat3 laplacian;
uniform mat3 gradient_f;

out vec4 fragColor;

void convolve3x3(mat3 kernelFilter) {
    vec4 finalColor = vec4(0.f);

    float widthScale = 1.f / width;
    float heightScale = 1.f / height;

    float offsetHeight = -1.f * heightScale;
    for (int r = 0; r < 3; ++r) { // perform convolution
        float offsetWidth = -1.f * widthScale;
        for (int c = 0; c < 3; ++c) {
            finalColor += kernelFilter[r][c] * texture(texture_samp, vec2(uv_coord[0] + offsetWidth, uv_coord[1] + offsetHeight));
            offsetWidth += widthScale;
        }
        offsetHeight += heightScale;
    }

    fragColor = finalColor;
}



void main()
{
    fragColor = vec4(0.f);

    // set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(texture_samp, uv_coord);

//    fragColor = vec4(uv_coord[0], uv_coord[1], 0.f, 0.f);


    // HARD CODED VALUES FOR COMPILER OPTIMIZATIONS
    if (kernelBased) { // sharpen filter
        convolve3x3(sharpen);
    } else if (kernelBasedExtra) {
        convolve3x3(laplacian);
    } else if (kernelBasedExtra2) {
        convolve3x3(gradient_f);
    }

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
