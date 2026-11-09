#version 430

layout (location = 0) in vec3 in_normal;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec3 in_position; // TODO put it maybe the first to have the same as outputs from vertex stage

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_albedo_texture;

layout (binding = 3, std140) uniform light_ubo
{
    vec3 color;
    float intensity;
    vec3 source;
} u_light;

void main()
{
    // TODO add a light color equal with this u_light.color * u_light.intensity in case we decide the intensity to be general for the light, not by diffuse or specual
    // TODO check more what is the normal way to do it here

    float ambient_factor = 0.1f;
    vec3 ambient_color = ambient_factor * u_light.color;

    vec3 normal = normalize(in_normal);
    vec3 light_direction = normalize(-u_light.source); // source is the direction the light travels, negate it to point towards the light

    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3  diffuse_color  = diffuse_factor * u_light.color * u_light.intensity;

    out_color = vec4(ambient_color + diffuse_color, 1.0) * texture(u_albedo_texture, in_texcoord);
}

/*

// TODO try this for your uniform buffer objects
// TODO maybe put all the structures in a common shader file? look into this

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

uniform DirectionalLight gDirectionalLight;

*/