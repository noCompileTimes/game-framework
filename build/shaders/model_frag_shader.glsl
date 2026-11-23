#version 430

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_albedo_texture;

layout (binding = 3, std140) uniform light_ubo
{
    vec3  color;
    float intensity;
    vec3  source;
} u_light;

layout (binding = 4, std140) uniform view_ubo
{
    vec3 position;
} u_view;

void main()
{
    vec3 light_color = u_light.color * u_light.intensity;

    float ambient_factor = 0.1f;
    vec3  ambient_color  = ambient_factor * u_light.color;

    vec3 normal          = normalize(in_normal);
    vec3 light_direction = normalize(-u_light.source); // source is the direction the light travels, negate it to point towards the light

    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3  diffuse_color  = diffuse_factor * light_color;

    float specular_intensity = 0.5;
    vec3 view_direction = normalize(u_view.position - in_position);

     vec3  halfway_direction = normalize(light_direction + view_direction);
    float specular_factor    = diffuse_factor > 0.0 ? pow(max(dot(normal, halfway_direction), 0.0), 128.0) : 0.0;
     vec3 specular_color     = specular_intensity * specular_factor * light_color;

    vec3 albedo_texture_color = texture(u_albedo_texture, in_texcoord).rgb;
     out_color = vec4((ambient_color + diffuse_color) * albedo_texture_color + specular_color, 1.0);
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