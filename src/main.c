#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cglm/cglm.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "IceCraft/camera.h"
#include "IceCraft/block_vertex.h"
#include "IceCraft/block.h"
#include "IceCraft/coordinate_axes.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"


void init_glfw();

void init_glad();

GLFWwindow* create_window(unsigned width, unsigned height, const char *title);

unsigned load_jpg_texture(const char *filename);

void generate_block_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct Block *block_ptr);

void generate_coord_axes_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct CoordinateAxes *coord_axes_ptr);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned build_shader_program(const char *vertex_shader_filepath, const char *fragment_shader_filepath);

void processInput(GLFWwindow *window, struct Camera *camera, int *show_coordinate_axes, int *c_key_is_blocked, const float delta);


int main()
{
    init_glfw();

    GLFWwindow *window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    init_glad();

    glEnable(GL_DEPTH_TEST);

    unsigned block_shader_program = build_shader_program("../shaders/block_vertex_shader.glsl", "../shaders/block_fragment_shader.glsl");

    unsigned coord_axes_shader_program = build_shader_program("../shaders/coord_axes_vertex_shader.glsl", "../shaders/coord_axes_fragment_shader.glsl");

    unsigned texture = load_jpg_texture("../assets/textures/ice_cheese.jpg");

    struct Camera camera;
    init_camera(&camera);

    int show_coordinate_axes = 0;
    int c_key_is_blocked = 0;

    struct Block block = generate_block(0.0f, 0.0f, 0.0f, 1.0f);

    struct CoordinateAxes coordinate_axes;
    generateCoordinateAxes(&coordinate_axes);

    GLuint block_VBO, block_VAO;
    generate_block_vao_and_vbo(&block_VAO, &block_VBO, &block);

    GLuint coord_axes_VBO, coord_axes_VAO;
    generate_coord_axes_vao_and_vbo(&coord_axes_VAO, &coord_axes_VBO, &coordinate_axes);

    const GLuint block_model_location = glGetUniformLocation(block_shader_program, "model");
    const GLuint block_view_location  = glGetUniformLocation(block_shader_program, "view");
    const GLuint block_projection_location = glGetUniformLocation(block_shader_program, "projection");

    const GLuint coord_axes_model_location = glGetUniformLocation(coord_axes_shader_program, "model");
    const GLuint coord_axes_view_location  = glGetUniformLocation(coord_axes_shader_program, "view");
    const GLuint coord_axes_projection_location = glGetUniformLocation(coord_axes_shader_program, "projection");


    mat4 view, projection;
    glm_perspective(glm_rad(45.0f), ((float) WINDOW_WIDTH) / ((float) WINDOW_HEIGHT), 0.1f, 100.0f, projection);

    unsigned frames_since_last_update = 0;
    double last_time = glfwGetTime();
    double time_of_last_update = last_time;
    const double update_timeout = 0.5;

    while (!glfwWindowShouldClose(window))
    {
        double current_time = glfwGetTime();
        const double delta = current_time - last_time;
        last_time = current_time;

        if (current_time >= time_of_last_update + update_timeout)
        {
            double average_fps = frames_since_last_update / update_timeout;
            const size_t title_size = 256;
            char title[title_size];
            snprintf(title, title_size, WINDOW_TITLE " | %.2f fps", average_fps);

            glfwSetWindowTitle(window, title);

            frames_since_last_update = 0;
            time_of_last_update = current_time;
        }

        processInput(window, &camera, &show_coordinate_axes, &c_key_is_blocked, delta);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(block_shader_program);

        vec3 camera_target;
        glm_vec3_add(camera.position, camera.front, camera_target);
        glm_lookat(camera.position, camera_target, camera.up, view);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        glUniformMatrix4fv(block_model_location, 1, GL_FALSE, (float*)model);
        glUniformMatrix4fv(block_view_location, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(block_projection_location, 1, GL_FALSE, (float*)projection);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(block_VAO);
        glDrawArrays(GL_TRIANGLES, 0, BLOCK_N_VERTICES);


        if (show_coordinate_axes)
        {
            glUseProgram(coord_axes_shader_program);

            glUniformMatrix4fv(coord_axes_model_location, 1, GL_FALSE, (float*)model);
            glUniformMatrix4fv(coord_axes_view_location, 1, GL_FALSE, (float*)view);
            glUniformMatrix4fv(coord_axes_projection_location, 1, GL_FALSE, (float*)projection);

            glBindVertexArray(coord_axes_VAO);
            glDrawArrays(GL_LINES, 0, COORDINATE_AXES_N_VERTICES);
        }
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        frames_since_last_update++;
    }

    glDeleteVertexArrays(1, &block_VAO);
    glDeleteBuffers(1, &block_VBO);

    glDeleteVertexArrays(1, &coord_axes_VAO);
    glDeleteBuffers(1, &coord_axes_VBO);

    glDeleteProgram(block_shader_program);
    glDeleteProgram(coord_axes_shader_program);

    free(block.vertices);

    glfwTerminate();
    return 0;
}


void init_glfw()
{
    if (!glfwInit())
    {
        const char *description;
        int code = glfwGetError(&description);
        if (description)
        {
            fprintf(stderr, "GLFW initialization failed with error code %d: %s\n", code, description);
        } else
        {
            fprintf(stderr, "GLFW initialization failed with error code %d\n", code);
        }

        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}


void init_glad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fputs("Failed to initialize GLAD!", stderr);
        exit(1);
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


GLFWwindow* create_window(unsigned width, unsigned height, const char *title)
{
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        fputs("Failed to create GLFW window\n", stderr);
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSwapInterval(0);

    return window;
}


unsigned load_jpg_texture(const char *filename)
{
    int texture_width, texture_height, texture_n_channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *texture_data = stbi_load(filename, &texture_width, &texture_height, &texture_n_channels, 0);
    stbi_set_flip_vertically_on_load(0);

    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    return texture;
}


void generate_block_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct Block *block_ptr)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, VAO_ptr);
    glBindVertexArray(*VAO_ptr);

    // Generate and bind VBO
    glGenBuffers(1, VBO_ptr);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO_ptr);
    glBufferData(GL_ARRAY_BUFFER, BLOCK_VERTICES_SIZE, block_ptr->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)(3 * sizeof(GLfloat)));  // Texture coordinates
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void generate_coord_axes_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct CoordinateAxes *coord_axes_ptr)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, VAO_ptr);
    glBindVertexArray(*VAO_ptr);

    // Generate and bind VBO
    glGenBuffers(1, VBO_ptr);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO_ptr);
    glBufferData(GL_ARRAY_BUFFER, COORDINATE_AXES_NVERTICES_SIZE, coord_axes_ptr->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct LineVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct LineVertex), (GLvoid*)(3 * sizeof(GLfloat)));  // Color attributes
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void processInput(GLFWwindow *window, struct Camera *camera, int *show_coordinate_axes, int *c_key_is_blocked, const float delta)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !(*c_key_is_blocked))
    {
        *show_coordinate_axes = !(*show_coordinate_axes);
        *c_key_is_blocked = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        *c_key_is_blocked = 0;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ camera->front[0] * CAMERA_SPEED * delta, camera->front[1] * CAMERA_SPEED * delta, camera->front[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ -camera->front[0] * CAMERA_SPEED * delta, -camera->front[1] * CAMERA_SPEED * delta, -camera->front[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera->front, camera->up, right);
        glm_normalize(right);
        move_camera(camera, (vec3){ -right[0] * CAMERA_SPEED * delta, -right[1] * CAMERA_SPEED * delta, -right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera->front, camera->up, right);
        glm_normalize(right);
        move_camera(camera, (vec3){ right[0] * CAMERA_SPEED * delta, right[1] * CAMERA_SPEED * delta, right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ 0.0f, CAMERA_SPEED * delta, 0.0f });
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ 0.0f, -CAMERA_SPEED * delta, 0.0f });
    }


    vec2 camera_rotation_offset = { 0.0f, 0.0f };

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera_rotation_offset[0] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera_rotation_offset[0] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    rotate_camera(camera, camera_rotation_offset);
}


unsigned build_shader_program(const char *vertex_shader_filepath, const char *fragment_shader_filepath)
{
    FILE *shader_file = fopen(vertex_shader_filepath, "r");
    if (!shader_file)
    {
        fputs("Vertex shader not found!", stderr);
        exit(1);
    }

    fseek(shader_file, 0L, SEEK_END);
    unsigned file_size = ftell(shader_file);
    rewind(shader_file);

    char *vertex_shader_source = malloc((file_size+1) * sizeof(char));
    if (!vertex_shader_source)
    {
        fprintf(stderr, "Couldn't allocate %u bytes for reading vertex shader!\n", file_size+1);
        exit(1);
    }

    fread(vertex_shader_source, sizeof(char), file_size, shader_file);
    vertex_shader_source[file_size] = '\0';
    fclose(shader_file);

    shader_file = fopen(fragment_shader_filepath, "r");
    if (!shader_file)
    {
        fputs("Fragment shader not found!", stderr);
        exit(1);
    }

    fseek(shader_file, 0L, SEEK_END);
    file_size = ftell(shader_file);
    rewind(shader_file);

    char *fragment_shader_source = malloc((file_size+1) * sizeof(char));
    if (!fragment_shader_source)
    {
        fprintf(stderr, "Couldn't allocate %u bytes for reading fragment shader!\n", file_size+1);
        exit(1);
    }

    fread(fragment_shader_source, sizeof(char), file_size, shader_file);
    fragment_shader_source[file_size] = '\0';
    fclose(shader_file);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (char const * const *)&vertex_shader_source, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (char const * const *)&fragment_shader_source, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    free(fragment_shader_source);
    free(vertex_shader_source);

    return shaderProgram;
}
