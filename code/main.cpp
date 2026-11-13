#include <stb_rect_pack.h>
#include <stb_image_write.h>

#include "backends/glfw/window_factory.hpp"

#include "core/window_manager.hpp"
#include "core/physics.hpp"
#include "core/file.hpp"
#include "core/rgba.hpp"

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
#include "geometry_utils.hpp"
#include "resources.hpp"
#include "text.hpp"
#include "camera.hpp"

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
    cube_geometry_vbo.storage(sizeof(model_vertex) * cube_geometry_vertices.size(), cube_geometry_vertices.data(), 0);

    graphics::opengl::Buffer cube_geometry_ebo;
    cube_geometry_ebo.create();
    cube_geometry_ebo.storage(sizeof(uint32_t) * cube_geometry_elements.size(), cube_geometry_elements.data(), 0);

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
    sphere_geometry_vbo.storage(sizeof(model_vertex) * sphere_geometry_vertices.size(), sphere_geometry_vertices.data(), 0);

    graphics::opengl::Buffer sphere_geometry_ebo;
    sphere_geometry_ebo.create();
    sphere_geometry_ebo.storage(sizeof(uint32_t) * sphere_geometry_elements.size(), sphere_geometry_elements.data(), 0);

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
    camera_ubo.storage(sizeof(data::camera), &model_camera_data, GL_DYNAMIC_STORAGE_BIT);

    glm::mat4 model_matrix { 1.0f };
    glm::mat4 sprite_matrix { 1.0f };

    graphics::opengl::Buffer transform_ubo;
    transform_ubo.create();
    transform_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::transform));
    transform_ubo.storage(sizeof(data::transform), &model_matrix, GL_DYNAMIC_STORAGE_BIT);

    graphics::opengl::Buffer material_ubo;
    material_ubo.create();
    material_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::material));
    material_ubo.storage(sizeof(data::material), nullptr, GL_DYNAMIC_STORAGE_BIT);

    data::light default_light
    {
        .color = { 1.0f, 1.0f, 1.0f },
        .intensity = 1.0f,
        .source = { -1.0f, -1.0f, -1.0f }
    };

    graphics::opengl::Buffer light_ubo;
    light_ubo.create();
    light_ubo.bind(GL_UNIFORM_BUFFER, std::to_underlying(data::binding::light));
    light_ubo.storage(sizeof(data::light), &default_light, GL_DYNAMIC_STORAGE_BIT);

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

    texture_data pavement_image = load_texture("textures/pavement_albedo.png");

    graphics::opengl::Texture pavement_texture;
    pavement_texture.type = GL_TEXTURE_2D;
    pavement_texture.create();

    pavement_texture.storage(pavement_image.width, pavement_image.height, GL_RGB8, 1);
    pavement_texture.subdata(pavement_image.width, pavement_image.height, GL_RGB, 0, pavement_image.pixels);

    // fonts - begin

    if (auto font_result = load_font("Antonio-Bold.ttf"); font_result != 0)
    {
        return font_result;
    }

    graphics::opengl::Buffer text_vbo;
    text_vbo.create();
    text_vbo.storage(1000 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);

    //graphics::opengl::Buffer text_ebo;
    //text_ebo.create();
    //text_vbo.storage(1000 * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);

    graphics::opengl::VertexArray text_vao;
    text_vao.create();
    text_vao.attach_vertices(text_vbo, sizeof(text_vertex));

    text_vao.attach_attribute({ 0, offsetof(text_vertex, position), 2, GL_FLOAT, 0 });
    text_vao.attach_attribute({ 1, offsetof(text_vertex, texcoord), 2, GL_FLOAT, 0 });

    // fonts - end

    // rectangles packing - begin

    std::string folder_name = "textures/characters";
    auto images_data = load_textures(folder_name);

    stbrp_context stbrp_context;

    auto atlas_width  = 1024;
    auto atlas_height = 1024;

    constexpr auto atlas_nodes_count = 1024;
    stbrp_node atlas_nodes[atlas_nodes_count];

    stbrp_init_target(&stbrp_context, atlas_width, atlas_height, atlas_nodes, atlas_nodes_count);

    const auto rectangles_count = static_cast<int32_t>(images_data.size());
    std::vector<stbrp_rect> rectangles(rectangles_count);

    constexpr auto atlas_padding = 2;

    for (auto i = 0; i < images_data.size(); i++)
    {
        const auto& image_data = images_data[i];

        rectangles[i].w = image_data.width  + atlas_padding;
        rectangles[i].h = image_data.height + atlas_padding;
    }

    auto all_packed = stbrp_pack_rects(&stbrp_context, rectangles.data(), rectangles_count);

    for (auto& rectangle : rectangles)
    {
        rectangle.x += atlas_padding / 2;
        rectangle.y += atlas_padding / 2;
        rectangle.w -= atlas_padding;
        rectangle.h -= atlas_padding;
    }

    printf("Packing status: %s\n\n", all_packed ? "All packed successfully!" : "Some rectangles did not fit.");

    geometry<sprite_vertex, uint32_t> rectangles_geometry;

    std::ofstream output_file;
    output_file.open(folder_name + ".txt", std::ios::trunc); // TODO check what this ios::trunc does
    // TODO have the extension as sprites

    for (auto i = 0; i < rectangles_count; ++i)
    {
        if (rectangles[i].was_packed)
        {
            // TODO move this sprite atlas write into a separate function/class
            // TODO find a better/best way to write this in a file
            output_file << rectangles[i].x << " " << rectangles[i].y << " " << rectangles[i].w << " " << rectangles[i].h << std::endl;

            rectangles_geometry.append(create_sprite({ 0, 0,  static_cast<float>(rectangles[i].w), static_cast<float>(rectangles[i].h) }, false));
        }
        else
        {
            printf("Rect FAILED to pack [Size: %dx%d]\n", rectangles[i].w, rectangles[i].h);
        }
    }

    const auto [sprite_vertices, sprite_elements] = create_sprite({ 0, 0, static_cast<float>(atlas_width), static_cast<float>(atlas_height) }, true);

    graphics::opengl::Buffer sprite_geometry_vbo;
    sprite_geometry_vbo.create();
    sprite_geometry_vbo.storage(sizeof(sprite_vertex) * sprite_vertices.size(), sprite_vertices.data(), 0);

    graphics::opengl::Buffer sprite_geometry_ebo;
    sprite_geometry_ebo.create();
    sprite_geometry_ebo.storage(sizeof(uint32_t) * sprite_elements.size(), sprite_elements.data(), 0);

    graphics::opengl::VertexArray sprite_geometry_vao;
    sprite_geometry_vao.create();

    sprite_geometry_vao.attach_vertices(sprite_geometry_vbo, sizeof(sprite_vertex));
    sprite_geometry_vao.attach_elements(sprite_geometry_ebo);

    sprite_geometry_vao.attach_attribute({ 0, offsetof(sprite_vertex, position), 2, GL_FLOAT, 0 });
    sprite_geometry_vao.attach_attribute({ 1, offsetof(sprite_vertex, texcoord), 2, GL_FLOAT, 0 });

    // rectangles packing - end

    // framebuffer - begin

    graphics::opengl::Texture framebuffer_texture;
    framebuffer_texture.type = GL_TEXTURE_2D;
    framebuffer_texture.create();
    framebuffer_texture.storage(atlas_width, atlas_height, GL_RGBA8, 1);

    //graphics::opengl::Renderbuffer renderbuffer;
    //renderbuffer.create();
    //renderbuffer.storage(atlas_width, atlas_height, GL_DEPTH_COMPONENT24);

    graphics::opengl::Framebuffer framebuffer;
    framebuffer.create();
    framebuffer.attach(framebuffer_texture, GL_COLOR_ATTACHMENT0, 0);
    //framebuffer.attach(renderbuffer, GL_DEPTH_ATTACHMENT);

    // framebuffer - end

    graphics::opengl::Buffer framebuffer_pbo;
    framebuffer_pbo.create();
    framebuffer_pbo.storage(atlas_width * atlas_height * 4, nullptr, GL_MAP_READ_BIT);
    framebuffer_pbo.bind(GL_PIXEL_PACK_BUFFER);

    // rectangles

    graphics::opengl::Buffer rectangles_vbo;
    rectangles_vbo.create();
    rectangles_vbo.storage(sizeof(sprite_vertex) * rectangles_geometry.vertices.size(), rectangles_geometry.vertices.data(), 0);

    graphics::opengl::Buffer rectangles_ebo;
    rectangles_ebo.create();
    rectangles_ebo.storage(sizeof(uint32_t) * rectangles_geometry.elements.size(), rectangles_geometry.elements.data(), 0);

    graphics::opengl::VertexArray rectangles_vao;
    rectangles_vao.create();

    rectangles_vao.attach_vertices(rectangles_vbo, sizeof(sprite_vertex));
    rectangles_vao.attach_elements(rectangles_ebo);

    rectangles_vao.attach_attribute({ 0, offsetof(sprite_vertex, position), 2, GL_FLOAT, 0 });
    rectangles_vao.attach_attribute({ 1, offsetof(sprite_vertex, texcoord), 2, GL_FLOAT, 0 });

    window_events->callbacks.on_resize = [&](const uint32_t width, const uint32_t height)
    {
        window_width  = width;
        window_height = height;
    };

    core::rgba framebuffer_color { 0.0f, 0.0f, 0.0f, 0.0f };

    graphics::opengl::Commands::clear(0.5f, 0.5f, 0.5f, 1.0f);

    graphics::opengl::Pipeline::disable(GL_MULTISAMPLE);

    // atlas output - begin

    framebuffer.clear(&framebuffer_color.r, 0);
    //framebuffer.clear(1.0f);
    framebuffer.bind();

    graphics::opengl::Commands::viewport(0, 0, atlas_width, atlas_height);

    graphics::opengl::Pipeline::disable(GL_BLEND);

    // rectangles

    sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(atlas_width), static_cast<float>(atlas_height), 0.0f, 0.0f, 1.0f);
    camera_ubo.subdata(0, sizeof(data::camera), &sprite_camera_data);

    sprite_program_shader.bind();
    sprite_sampler.bind(0);

    rectangles_vao.bind();

    for (auto i = 0; i < images_data.size(); i++)
    {
        auto& rectangle = rectangles[i];

        // TODO remove it if not needed later on
        auto parent_matrix = translate(glm::mat4(1.0f), { 0.0, 0.0, 0 });

        auto rectangle_matrix = translate(parent_matrix, { rectangle.x, rectangle.y, 0.0f });
        transform_ubo.subdata(0, sizeof(glm::mat4), &rectangle_matrix);

        graphics::opengl::Texture rectangle_texture; // TODO cache this into a set/map to not recreate it everytime if is the same size
        rectangle_texture.type = GL_TEXTURE_2D;
        rectangle_texture.create();
        rectangle_texture.storage(rectangle.w, rectangle.h, GL_RGBA8, 1);
        rectangle_texture.subdata(rectangle.w, rectangle.h, GL_RGBA, 0, images_data[i].pixels);
        rectangle_texture.bind(0); // TODO use a constant here? like albedo?

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, i * 6, 6);

        rectangle_texture.destroy();
    }

    auto channels = 4;
    auto buffer_size = atlas_width * atlas_height * channels;

    graphics::opengl::Commands::read_pixels(0, 0, atlas_width, atlas_height, GL_RGBA, GL_UNSIGNED_BYTE, buffer_size);

    if (auto* pixels = static_cast<uint8_t*>(framebuffer_pbo.map(0, buffer_size, GL_MAP_READ_BIT)))
    {
        stbi_flip_vertically_on_write(1);
        stbi_write_png((folder_name + "_atlas.png").c_str(), atlas_width, atlas_height, channels, pixels, atlas_width * channels);

        framebuffer_pbo.unmap();
    }

    // atlas output - end

    graphics::opengl::Pipeline::enable(GL_BLEND);
    graphics::opengl::Pipeline::blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3  red_color { 1.0f, 0.0f, 0.0f };
    glm::vec3 blue_color { 0.0f, 0.0f, 1.0f };

    glm::mat4 text_matrix { 1.0f };

    auto default_texture_width  = 1;
    auto default_texture_height = 1;

    uint8_t default_pixels[3] { 255, 255, 255 };

    graphics::opengl::Texture default_texture;
    default_texture.type = GL_TEXTURE_2D;
    default_texture.create();

    default_texture.storage(default_texture_width, default_texture_height, GL_RGB8, 1);
    default_texture.subdata(default_texture_width, default_texture_height, GL_RGB, 0, default_pixels);

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
    debug_vbo.storage(sizeof(editor_vertex) * debug_vertices_capacity, nullptr, GL_DYNAMIC_STORAGE_BIT);

    graphics::opengl::VertexArray debug_vao;
    debug_vao.create();
    debug_vao.attach_vertices(debug_vbo, sizeof(editor_vertex));

    debug_vao.attach_attribute({ 0, offsetof(editor_vertex, position), 3, GL_FLOAT, 0 });
    debug_vao.attach_attribute({ 1, offsetof(editor_vertex, color), 3, GL_FLOAT, 0 });

    // physics end

    // input begin

    static auto show_cube = true;

    glfwSetKeyCallback(std::any_cast<GLFWwindow*>(window->handle()), [](GLFWwindow*, const int key, int, const int action, int)
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
            screen_pos_to_world_ray(static_cast<int>(mouse_x), screen_height - static_cast<int>(mouse_y),
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
        camera_ubo.subdata(0, sizeof(data::camera), &model_camera_data);

        transform_ubo.subdata(0, sizeof(model_matrix), &model_matrix);

        bt_object_transform.setFromOpenGLMatrix(&model_matrix[0].x);

        bt_collision_object->setWorldTransform(bt_object_transform);

        physics.update();

        // clamp to the buffer capacity so we never write past the end of debug_vbo
        const auto debug_vertices_count = std::min<size_t>(physics.debug()->vertices.size(), debug_vertices_capacity);

        debug_vbo.subdata(0, sizeof(core::vec3) * debug_vertices_count, physics.debug()->vertices.data());

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
        transform_ubo.subdata(0, sizeof(collider_matrix), &collider_matrix);

        debug_vao.bind();

        graphics::opengl::Commands::draw_vertices(GL_LINES, 0, static_cast<int32_t>(debug_vertices_count / 2));

        // 2D drawing

        graphics::opengl::Pipeline::disable(GL_DEPTH_TEST);

        sprite_sampler.bind(0);

        // sprites

        sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, 0.0f, 1.0f);
        camera_ubo.subdata(0, sizeof(data::camera), &sprite_camera_data);

        sprite_program_shader.bind();

        framebuffer_texture.bind(0);

        sprite_geometry_vao.bind();

        transform_ubo.subdata(0, sizeof(sprite_matrix), &sprite_matrix);

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, sprite_elements.size());

        // texts

        text_sampler.bind(0);

        text_program_shader.bind();

        text_vao.bind();

        text_matrix = translate(glm::mat4(1.0f), { 200.0f, 700.0f, 0.0f });
        transform_ubo.subdata(0, sizeof(text_matrix), &text_matrix);

        material_ubo.subdata(0, sizeof(glm::vec3), &red_color);
        render_text(text_vbo, "This is sample text");

        text_matrix = translate(glm::mat4(1.0f), { 200.0f, 800.0f, 0.0f });
        text_matrix = scale(text_matrix, { 0.5f, 0.5f, 0.5f });
        transform_ubo.subdata(0, sizeof(text_matrix), &text_matrix);

        material_ubo.subdata(0, sizeof(glm::vec3), &blue_color);
        render_text(text_vbo, "This is a second sample text");

        window_manager.update();
    }

    pavement_texture.destroy();

      text_sampler.destroy();
     model_sampler.destroy();
    sprite_sampler.destroy();

      cube_geometry_vao.destroy();
    sprite_geometry_vao.destroy();

    cube_geometry_vbo.destroy();
    cube_geometry_ebo.destroy();

    sprite_geometry_vbo.destroy();
    sprite_geometry_ebo.destroy();

    model_program_shader.destroy();
    sprite_program_shader.destroy();

    window->destroy();

    return 0;
}

// VERSION 2

// TODO - make a to_base function which wraps std::to_underlying

// VERSION 1

// TODO - use the range buffer binding

// TODO - create a light opengl functions loader

// TODO - decide which version to keep for shader stage type parameter | with the constructor or the function

// TODO - name the shader files as the variables - CLAUDE

// TODO - check and understand why in some cases we need create_sprite with flip_vertically true and sometimes false
// TODO - update all the years for license files 2025 - 2026 in modules and game framework (just in these, no other places)  - CLAUDE

// TODO - remove glfwHideWindow from glfw and all the internal function related to this

// TODO - check if the RoboticsLearning can be removed from bullet together with all the robotics stuff we will not ever use
// TODO - check which modules we will not use at any time in the future in the stb nothings

// TODO - remove unnecessary formats loading from stb_image also from stb_image_write

// TODO - have this #define GL_COLOR_ATTACHMENT0 0x8CE0 as a variable representing the albedo?
// TODO - remove everything (functions) related to GL_POINTS | we will never render points or something like this

// TODO - check glfw to be sure is using just GLFW_OPENGL_CORE_PROFILE

// TODO - resolve the flipping of the y from the ortho matrix (need to recheck all the sprites/fonts)

// TODO - make the on_resize callback to use something from the window (glfwGetWindowSize(glfw_window, &screen_width, &screen_height)) not the size from the framebuffer

// TODO - in case in glfw use use on_resize the window size, remove the framebuffer callbacks from it