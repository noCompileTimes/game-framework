#include "backends/glfw/window_factory.hpp"

#include "core/window_manager.hpp"
#include "core/physics.hpp"
#include "core/file.hpp"

#include "graphics/opengl/buffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/commands.hpp"
#include "graphics/opengl/pipeline.hpp"
#include "graphics/opengl/shader_stage.hpp"
#include "graphics/opengl/shader_program.hpp"
#include "graphics/opengl/texture_sampler.hpp"
#include "graphics/opengl/framebuffer.hpp"

#include "data/binding.hpp"
#include "data/camera.hpp"
#include "data/transform.hpp"
#include "data/material.hpp"
#include "data/light.hpp"
#include "data/view.hpp"

#include "sprite_packer.hpp"
#include "sprite_renderer.hpp"
#include "sprite_geometry.hpp"
#include "geometry_utils.hpp"
#include "resources.hpp"
#include "text_renderer.hpp"
#include "camera.hpp"
#include "character_entity.hpp"
#include "sprite_atlas.hpp"
#include "font_packer.hpp"
#include "font_atlas.hpp"
#include "text.hpp"
#include "texture_binding.hpp"

auto main() -> int32_t
{
    constexpr core::window::configuration window_configuration
    {
        "Game Framework", 1920, 1080, 4
    };

    const std::unique_ptr<core::base::WindowFactory> window_factory = std::make_unique<backends::glfw::WindowFactory>();

    core::WindowManager window_manager;
    window_manager.init(window_factory, window_configuration);

    const auto window = window_manager.window();
    const auto window_events = window_manager.window_events();

    static auto window_width = window_configuration.width;
    static auto window_height = window_configuration.height;

    static auto window_is_running = true;

    window_events->callbacks.on_resize = [&](const int32_t width, const int32_t height)
    {
        window_width  = width;
        window_height = height;
    };

    window_events->callbacks.on_close = [&]
    {
        window_is_running = false;
    };

    gladLoadGL();

    graphics::opengl::ShaderStage model_vertex_stage { GL_VERTEX_SHADER };
    model_vertex_stage.create();
    model_vertex_stage.source(core::File::read("shaders/model_vert_shader.glsl"));

    graphics::opengl::ShaderStage model_fragment_stage { GL_FRAGMENT_SHADER };
    model_fragment_stage.create();
    model_fragment_stage.source(core::File::read("shaders/model_frag_shader.glsl"));

    graphics::opengl::ShaderStage sprite_vertex_stage { GL_VERTEX_SHADER };
    sprite_vertex_stage.create();
    sprite_vertex_stage.source(core::File::read("shaders/sprite_vert_shader.glsl"));

    graphics::opengl::ShaderStage sprite_fragment_stage { GL_FRAGMENT_SHADER };
    sprite_fragment_stage.create();
    sprite_fragment_stage.source(core::File::read("shaders/sprite_frag_shader.glsl"));

    graphics::opengl::ShaderStage basic_vertex_stage { GL_VERTEX_SHADER };
    basic_vertex_stage.create();
    basic_vertex_stage.source(core::File::read("shaders/basic_vert_shader.glsl"));

    graphics::opengl::ShaderStage basic_fragment_stage { GL_FRAGMENT_SHADER };
    basic_fragment_stage.create();
    basic_fragment_stage.source(core::File::read("shaders/basic_frag_shader.glsl"));

    graphics::opengl::ShaderStage text_vertex_stage { GL_VERTEX_SHADER };
    text_vertex_stage.create();
    text_vertex_stage.source(core::File::read("shaders/text_vert_shader.glsl"));

    graphics::opengl::ShaderStage text_fragment_stage { GL_FRAGMENT_SHADER };
    text_fragment_stage.create();
    text_fragment_stage.source(core::File::read("shaders/text_frag_shader.glsl"));

    graphics::opengl::ShaderProgram model_program_shader;
    model_program_shader.create();
    model_program_shader.attach(model_vertex_stage);
    model_program_shader.attach(model_fragment_stage);
    model_program_shader.link();

    graphics::opengl::ShaderProgram sprite_program_shader;
    sprite_program_shader.create();
    sprite_program_shader.attach(sprite_vertex_stage);
    sprite_program_shader.attach(sprite_fragment_stage);
    sprite_program_shader.link();

    graphics::opengl::ShaderProgram basic_program_shader;
    basic_program_shader.create();
    basic_program_shader.attach(basic_vertex_stage);
    basic_program_shader.attach(basic_fragment_stage);
    basic_program_shader.link();

    graphics::opengl::ShaderProgram text_program_shader;
    text_program_shader.create();
    text_program_shader.attach(text_vertex_stage);
    text_program_shader.attach(text_fragment_stage);
    text_program_shader.link();

    model_vertex_stage.destroy();
    model_fragment_stage.destroy();

    sprite_vertex_stage.destroy();
    sprite_fragment_stage.destroy();

    basic_vertex_stage.destroy();
    basic_fragment_stage.destroy();

    text_vertex_stage.destroy();
    text_fragment_stage.destroy();

    const auto [cube_geometry_vertices, cube_geometry_elements] = load_geometry("models/cube.obj");

    graphics::opengl::Buffer cube_geometry_vbo;
    cube_geometry_vbo.create();
    cube_geometry_vbo.allocate(sizeof(model_vertex) * cube_geometry_vertices.size(), cube_geometry_vertices.data(), 0);

    graphics::opengl::Buffer cube_geometry_ebo;
    cube_geometry_ebo.create();
    cube_geometry_ebo.allocate(sizeof(uint32_t) * cube_geometry_elements.size(), cube_geometry_elements.data(), 0);

    graphics::opengl::VertexArray cube_geometry_vao;
    cube_geometry_vao.create();

    cube_geometry_vao.attach_vertices(cube_geometry_vbo, sizeof(model_vertex));
    cube_geometry_vao.attach_elements(cube_geometry_ebo);

    cube_geometry_vao.attach_attribute({ 0, offsetof(model_vertex, position), 3, GL_FLOAT, 0 });
    cube_geometry_vao.attach_attribute({ 1, offsetof(model_vertex, normal), 3, GL_FLOAT, 0 });
    cube_geometry_vao.attach_attribute({ 2, offsetof(model_vertex, texcoord), 2, GL_FLOAT, 0 });

    const auto [sphere_geometry_vertices, sphere_geometry_elements] = load_geometry("models/sphere.obj");

    graphics::opengl::Buffer sphere_geometry_vbo;
    sphere_geometry_vbo.create();
    sphere_geometry_vbo.allocate(sizeof(model_vertex) * sphere_geometry_vertices.size(), sphere_geometry_vertices.data(), 0);

    graphics::opengl::Buffer sphere_geometry_ebo;
    sphere_geometry_ebo.create();
    sphere_geometry_ebo.allocate(sizeof(uint32_t) * sphere_geometry_elements.size(), sphere_geometry_elements.data(), 0);

    graphics::opengl::VertexArray sphere_geometry_vao;
    sphere_geometry_vao.create();

    sphere_geometry_vao.attach_vertices(sphere_geometry_vbo, sizeof(model_vertex));
    sphere_geometry_vao.attach_elements(sphere_geometry_ebo);

    sphere_geometry_vao.attach_attribute({ 0, offsetof(model_vertex, position), 3, GL_FLOAT, 0 });
    sphere_geometry_vao.attach_attribute({ 1, offsetof(model_vertex, normal), 3, GL_FLOAT, 0 });
    sphere_geometry_vao.attach_attribute({ 2, offsetof(model_vertex, texcoord), 2, GL_FLOAT, 0 });

    const auto [plane_geometry_vertices, plane_geometry_elements] = load_geometry("models/plane.obj");

    graphics::opengl::Buffer plane_geometry_vbo;
    plane_geometry_vbo.create();
    plane_geometry_vbo.allocate(sizeof(model_vertex) * plane_geometry_vertices.size(), plane_geometry_vertices.data(), 0);

    graphics::opengl::Buffer plane_geometry_ebo;
    plane_geometry_ebo.create();
    plane_geometry_ebo.allocate(sizeof(uint32_t) * plane_geometry_elements.size(), plane_geometry_elements.data(), 0);

    graphics::opengl::VertexArray plane_geometry_vao;
    plane_geometry_vao.create();

    plane_geometry_vao.attach_vertices(plane_geometry_vbo, sizeof(model_vertex));
    plane_geometry_vao.attach_elements(plane_geometry_ebo);

    plane_geometry_vao.attach_attribute({ 0, offsetof(model_vertex, position), 3, GL_FLOAT, 0 });
    plane_geometry_vao.attach_attribute({ 1, offsetof(model_vertex, normal), 3, GL_FLOAT, 0 });
    plane_geometry_vao.attach_attribute({ 2, offsetof(model_vertex, texcoord), 2, GL_FLOAT, 0 });

    glm::vec3 camera_position { 0.0f, 7.5f, 15.0f };

    static data::camera model_camera_data;
    model_camera_data.view       = lookAt(camera_position, glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
    model_camera_data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.3f, 100.0f);

    data::view view_data { .position = camera_position };

    data::camera sprite_camera_data;
    sprite_camera_data.view       = glm::mat4(1.0f);
    sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f, 1.0f);

    graphics::opengl::Buffer camera_ubo;
    camera_ubo.create();
    camera_ubo.allocate(sizeof(data::camera), &model_camera_data, GL_DYNAMIC_STORAGE_BIT);
    camera_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::camera));

    graphics::opengl::Buffer view_ubo;
    view_ubo.create();
    view_ubo.allocate(sizeof(data::view), &view_data, GL_DYNAMIC_STORAGE_BIT); // upload() it when the camera moves
    view_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::view));

    constexpr glm::vec3 model_position { 0.0f, 1.5f, 0.0f };
    constexpr glm::vec3 light_position { 4.0f, 4.0f, 4.0f };

    glm::mat4 model_matrix  { 1.0f };
    glm::mat4 plane_matrix  { 1.0f };
    glm::mat4 sprite_matrix = translate(glm::mat4(1.0f), { 10.0f, 10.0f, 0.0f } );
    glm::mat4  light_matrix = translate(glm::mat4(1.0f), light_position);
    glm::mat4  debug_matrix { 1.0f };

    graphics::opengl::Buffer transform_ubo;
    transform_ubo.create();
    transform_ubo.allocate(sizeof(data::transform), nullptr, GL_DYNAMIC_STORAGE_BIT);
    transform_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::transform));

    graphics::opengl::Buffer material_ubo;
    material_ubo.create();
    material_ubo.allocate(sizeof(data::material), nullptr, GL_DYNAMIC_STORAGE_BIT);
    material_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::material));

    data::light default_light
    {
        .color = { 1.0f, 1.0f, 1.0f },
        .intensity = 1.0f,
        .source = { -1.0f, -1.0f, -1.0f }
    };

    graphics::opengl::Buffer light_ubo;
    light_ubo.create();
    light_ubo.allocate(sizeof(data::light), &default_light, GL_DYNAMIC_STORAGE_BIT);
    light_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::light));

    graphics::opengl::TextureSampler text_sampler;
    text_sampler.create();
    text_sampler.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    text_sampler.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    text_sampler.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    text_sampler.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    graphics::opengl::TextureSampler sprite_sampler;
    sprite_sampler.create();
    sprite_sampler.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sprite_sampler.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sprite_sampler.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    sprite_sampler.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    graphics::opengl::TextureSampler model_sampler;
    model_sampler.create();
    model_sampler.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    model_sampler.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    image_data pavement_image = load_image("textures/pavement_albedo.png");

    graphics::opengl::Texture pavement_texture { GL_TEXTURE_2D };
    pavement_texture.create();

    pavement_texture.allocate(pavement_image.width, pavement_image.height, GL_RGB8, 1);
    pavement_texture.upload(pavement_image.width, pavement_image.height, GL_RGB, 0, pavement_image.pixels);

    stbi_image_free(pavement_image.pixels);

    // atlas output - begin

    // TODO maybe have this like in a separate "scene" or maybe when we press a key
    SpritePacker characters_packer { "textures/characters", 1024, 1024 };
    characters_packer.init();
    characters_packer.build(sprite_program_shader, sprite_sampler, camera_ubo, transform_ubo);

    SpritePacker characters_faces_packer { "textures/characters_faces", 512, 512 };
    characters_faces_packer.init();
    characters_faces_packer.build(sprite_program_shader, sprite_sampler, camera_ubo, transform_ubo);

    SpriteAtlas character_atlas;
    character_atlas.init("textures/characters.txt");

    SpriteAtlas characters_faces_atlas;
    characters_faces_atlas.init("textures/characters_faces.txt");

    // atlas viewer - begin

    const auto [atlas_viewer_vertices, atlas_viewer_elements] = create_sprite({ 0, 0, 256, 256 }, true);

    graphics::opengl::Buffer atlas_viewer_geometry_vbo;
    atlas_viewer_geometry_vbo.create();
    atlas_viewer_geometry_vbo.allocate(sizeof(sprite_vertex) * atlas_viewer_vertices.size(), atlas_viewer_vertices.data(), 0);

    graphics::opengl::Buffer atlas_viewer_geometry_ebo;
    atlas_viewer_geometry_ebo.create();
    atlas_viewer_geometry_ebo.allocate(sizeof(uint32_t) * atlas_viewer_elements.size(), atlas_viewer_elements.data(), 0);

    graphics::opengl::VertexArray atlas_viewer_geometry_vao;
    atlas_viewer_geometry_vao.create();

    atlas_viewer_geometry_vao.attach_vertices(atlas_viewer_geometry_vbo, sizeof(sprite_vertex));
    atlas_viewer_geometry_vao.attach_elements(atlas_viewer_geometry_ebo);

    atlas_viewer_geometry_vao.attach_attribute({ 0, offsetof(sprite_vertex, position), 2, GL_FLOAT, 0 });
    atlas_viewer_geometry_vao.attach_attribute({ 1, offsetof(sprite_vertex, texcoord), 2, GL_FLOAT, 0 });

    // atlas viewer - end

    Font default_font;

    if (auto font_result = default_font.load("Antonio-Bold.ttf"); font_result != 0)
    {
        return font_result;
    }

    FontPacker font_packer { "textures/font", 512, 512 };
    font_packer.init(default_font.images);
    font_packer.build(sprite_program_shader, sprite_sampler, camera_ubo, transform_ubo);

    default_font.release_images();

    FontAtlas font_atlas;
    font_atlas.init("textures/font.txt");

    Text text_01
    {
        .text = "This is sample text",
    };

    Text text_02
    {
        .text = "This is a second sample text",
    };

    Text text_03
    {
        .text = "This is a third sample text",
    };

    text_01.build(default_font, font_atlas);
    text_02.build(default_font, font_atlas);
    text_03.build(default_font, font_atlas);

    graphics::opengl::Commands::clear(0.5f, 0.5f, 0.5f, 1.0f);

    graphics::opengl::Pipeline::enable(GL_MULTISAMPLE);
    graphics::opengl::Pipeline::enable(GL_BLEND);
    graphics::opengl::Pipeline::blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3   red_color { 1.0f, 0.0f, 0.0f };
    glm::vec3 green_color { 0.0f, 1.0f, 0.0f };
    glm::vec3  blue_color { 0.0f, 0.0f, 1.0f };

    auto default_texture_width  = 1;
    auto default_texture_height = 1;

    uint8_t default_pixels[3] { 255, 255, 255 };

    graphics::opengl::Texture default_texture { GL_TEXTURE_2D };
    default_texture.create();

    default_texture.allocate(default_texture_width, default_texture_height, GL_RGB8, 1);
    default_texture.upload(default_texture_width, default_texture_height, GL_RGB, 0, default_pixels);

    // physics begin

    static btCollisionShape* bt_cube_shape = new btBoxShape({ 1.0f, 1.0f, 1.0f });
    static btCollisionShape* bt_sphere_shape = new btSphereShape(1.0f);

    btTransform bt_object_transform;
    bt_object_transform.setIdentity();

    static auto bt_collision_object = new btCollisionObject();
    bt_collision_object->setCollisionShape(bt_cube_shape);
    bt_collision_object->setWorldTransform(bt_object_transform);

    static core::Physics physics;
                         physics.init();

    physics.add_collision_object(bt_collision_object);

    constexpr uint32_t debug_vertices_capacity = 10000;

    graphics::opengl::Buffer debug_vbo;
    debug_vbo.create();
    debug_vbo.allocate(sizeof(editor_vertex) * debug_vertices_capacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

    graphics::opengl::VertexArray debug_vao;
    debug_vao.create();
    debug_vao.attach_vertices(debug_vbo, sizeof(editor_vertex));

    debug_vao.attach_attribute({ 0, offsetof(editor_vertex, position), 3, GL_FLOAT, 0 });
    debug_vao.attach_attribute({ 1, offsetof(editor_vertex, color), 3, GL_FLOAT, 0 });

    constexpr auto light_default_value = 0.15f; // TODO propose a better name
    constexpr core::aabb light_default_bounds
    {
        .min // TODO maybe use here some constructor which set all of them as this value?
        {
            -light_default_value,
            -light_default_value,
            -light_default_value
        },
        .max // TODO maybe use here some constructor which set all of them as this value?
        {
            light_default_value,
            light_default_value,
            light_default_value
        }
    };

    auto [light_geometry_vertices, light_geometry_elements] = create_cube(light_default_bounds, { 0.0f, 0.0f, 1.0f });

    graphics::opengl::Buffer light_vbo;
    light_vbo.create();
    light_vbo.allocate(sizeof(editor_vertex) * light_geometry_vertices.size(), light_geometry_vertices.data(), 0);

    graphics::opengl::Buffer light_ebo;
    light_ebo.create();
    light_ebo.allocate(sizeof(uint32_t) * light_geometry_elements.size(), light_geometry_elements.data(), 0);

    graphics::opengl::VertexArray light_vao;
    light_vao.create();
    light_vao.attach_vertices(light_vbo, sizeof(editor_vertex));
    light_vao.attach_elements(light_ebo);

    light_vao.attach_attribute({ 0, offsetof(editor_vertex, position), 3, GL_FLOAT, 0 });
    light_vao.attach_attribute({ 1, offsetof(editor_vertex, color), 3, GL_FLOAT, 0 });

    // physics end

    constexpr auto default_x_entity_position = 1700.0f;

    // the position of the body, the hands and the face are drawn with an offset from it
    character_entity red_entity    { .face_index =  6, .body_index = 41, .hand_index = 44, .x = default_x_entity_position, .y = 450.0f }; // angry (face_g, red_body_rhombus, red_hand_closed)
    character_entity blue_entity   { .face_index = 10, .body_index =  3, .hand_index =  5, .x = default_x_entity_position, .y = 700.0f }; // sad (face_k, blue_body_squircle, blue_hand_open)
    character_entity yellow_entity { .face_index = 11, .body_index = 50, .hand_index = 59, .x = default_x_entity_position, .y = 210.0f }; // happy (face_l, yellow_body_circle, yellow_hand_thumb)

    // input begin

    static auto show_cube = true;
    static auto show_body = true;

    glfwSetKeyCallback(std::any_cast<GLFWwindow*>(window->handle()), [](GLFWwindow*, const int32_t key, int32_t, const int32_t action, int32_t)
    {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            show_cube = !show_cube;

            if (show_cube)
            {
                bt_collision_object->setCollisionShape(bt_cube_shape);
            }
            else
            {
                bt_collision_object->setCollisionShape(bt_sphere_shape);
            }
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            window_is_running = false;
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            show_body = !show_body;
        }
    });

    glfwSetMouseButtonCallback(std::any_cast<GLFWwindow*>(window->handle()), [](GLFWwindow* glfw_window, const int button, int action, int)
    {
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
        {
            double mouse_x;
            double mouse_y;

            glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);

            // the cursor is in screen coordinates, while window_width/window_height come from the framebuffer (on_resize)
            // with display scaling (e.g. 150%) they differ, so use the window size in screen coordinates to match the cursor
            int screen_width;
            int screen_height;

            glfwGetWindowSize(glfw_window, &screen_width, &screen_height);

            core::vec3 out_origin;
            core::vec3 out_direction;

            // glfw gives the cursor from the top-left corner, the function expects it from the bottom-left
            screen_pos_to_world_ray(static_cast<int32_t>(mouse_x), screen_height - static_cast<int32_t>(mouse_y),
                screen_width, screen_height, model_camera_data.view, model_camera_data.projection,out_origin,out_direction);

            if (const auto collision_object = physics.raycast(out_origin, out_direction); collision_object != nullptr)
            {
                std::println("object is hit");
            }
        }
    });

    // input end

    graphics::opengl::Framebuffer default_fbo;
                                  default_fbo.bind();
    while (window_is_running)
    {
        // update

        model_matrix = translate(glm::mat4(1.0f), model_position);
        model_matrix = rotate(model_matrix, static_cast<float>(glfwGetTime()), glm::vec3{ 0.0f, 1.0f, 0.0f });

        // clearing

        graphics::opengl::Commands::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        graphics::opengl::Commands::viewport(0, 0, window_width, window_height);

        // 3D drawing

        graphics::opengl::Pipeline::enable(GL_DEPTH_TEST);

        model_sampler.bind(std::to_underlying(texture_binding::albedo));

        // models

        model_camera_data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.3f, 100.0f);
        camera_ubo.upload(0, sizeof(data::camera), &model_camera_data);

        bt_object_transform.setFromOpenGLMatrix(&model_matrix[0].x);

        bt_collision_object->setWorldTransform(bt_object_transform);

        physics.update();

        // the physics debug stores a position and a color vec3 per vertex, so two of them make one editor_vertex
        // clamp to the buffer capacity so we never write past the end of debug_vbo
        const auto debug_vertices_count = std::min<size_t>(physics.debug()->vertices.size() / 2, debug_vertices_capacity);

        debug_vbo.upload(0, sizeof(editor_vertex) * debug_vertices_count, physics.debug()->vertices.data());

        model_program_shader.bind();

        default_texture.bind(std::to_underlying(texture_binding::albedo));

        transform_ubo.upload(0, sizeof(glm::mat4), &plane_matrix);

        plane_geometry_vao.bind();

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, plane_geometry_elements.size());

        transform_ubo.upload(0, sizeof(glm::mat4), &model_matrix);

        if (show_cube)
        {
            pavement_texture.bind(std::to_underlying(texture_binding::albedo));

            cube_geometry_vao.bind();

            graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, cube_geometry_elements.size());
        }
        else
        {
            default_texture.bind(std::to_underlying(texture_binding::albedo));

            sphere_geometry_vao.bind();

            graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, sphere_geometry_elements.size());
        }

        basic_program_shader.bind();

        if (show_cube)
        {
            transform_ubo.upload(0, sizeof(glm::mat4), &debug_matrix);

            debug_vao.bind();

            graphics::opengl::Commands::draw_vertices(GL_LINES, 0, static_cast<int32_t>(debug_vertices_count));
        }

        transform_ubo.upload(0, sizeof(glm::mat4), &light_matrix);

        light_vao.bind();

        graphics::opengl::Commands::draw_elements(GL_LINES, 0, light_geometry_elements.size());

        // 2D drawing

        graphics::opengl::Pipeline::disable(GL_DEPTH_TEST);

        sprite_sampler.bind(std::to_underlying(texture_binding::albedo));

        // sprites

        sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f, 1.0f);
        camera_ubo.upload(0, sizeof(data::camera), &sprite_camera_data);

        sprite_program_shader.bind();

        // sprites drawing - begin

        character_atlas.bind();

        // yellow - happy (yellow_body_circle, yellow_hand_thumb)
        render_sprite(yellow_entity.x, yellow_entity.y, 0.0f, yellow_entity.body_index, false, transform_ubo);
        render_sprite(yellow_entity.x - 110.0f, yellow_entity.y + 40.0f, -10.0f, yellow_entity.hand_index, false, transform_ubo);
        render_sprite(yellow_entity.x + 110.0f, yellow_entity.y + 40.0f,  10.0f, yellow_entity.hand_index, true,  transform_ubo);

        // red - angry (red_body_rhombus, red_hand_closed)
        render_sprite(red_entity.x, red_entity.y, 0.0f, red_entity.body_index, false, transform_ubo);
        render_sprite(red_entity.x - 101.0f, red_entity.y + 45.0f,  90.0f, red_entity.hand_index, false, transform_ubo);
        render_sprite(red_entity.x +  85.0f, red_entity.y + 64.0f, -90.0f, red_entity.hand_index, true,  transform_ubo);

        // blue - sad (blue_body_squircle, blue_hand_open)
        render_sprite(blue_entity.x, blue_entity.y, 0.0f, blue_entity.body_index, false, transform_ubo);
        render_sprite(blue_entity.x - 90.0f, blue_entity.y + 68.0f,  170.0f, blue_entity.hand_index, false, transform_ubo);
        render_sprite(blue_entity.x + 90.0f, blue_entity.y + 68.0f, -170.0f, blue_entity.hand_index, true,  transform_ubo);

        characters_faces_atlas.bind();

        render_sprite(yellow_entity.x, yellow_entity.y + 2.0f, 0.0f, yellow_entity.face_index, false, transform_ubo); // face_l - happy
        render_sprite(red_entity.x,    red_entity.y    + 3.0f, 0.0f, red_entity.face_index, false, transform_ubo); // face_g - angry
        render_sprite(blue_entity.x,   blue_entity.y   + 8.0f, 0.0f, blue_entity.face_index, false, transform_ubo); // face_k - sad

        // sprites drawing - end

        // atlas viewer - begin

        // TODO draw both sprite atlases texture and the font atlases texture

        if (show_body)
        {
            characters_packer.texture.bind(std::to_underlying(texture_binding::albedo)); // TODO here should not be the atlas texture?
        }
        else
        {
            //characters_faces_packer.texture.bind(std::to_underlying(texture_binding::albedo));
            font_packer.texture.bind(std::to_underlying(texture_binding::albedo)); // TODO here should not be the atlas texture?
        }

        transform_ubo.upload(0, sizeof(glm::mat4), &sprite_matrix);

        atlas_viewer_geometry_vao.bind();

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, atlas_viewer_elements.size());

        // atlas viewer - end

        // texts

        text_sampler.bind(std::to_underlying(texture_binding::albedo));

        text_program_shader.bind();

        font_atlas.bind();

        // the text sizes are at scale 1, so they are multiplied by the scale the text is drawn with
        constexpr auto text_01_x     = 100.0f;
        constexpr auto text_01_y     = 890.0f;
        constexpr auto text_01_scale = 1.0f;

        material_ubo.upload(0, sizeof(glm::vec3), &red_color);
        render_text(text_01, text_01_x, text_01_y, text_01_scale, transform_ubo);

        // right after text_01
        material_ubo.upload(0, sizeof(glm::vec3), &blue_color);
        render_text(text_02, text_01_x + text_01.width * text_01_scale, text_01_y, 0.5f, transform_ubo);

        // right below text_01
        material_ubo.upload(0, sizeof(glm::vec3), &green_color);
        render_text(text_03, text_01_x, text_01_y + text_01.height * text_01_scale, 1.0f, transform_ubo);

        window_manager.update();
    }

    pavement_texture.destroy();
     default_texture.destroy();

    characters_packer.texture.destroy();
    characters_faces_packer.texture.destroy();
    font_packer.texture.destroy();

           character_atlas.release();
    characters_faces_atlas.release();
                font_atlas.release();

      text_sampler.destroy();
     model_sampler.destroy();
    sprite_sampler.destroy();

            cube_geometry_vao.destroy();
          sphere_geometry_vao.destroy();
           plane_geometry_vao.destroy();
    atlas_viewer_geometry_vao.destroy();
                    debug_vao.destroy();
                    light_vao.destroy();

    cube_geometry_vbo.destroy();
    cube_geometry_ebo.destroy();

    sphere_geometry_vbo.destroy();
    sphere_geometry_ebo.destroy();

    plane_geometry_vbo.destroy();
    plane_geometry_ebo.destroy();

    atlas_viewer_geometry_vbo.destroy();
    atlas_viewer_geometry_ebo.destroy();

    debug_vbo.destroy();

    light_vbo.destroy();
    light_ebo.destroy();

       camera_ubo.destroy();
         view_ubo.destroy();
    transform_ubo.destroy();
     material_ubo.destroy();
        light_ubo.destroy();

    model_program_shader.destroy();
    sprite_program_shader.destroy();
     basic_program_shader.destroy();
      text_program_shader.destroy();

    window_manager.release();

    return 0;
}

// VERSION 3

// TODO - move the ambient and specular values from model_frag_shader to the material ubo (data::material + material_ubo, binding 2):
//        ambient_factor (0.1), specular_intensity (0.5) and the shininess exponent (128.0) - they belong to the surface, not the light
//        (keep the std140 layout in sync: data::material is alignas(16) with albedo_color as rgb, the new floats can fill the vec3 padding)

// VERSION 2

// TODO - check if using function in glsl will break the performance? like for compute_direction_light, stuff like this

// TODO - integrate the imguizmo to use the rotation/orientation tool for the light cube
// TODO - based on the orientation of the cube compute also a direction for the light to be set for the shader

// TODO - ask for a code review on the font, font_atlas, font_packer, text, text_renderer - basically what is about the text implementation

// TODO - some way to edit an existing sprite atlas to adjust the sprite pivot and maybe who knows other characteristics

// TODO - make a to_base function which wraps std::to_underlying

// TODO - check why the hands on the yellow and blue are having some weird edges? - some about the blending, I did not like the first solution proposed

// TODO - maybe the part of preparing the images can be different for font atlas and sprite atlas, but the rendering/building of the atlas should be the same? maybe we can have a
//        base atlas class or something? and in the base we draw the things in a generic way? and we just prepare the data in a more different way
// TODO - for the above - maybe we should have separate things? maybe for the font we need more data to be saved in the file? some different filtering for the font when we will do outline and all that?

// TODO - consider maybe doing the sprite atlas on cpu with memcpy maybe do some tests and see which one is faster
//        maybe in this way we will just display a texture and a rectangle (lines) for each rectangle packed
//        think on how to update he memory in the texture, all at once or some regions with upload
// TODO - make a SpriteAtlasViewer/FontViewer or maybe even TextureViewer class

// TODO - be sure that in all the modules and frameworks (currently just one) the cmake functions have the same order in all of the CMakeFile lists

// TODO - add the opengl debug output: glEnable(GL_DEBUG_OUTPUT) + glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS) + glDebugMessageCallback
//        (glad already has glDebugMessageCallback/glDebugMessageControl), filter the notifications with glDebugMessageControl
//        and create the glfw context as a debug context so the driver reports more - it catches errors like upload() on
//        a buffer without GL_DYNAMIC_STORAGE_BIT (GL_INVALID_OPERATION) that otherwise fail silently

// TODO - when there are many objects, put all the transforms in one buffer with a slot per object and bind a range per draw
//        instead of the many small transform_ubo.upload() calls (each one makes the driver sync on the same buffer)
//        the range offset must be a multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT (256 on this gpu, 64 or less on others),
//        query it once with glGetIntegerv and round the slots up: slot i at i * align_up(sizeof(data::transform)), upload once per frame
//        (the size has no alignment rule, ssbos have their own GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT)

// VERSION 1

// TODO - use the range buffer binding

// TODO - create a light opengl functions loader

// TODO - name the shader files as the variables - CLAUDE

// TODO - check and understand why in some cases we need create_sprite with flip_vertically true and sometimes false

// TODO - remove glfwHideWindow from glfw and all the internal function related to this

// TODO - check which modules we will not use at any time in the future in the stb nothings

// TODO - remove unnecessary formats loading from stb_image also from stb_image_write

// TODO - have this #define GL_COLOR_ATTACHMENT0 0x8CE0 as a variable representing the albedo?
// TODO - remove everything (functions) related to GL_POINTS | we will never render points or something like this

// TODO - resolve the flipping of the y from the ortho matrix (need to recheck all the sprites/fonts)

// TODO - make the on_resize callback to use something from the window (glfwGetWindowSize(glfw_window, &screen_width, &screen_height)) not the size from the framebuffer

// TODO - in case in glfw use use on_resize the window size, remove the framebuffer callbacks from it