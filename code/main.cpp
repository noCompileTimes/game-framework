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

#include "sprite_packer.hpp"
#include "sprite_renderer.hpp"
#include "geometry_utils.hpp"
#include "resources.hpp"
#include "text_renderer.hpp"
#include "camera.hpp"
#include "sprite_atlas.hpp"
#include "font_packer.hpp"
#include "font_atlas.hpp"
#include "text.hpp"

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

    window_events->callbacks.on_resize = [&](const uint32_t width, const uint32_t height)
    {
        window_width  = width;
        window_height = height;
    };

    window_events->callbacks.on_close = [&]
    {
        window_is_running = false;
    };

    gladLoadGL();

    graphics::opengl::ShaderStage model_vertex_stage;
    model_vertex_stage.create(GL_VERTEX_SHADER);
    model_vertex_stage.source(core::File::read("shaders/model_vert_shader.glsl"));

    graphics::opengl::ShaderStage model_fragment_stage;
    model_fragment_stage.type(GL_FRAGMENT_SHADER);
    model_fragment_stage.create();
    model_fragment_stage.source(core::File::read("shaders/model_frag_shader.glsl"));

    graphics::opengl::ShaderStage sprite_vertex_stage;
    sprite_vertex_stage.create(GL_VERTEX_SHADER);
    sprite_vertex_stage.source(core::File::read("shaders/sprite_vert_shader.glsl"));

    graphics::opengl::ShaderStage sprite_fragment_stage;
    sprite_fragment_stage.create(GL_FRAGMENT_SHADER);
    sprite_fragment_stage.source(core::File::read("shaders/sprite_frag_shader.glsl"));

    graphics::opengl::ShaderStage basic_vertex_stage;
    basic_vertex_stage.create(GL_VERTEX_SHADER);
    basic_vertex_stage.source(core::File::read("shaders/basic_vert_shader.glsl"));

    graphics::opengl::ShaderStage basic_fragment_stage;
    basic_fragment_stage.type(GL_FRAGMENT_SHADER);
    basic_fragment_stage.create();
    basic_fragment_stage.source(core::File::read("shaders/basic_frag_shader.glsl"));

    graphics::opengl::ShaderStage text_vertex_stage;
    text_vertex_stage.create(GL_VERTEX_SHADER);
    text_vertex_stage.source(core::File::read("shaders/text_vert_shader.glsl"));

    graphics::opengl::ShaderStage text_fragment_stage;
    text_fragment_stage.create(GL_FRAGMENT_SHADER);
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

    static data::camera model_camera_data;
    model_camera_data.view       = lookAt(glm::vec3 { 0.0f, 0.0f, 10.0f }, glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
    model_camera_data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.3f, 100.0f);

    data::camera sprite_camera_data;
    sprite_camera_data.view       = glm::mat4(1.0f);
    sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f, 1.0f);

    graphics::opengl::Buffer camera_ubo;
    camera_ubo.create();
    camera_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::camera));
    camera_ubo.allocate(sizeof(data::camera), &model_camera_data, GL_DYNAMIC_STORAGE_BIT);

    glm::mat4 model_matrix { 1.0f };
    glm::mat4 sprite_matrix { 1.0f };

    graphics::opengl::Buffer transform_ubo;
    transform_ubo.create();
    transform_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::transform));
    transform_ubo.allocate(sizeof(data::transform), &model_matrix, GL_DYNAMIC_STORAGE_BIT);

    graphics::opengl::Buffer material_ubo;
    material_ubo.create();
    material_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::material));
    material_ubo.allocate(sizeof(data::material), nullptr, GL_DYNAMIC_STORAGE_BIT);

    data::light default_light
    {
        .color = { 1.0f, 1.0f, 1.0f },
        .intensity = 1.0f,
        .source = { -1.0f, -1.0f, -1.0f }
    };

    graphics::opengl::Buffer light_ubo;
    light_ubo.create();
    light_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::light));
    light_ubo.allocate(sizeof(data::light), &default_light, GL_DYNAMIC_STORAGE_BIT);

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

    graphics::opengl::Texture pavement_texture;
    pavement_texture.type = GL_TEXTURE_2D;
    pavement_texture.create();

    pavement_texture.allocate(pavement_image.width, pavement_image.height, GL_RGB8, 1);
    pavement_texture.upload(pavement_image.width, pavement_image.height, GL_RGB, 0, pavement_image.pixels);

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

    text_01.build(default_font, font_atlas);
    text_02.build(default_font, font_atlas);

    graphics::opengl::Commands::clear(0.5f, 0.5f, 0.5f, 1.0f);

    graphics::opengl::Pipeline::enable(GL_MULTISAMPLE);
    graphics::opengl::Pipeline::enable(GL_BLEND);
    graphics::opengl::Pipeline::blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3  red_color { 1.0f, 0.0f, 0.0f };
    glm::vec3 blue_color { 0.0f, 0.0f, 1.0f };

    auto default_texture_width  = 1;
    auto default_texture_height = 1;

    uint8_t default_pixels[3] { 255, 255, 255 };

    graphics::opengl::Texture default_texture;
    default_texture.type = GL_TEXTURE_2D;
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

    // physics end

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

         model_matrix = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3{ 0.0f, 1.0f, 0.0f });
        sprite_matrix = translate(glm::mat4(1.0f), { 10.0f, 10.0f, 0.0f } );

        // clearing

        graphics::opengl::Commands::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        graphics::opengl::Commands::viewport(0, 0, window_width, window_height);

        // 3D drawing

        graphics::opengl::Pipeline::enable(GL_DEPTH_TEST);

        model_sampler.bind(0);

        // models

        model_camera_data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.3f, 100.0f);
        camera_ubo.upload(0, sizeof(data::camera), &model_camera_data);

        transform_ubo.upload(0, sizeof(model_matrix), &model_matrix);

        bt_object_transform.setFromOpenGLMatrix(&model_matrix[0].x);

        bt_collision_object->setWorldTransform(bt_object_transform);

        physics.update();

        // clamp to the buffer capacity so we never write past the end of debug_vbo
        const auto debug_vertices_count = std::min<size_t>(physics.debug()->vertices.size(), debug_vertices_capacity);

        debug_vbo.upload(0, sizeof(core::vec3) * debug_vertices_count, physics.debug()->vertices.data());

        model_program_shader.bind();

        if (show_cube)
        {
            pavement_texture.bind(0);

            cube_geometry_vao.bind();

            graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, cube_geometry_elements.size());
        }
        else
        {
            default_texture.bind(0);

            sphere_geometry_vao.bind();

            graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, sphere_geometry_elements.size());
        }

        basic_program_shader.bind();

        glm::mat4 collider_matrix { 1.0f };
        transform_ubo.upload(0, sizeof(collider_matrix), &collider_matrix);

        debug_vao.bind();

        graphics::opengl::Commands::draw_vertices(GL_LINES, 0, static_cast<int32_t>(debug_vertices_count / 2));

        // 2D drawing

        graphics::opengl::Pipeline::disable(GL_DEPTH_TEST);

        sprite_sampler.bind(0);

        // sprites

        sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f, 1.0f);
        camera_ubo.upload(0, sizeof(data::camera), &sprite_camera_data);

        sprite_program_shader.bind();

        // sprites drawing - begin

        character_atlas.bind();

        // yellow - happy (yellow_body_circle, yellow_hand_thumb)
        render_sprite(1550.0f, 210.0f, 0.0f, 50, false, transform_ubo);
        render_sprite(1440.0f, 250.0f, -10.0f, 59, false, transform_ubo);
        render_sprite(1660.0f, 250.0f, 10.0f, 59, true, transform_ubo);

        // red - angry (red_body_rhombus, red_hand_closed)
        render_sprite(1550.0f, 450.0f, 0.0f, 41, false,  transform_ubo);
        render_sprite(1449.0f, 495.0f, 90.0f, 44, false, transform_ubo);
        render_sprite(1635.0f, 514.0f, -90.0f, 44, true, transform_ubo);

        // blue - sad (blue_body_squircle, blue_hand_open)
        render_sprite(1550.0f, 700.0f, 0.0f, 3, false, transform_ubo);
        render_sprite(1460.0f, 768.0f, 170.0f, 5, false, transform_ubo);
        render_sprite(1640.0f, 768.0f, -170.0f, 5, true, transform_ubo);

        characters_faces_atlas.bind();

        render_sprite(1550.0f, 212.0f, 0.0f, 11, false, transform_ubo); // face_l - happy
        render_sprite(1550.0f, 453.0f, 0.0f, 6, false,  transform_ubo);  // face_g - angry
        render_sprite(1550.0f, 708.0f, 0.0f, 10, false, transform_ubo); // face_k - sad

        // sprites drawing - end

        // atlas viewer - begin

        // TODO draw both sprite atlases texture and the font atlases texture

        if (show_body)
        {
            characters_packer.texture.bind(0); // TODO here should not be the atlas texture?
        }
        else
        {
            //characters_faces_packer.texture.bind(0);
            font_packer.texture.bind(0); // TODO here should not be the atlas texture?
        }

        transform_ubo.upload(0, sizeof(sprite_matrix), &sprite_matrix);

        atlas_viewer_geometry_vao.bind();

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, atlas_viewer_elements.size());

        // atlas viewer - end

        // texts

        text_sampler.bind(0);

        text_program_shader.bind();

        font_atlas.bind();

        material_ubo.upload(0, sizeof(glm::vec3), &red_color);
        render_text(text_01, 200.0f, 700.0f, 1.0f, transform_ubo);

        material_ubo.upload(0, sizeof(glm::vec3), &blue_color);
        render_text(text_02, 200.0f, 800.0f, 0.5f, transform_ubo);

        window_manager.update();
    }

    pavement_texture.destroy();

      text_sampler.destroy();
     model_sampler.destroy();
    sprite_sampler.destroy();

      cube_geometry_vao.destroy();
    atlas_viewer_geometry_vao.destroy();

    cube_geometry_vbo.destroy();
    cube_geometry_ebo.destroy();

    atlas_viewer_geometry_vbo.destroy();
    atlas_viewer_geometry_ebo.destroy();

    model_program_shader.destroy();
    sprite_program_shader.destroy();

    window->destroy();

    return 0;
}

// VERSION 2

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

// VERSION 1

// TODO - use the range buffer binding

// TODO - create a light opengl functions loader

// TODO - decide which version to keep for shader stage type parameter | with the constructor or the function

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