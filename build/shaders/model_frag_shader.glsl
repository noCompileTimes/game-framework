#version 430

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_albedo_texture;

layout (binding = 3, std140) uniform light_ubo
{
    vec3  color;     // TODO maybe rename this with light_color; and so on and we can drop u_light? if yes, do for all the _ubo
    float intensity;
    vec3  source;
} u_light;

layout (binding = 4, std140) uniform view_ubo
{
    vec3 position;
} u_view;

vec3 compute_directional_light(vec3 normal, vec3 view_direction, vec3 albedo)
{
    vec3 light_color     = u_light.color * u_light.intensity;
    vec3 light_direction = normalize(-u_light.source); // source is the direction the light travels, negate it to point towards the light

    float ambient_factor = 0.1;
    vec3  ambient_color  = ambient_factor * u_light.color;

    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3  diffuse_color  = diffuse_factor * light_color;

    float specular_intensity = 0.5;
    vec3  halfway_direction  = normalize(light_direction + view_direction);
    float specular_factor    = diffuse_factor > 0.0 ? pow(max(dot(normal, halfway_direction), 0.0), 128.0) : 0.0;
    vec3  specular_color     = specular_intensity * specular_factor * light_color;

    return (ambient_color + diffuse_color) * albedo + specular_color;
}

void main()
{
    vec3 normal         = normalize(in_normal);
    vec3 view_direction = normalize(u_view.position - in_position);
    vec3 albedo         = texture(u_albedo_texture, in_texcoord).rgb;

    out_color = vec4(compute_directional_light(normal, view_direction, albedo), 1.0);
}