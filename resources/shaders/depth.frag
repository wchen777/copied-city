#version 330 core

out vec4 output_color;

// TODO: pass this in as uniform in both cases
float near = 0.1;
float far  = 400.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
     gl_FragDepth = LinearizeDepth(gl_FragCoord.z) / far;
//     output_color = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, 1.f);
}
