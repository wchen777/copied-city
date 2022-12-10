#version 330 core

in vec3 vertex_pos_world; // from the vertex shader
in vec3 vertex_norm_world; // from the vertex shader
in vec4 fragPosLightSpace;
in vec2 uv_coord; // from vertex shader

out vec4 output_color; // total illumination output color

// TODO: light sources
uniform int num_lights;
uniform int light_types[8]; // light types -> point=0, directional=1, spot=2
uniform vec3 light_dirs[8]; // light directions
uniform vec3 light_positions[8]; // light positions
uniform vec3 light_colors[8]; // light intensities

uniform vec3 light_functions[8]; // light attenuation
uniform float light_angles[8]; // light angles
uniform float light_penumbras[8]; // only for spot

uniform sampler2D shadowMap;


// for ambient lighting
uniform float k_a;
uniform vec3 cAmbient;

// for diffuse lighting
uniform float k_d;
uniform vec3 cDiffuse;

// for specular lighting
uniform float shininess;
uniform float k_s;
uniform vec3 cam_pos;
uniform vec3 cSpecular;

// for block texture
uniform sampler2D block_texture;

// fog attentuation
float getFogFactor(float d)
{
    const float FogMax = 250.0;
    const float FogMin = 20.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

// falloff for spot lights
float falloffFactor(float angle, float innerA, float outerA) {
    float t = (angle - innerA) / (outerA - innerA);
    return -2 * pow(t, 3) + 3 * pow(t, 2);
}

// ambient
vec3 Ambient() {
    return k_a * cAmbient;
}

// lambertian/diffuse
vec3 Diffuse(float NdotL) {
    vec3 diffColor = k_d * NdotL * cDiffuse;
    vec3 texColor = vec3(NdotL * texture(block_texture, uv_coord));
    return diffColor + (0.7 * (texColor - diffColor));
}

// specular
vec3 Specular(float RdotV) {
    if (shininess > 0) {
        return k_s * pow(RdotV, shininess) * cSpecular;
    } else {
        return k_s * RdotV * cSpecular;
    }
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void Phong(){

    // AMBIENT
    output_color[0] += k_a * cAmbient[0];
    output_color[1] += k_a * cAmbient[1];
    output_color[2] += k_a * cAmbient[2];

    vec3 N = normalize(vertex_norm_world);

    for (int i = 0; i < num_lights; ++i) {

        vec3 illumAcc = vec3(0.f, 0.f, 0.f);

        float Fatt = 1.f;
        float falloff = 1.f;

        // direciton to light differs between light types
        vec3 L = vec3(0.f);

        float d = length(vertex_pos_world - light_positions[i]);

        if (light_types[i] == 1) { // ---------- directional
            L = normalize(-1 * light_dirs[i]);
        }
        else if (light_types[i] == 0) { // --------- point
            L = normalize(light_positions[i] - vertex_pos_world);

             Fatt = min(1.f / (light_functions[i][0] + (d * light_functions[i][1]) + (d * d * light_functions[i][2])), 1.f);
        }
        else if (light_types[i] == 2) { // ---------- spot
            L = normalize(light_positions[i] - vertex_pos_world);

            float x = acos(dot(L, normalize(-1.f * light_dirs[i]))); // normalize this every time????
            float inner = light_angles[i] - light_penumbras[i]; // inner angle is the total angle - the penumbra

            // falloff factor
            falloff = x <= inner ? 1.f
                                 : (x > light_angles[i])
                                      ? 0.f
                                      : (1.f - falloffFactor(x, inner, light_angles[i]));

            Fatt = min(1.f / (light_functions[i][0] + (d * light_functions[i][1]) + (d * d * light_functions[i][2])), 1.f);
        }
        else { // unsupported light type
            continue;
        }

        float NdotL = clamp(dot(N, L), 0.f, 1.f);

        // DIFFUSE
        illumAcc += Diffuse(NdotL);


        // SPECULAR
        vec3 R = reflect(L, N);
        vec3 dirToCamera = normalize(vertex_pos_world - cam_pos);
        float RdotV = clamp(dot(R, dirToCamera), 0.0, 1.0);

        illumAcc += Specular(RdotV);

        // add total lighting

        illumAcc *= Fatt * falloff * light_colors[i];

//        vec4 FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
        float shadow = ShadowCalculation(fragPosLightSpace);
        output_color[0] += (1-shadow)*illumAcc[0];
        output_color[1] += (1-shadow)*illumAcc[1];
        output_color[2] += (1-shadow)*illumAcc[2];
    }

    output_color[3] = 1.f;
}


void main() {
    vec4 fog_color = vec4(0.9f, 0.9f, 0.9f, 0.6f);
    output_color = vec4(0.f, 0.f, 0.f, 0.f);

    float pointD = distance(vertex_pos_world, cam_pos);
    float alphaFog = getFogFactor(pointD);

    Phong();

    // blend fog and color
    output_color = mix(output_color, fog_color, alphaFog);

    // dim to stymie lighting effects
    output_color *= 0.8f;
}
