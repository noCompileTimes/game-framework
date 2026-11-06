#version 430

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

layout (location = 0) out vec3 out_normal;
layout (location = 1) out vec2 out_texcoord;
layout (location = 2) out vec3 out_position; // TODO put it maybe the first to have the same as inputs

layout (binding = 0, std140) uniform camera_ubo
{
    mat4 view;
    mat4 projection;
};

layout (binding = 1, std140) uniform transform_ubo
{
    mat4 model;
    // TODO add the normal matrix here
};

void main()
{
    gl_Position = projection * view * model * vec4(in_position, 1.0);

    // TODO use this mat3(transpose(inverse(model))) when you are having a uniform scalling
    out_normal   = mat3(model) * in_normal;
    out_texcoord = in_texcoord;
    out_position = vec3(model * vec4(in_position, 1.0));
}