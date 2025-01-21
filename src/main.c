#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "cglm/cglm.h"

#include "IceCraft/camera.h"
#include "IceCraft/block_vertex.h"
#include "IceCraft/block.h"
#include "IceCraft/coordinate_axes.h"
#include "IceCraft/chunk.h"
#include "IceCraft/helper_funcs.h"
#include "IceCraft/opengl_utils.h"
#include "IceCraft/input_handler.h"
#include "IceCraft/world.h"
#include "IceCraft/gui_block_selector.h"
#include "IceCraft/gui_crossbar.h"
#include "IceCraft/player.h"
//#include "IceCraft/bote.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"
#define MSG_BUFFER_SIZE 1024

#ifndef NTOHLL
#define NTOHLL(x) ((((uint64_t)ntohl((x) & 0xFFFFFFFF)) << 32) | ntohl((x) >> 32))
#endif

void parse_and_place_block(const char *command, struct Chunk *chunk)
{
    const char *asserted_prefix = "place ";
    const unsigned prefix_length = strlen(asserted_prefix);

    if (strncmp(command, asserted_prefix, prefix_length) != 0)
    {
        fprintf(stderr, "parse_and_place_block function used with a command different from 'place': %s\n", command);
        exit(EXIT_FAILURE);
    }

    command += prefix_length;

    unsigned idx_in_buffer = 0;
    char buffer[16];
    buffer[0] = '\0';

    float x, y, z;
    unsigned material_id;

    unsigned currently_parsing = 0;  // 0: x, 1: y, 2: z, 3: material_id

    while (*command != '\0')
    {
        if (*command == '.')
        {
            command++;
            continue;
        }

        if (isdigit(*command) || *command == '-')
        {
            buffer[idx_in_buffer++] = *command;
            buffer[idx_in_buffer] = '\0';
        } else if (*command == ' ')
        {
            switch (currently_parsing++)
            {
                case 0:
                    x = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
                
                case 1:
                    y = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
                
                case 2:
                    z = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
                
                case 3:
                    material_id = strtoul(buffer, NULL, 10);
            
                default:
                    fprintf(stderr, "Invalid value of 'currently_parsing' in parse_and_place_block function: %u\n", currently_parsing);
                    exit(EXIT_FAILURE);
            }
        }

        command++;
    }

    material_id = strtoul(buffer, NULL, 10);

    add_block_to_chunk(x, y, z, material_id, chunk);
}


void parse_and_destroy_block(const char *command, struct Chunk *chunk)
{
    const char *asserted_prefix = "destroy ";
    const unsigned prefix_length = strlen(asserted_prefix);

    if (strncmp(command, asserted_prefix, prefix_length) != 0)
    {
        fprintf(stderr, "parse_and_destroy_block function used with a command different from 'destroy': %s\n", command);
        exit(EXIT_FAILURE);
    }

    command += prefix_length;

    unsigned idx_in_buffer = 0;
    char buffer[16];
    buffer[0] = '\0';

    float x, y, z;

    unsigned currently_parsing = 0;  // 0: x, 1: y, 2: z

    while (*command != '\0')
    {
        if (*command == '.')
        {
            command++;
            continue;
        }

        if (isdigit(*command) || *command == '-')
        {
            buffer[idx_in_buffer++] = *command;
            buffer[idx_in_buffer] = '\0';
        } else if (*command == ' ')
        {
            switch (currently_parsing++)
            {
                case 0:
                    x = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
                
                case 1:
                    y = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
                
                case 2:
                    z = atof(buffer);
                    buffer[0] = '\0';
                    idx_in_buffer = 0;
                    break;
            
                default:
                    fprintf(stderr, "Invalid value of 'currently_parsing' in parse_and_place_block function: %u\n", currently_parsing);
                    exit(EXIT_FAILURE);
            }
        }

        command++;
    }

    z = atof(buffer);

    if (!block_does_exist(x, y, z, chunk))
    {
        printf("[WARNING] Tried to destroy block at (%f | %f | %f) that doesn't exist.\n", x, y, z);
        return;
    }

    unsigned index = get_block_located_at(x, y, z, chunk);

    remove_block_from_chunk(index, chunk);

}


void update_chunk_from_server(int sock, struct Chunk *chunk)
{
    uint64_t mods_data_size;
    if (recv(sock, &mods_data_size, sizeof(mods_data_size), 0) != sizeof(mods_data_size))
    {
        fprintf(stderr, "Failed to recv mods_data_size!\n");
        exit(EXIT_FAILURE);
    }

    mods_data_size = NTOHLL(mods_data_size);

    char *mods_buffer = malloc(mods_data_size);
    if (!mods_buffer)
    {
        fprintf(stderr, "Couldn't allocate %lu bytes for mods buffer!\n", mods_data_size);
        exit(EXIT_FAILURE);
    }

    size_t mods_received = 0;
    while (mods_received < mods_data_size)
    {
        ssize_t modchunk = recv(sock, mods_buffer + mods_received, mods_data_size - mods_received, 0);
        if (modchunk <= 0) {
            free(mods_buffer);
            fprintf(stderr, "Failed to receive data!\n");
            exit(EXIT_FAILURE);
        }
        mods_received += modchunk;
    }

    char *command = strtok(mods_buffer, "\n");

    while (command != NULL)
    {
        const char *place_cmd = "place ";
        const char *destroy_cmd = "destroy ";

        if (strncmp(place_cmd, command, strlen(place_cmd)) == 0)
        {
            parse_and_place_block(command, chunk);
        } else if (strncmp(destroy_cmd, command, strlen(destroy_cmd)) == 0)
        {
            parse_and_destroy_block(command, chunk);
        }
        command = strtok(NULL, "\n");
    }

    free(mods_buffer);
}


int gravity_enabled = 1;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "You can't use IceCraft alone ...\n");
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        fprintf(stderr, "Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Connection to server failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server at %s:%d\n", server_ip, server_port);

    char msg_buffer[1024];

    init_glfw();

    GLFWwindow *window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    init_glad();

    glEnable(GL_DEPTH_TEST);

    unsigned world_shader_program = build_shader_program("../shaders/world/vertex_shader.glsl", "../shaders/world/fragment_shader.glsl");

    unsigned coord_axes_shader_program = build_shader_program("../shaders/coord_axes/vertex_shader.glsl", "../shaders/coord_axes/fragment_shader.glsl");

    unsigned hud_shader_program = build_shader_program("../shaders/hud/vertex_shader.glsl", "../shaders/hud/fragment_shader.glsl");

    unsigned texture_atlas = load_jpg_texture("../assets/textures/textures.jpg");

    struct Player louis;
    init_player(&louis);
    int space_key_is_blocked = 0;

    //struct Bote bote;
    //init_bote(&bote);

    int show_coordinate_axes = 0;
    int c_key_is_blocked = 0;

    struct World world;
    generate_flat_world(&world);

    struct CoordinateAxes coordinate_axes;
    generate_coordinate_axes(&coordinate_axes);

    struct GUIBlockSelector block_selector;
    generate_hud(&block_selector);

    struct GUICrossbar crossbar;
    generate_crossbar(&crossbar);

    const GLuint world_model_location = glGetUniformLocation(world_shader_program, "model");
    const GLuint world_view_location  = glGetUniformLocation(world_shader_program, "view");
    const GLuint world_projection_location = glGetUniformLocation(world_shader_program, "projection");

    const GLuint coord_axes_model_location = glGetUniformLocation(coord_axes_shader_program, "model");
    const GLuint coord_axes_view_location  = glGetUniformLocation(coord_axes_shader_program, "view");
    const GLuint coord_axes_projection_location = glGetUniformLocation(coord_axes_shader_program, "projection");


    mat4 view, projection;
    glm_perspective(glm_rad(45.0f), ((float) WINDOW_WIDTH) / ((float) WINDOW_HEIGHT), 0.1f, 100.0f, projection);

    unsigned frames_since_last_update = 0;
    double last_time = glfwGetTime();
    double time_of_last_update = last_time;
    const double update_timeout = 0.5;

    glBindTexture(GL_TEXTURE_2D, texture_atlas);

    double remaining_time_next_print = 0.0;
    double remaining_time_next_update_from_server = 0.0;

    update_chunk_from_server(sock, world.chunk);

    while (!glfwWindowShouldClose(window))
    {
        const double current_time = glfwGetTime();
        const double delta = current_time - last_time;
        last_time = current_time;
        remaining_time_next_print -= delta;
        remaining_time_next_update_from_server -= delta;

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

        if (remaining_time_next_update_from_server <= 0.0)
        {
            const char *update_msg = "update";
            const unsigned update_msg_length = strlen(update_msg);

            if (send(sock, update_msg, update_msg_length, 0) == -1)
            {
                fprintf(stderr, "Failed to send message: %s\n", update_msg);
                exit(EXIT_FAILURE);
            }

            update_chunk_from_server(sock, world.chunk);

            remaining_time_next_update_from_server = 0.1;
        } else
        {
            processInput(window, &block_selector, &louis, sock, &world, &show_coordinate_axes, &c_key_is_blocked, &space_key_is_blocked, delta);
        }

        update_player(&louis, world.chunk, delta);


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(world_shader_program);

        vec3 camera_target;
        glm_vec3_add(louis.camera.position, louis.camera.front, camera_target);
        glm_lookat(louis.camera.position, camera_target, louis.camera.up, view);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        glUniformMatrix4fv(world_model_location, 1, GL_FALSE, (float*)model);
        glUniformMatrix4fv(world_view_location, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(world_projection_location, 1, GL_FALSE, (float*)projection);

        glBindVertexArray(world.chunk->VAO);
        glDrawArrays(GL_TRIANGLES, 0, world.chunk->placed_blocks*BLOCK_N_VERTICES);

        //glBindVertexArray(bote.VAO);
        //glDrawArrays(GL_TRIANGLES, 0, bote.n_vertices * sizeof(struct BlockVertex));

        if (show_coordinate_axes)
        {
            glUseProgram(coord_axes_shader_program);

            glUniformMatrix4fv(coord_axes_model_location, 1, GL_FALSE, (float*)model);
            glUniformMatrix4fv(coord_axes_view_location, 1, GL_FALSE, (float*)view);
            glUniformMatrix4fv(coord_axes_projection_location, 1, GL_FALSE, (float*)projection);

            glBindVertexArray(coordinate_axes.VAO);
            glDrawArrays(GL_LINES, 0, COORDINATE_AXES_N_VERTICES);
        }

        glUseProgram(hud_shader_program);
        glBindVertexArray(block_selector.VAO);
        glDrawArrays(GL_TRIANGLES, 0, HUD_N_VERTICES);
        glBindVertexArray(crossbar.VAO);
        glDrawArrays(GL_TRIANGLES, 0, GUI_CROSSBAR_N_VERTICES);
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        frames_since_last_update++;
    }

    close(sock);
    printf("Disconnected from server.\n");

    glDeleteVertexArrays(1, &world.chunk->VAO);
    glDeleteBuffers(1, &world.chunk->VBO);

    glDeleteVertexArrays(1, &coordinate_axes.VAO);
    glDeleteBuffers(1, &coordinate_axes.VBO);

    //glDeleteVertexArrays(1, &bote.VAO);
    //glDeleteBuffers(1, &bote.VBO);
    //free(bote.vertices);

    glDeleteProgram(world_shader_program);
    glDeleteProgram(coord_axes_shader_program);

    world_free_chunk(&world);

    glfwTerminate();
    return 0;
}
