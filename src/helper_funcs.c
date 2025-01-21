#include "IceCraft/helper_funcs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Couldn't open file '%s'!\n", filename);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    const unsigned file_size = ftell(file);
    rewind(file);

    char *content = (char*) malloc((file_size+1) * sizeof(char));
    if (!content)
    {
        fprintf(stderr, "Couldn't allocate %u bytes for reading file '%s'!\n", file_size+1, filename);
        exit(1);
    }

    fread(content, sizeof(char), file_size, file);
    content[file_size] = '\0';
    fclose(file);

    return content;
}


char **split_string(const char *string, char delimiter, int *count)
{
    if (!string)
    {
        fprintf(stderr, "Couldn't split string '%s', as it's NULL!\n", string);
        exit(1);
    }

    char *str_copy = strdup(string);
    if (!str_copy)
    {
        fprintf(stderr, "Couldn't split string '%s', as duplicating it failed!\n", string);
        exit(1);
    }

    int capacity = 10;
    char **tokens = (char**) malloc(capacity * sizeof(char*));
    if (!tokens)
    {
        free(str_copy);
        fprintf(stderr, "Couldn't split string '%s', as malloc for the tokens of capacity %d failed!\n", string, capacity);
        exit(1);
    }

    int token_count = 0;
    char delim[2] = { delimiter, '\0' };

    char *token = strtok(str_copy, delim);
    while (NULL != token)
    {
        if (token_count >= capacity)
        {
            capacity *= 2;
            char **temp = (char**) realloc(tokens, capacity * sizeof(char*));
            if (!temp)
            {
                free(tokens);
                free(str_copy);
                fprintf(stderr, "Couldn't split string '%s', as extending the capacity to %d tokens failed!\n", string, capacity);
                exit(1);
            }
            tokens = temp;
        }

        tokens[token_count++] = strdup(token);
        token = strtok(NULL, delim);
    }

    free(str_copy);

    char **result = (char**) realloc(tokens, token_count * sizeof(char*));
    if (result)
    {
        tokens = result;
    }

    *count = token_count;

    return tokens;
}


void free_tokens(char **tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}


unsigned load_jpg_texture(const char *filename)
{
    int texture_width, texture_height, texture_n_channels;
    unsigned char *texture_data = stbi_load(filename, &texture_width, &texture_height, &texture_n_channels, 0);

    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    return texture;
}


inline static void assign_point_to_vertex(const struct Point *p, struct BlockVertex *v)
{
    *v = *((struct BlockVertex*)p);
}


void generate_cube_vertices_from_points(struct BlockVertex *vertices, const struct Point *points, float u_min, float u_max, float v_min, float v_max)
{
    /*
       C - - - - B
      /|        /|
     / |       / |
    D -|- - - A  |
    |  |      |  |
    |  G - - -|- F
    | /       | /
    |/        |/
    H - - - - E
    */

    const struct Point A = points[0];
    const struct Point B = points[1];
    const struct Point C = points[2];
    const struct Point D = points[3];
    const struct Point E = points[4];
    const struct Point F = points[5];
    const struct Point G = points[6];
    const struct Point H = points[7];


    // CFG

    assign_point_to_vertex(&C, vertices+0);
    vertices[0].u = u_max;
    vertices[0].v = v_max;

    assign_point_to_vertex(&G, vertices+1);
    vertices[1].u = u_max;
    vertices[1].v = v_min;

    assign_point_to_vertex(&F, vertices+2);
    vertices[2].u = u_min;
    vertices[2].v = v_min;


    // BCF

    assign_point_to_vertex(&F, vertices+3);
    vertices[3].u = u_min;
    vertices[3].v = v_min;

    assign_point_to_vertex(&B, vertices+4);
    vertices[4].u = u_min;
    vertices[4].v = v_max;

    assign_point_to_vertex(&C, vertices+5);
    vertices[5].u = u_max;
    vertices[5].v = v_max;


    // DEH

    assign_point_to_vertex(&D, vertices+6);
    vertices[6].u = u_min;
    vertices[6].v = v_max;

    assign_point_to_vertex(&H, vertices+7);
    vertices[7].u = u_min;
    vertices[7].v = v_min;

    assign_point_to_vertex(&E, vertices+8);
    vertices[8].u = u_max;
    vertices[8].v = v_min;


    // ADE

    assign_point_to_vertex(&E, vertices+9);
    vertices[9].u = u_max;
    vertices[9].v = v_min;

    assign_point_to_vertex(&A, vertices+10);
    vertices[10].u = u_max;
    vertices[10].v = v_max;

    assign_point_to_vertex(&D, vertices+11);
    vertices[11].u = u_min;
    vertices[11].v = v_max;


    // FGH

    assign_point_to_vertex(&H, vertices+12);
    vertices[12].u = u_min;
    vertices[12].v = v_max;

    assign_point_to_vertex(&G, vertices+13);
    vertices[13].u = u_min;
    vertices[13].v = v_min;

    assign_point_to_vertex(&F, vertices+14);
    vertices[14].u = u_max;
    vertices[14].v = v_min;


    // EFH

    assign_point_to_vertex(&F, vertices+15);
    vertices[15].u = u_max;
    vertices[15].v = v_min;

    assign_point_to_vertex(&E, vertices+16);
    vertices[16].u = u_max;
    vertices[16].v = v_max;

    assign_point_to_vertex(&H, vertices+17);
    vertices[17].u = u_min;
    vertices[17].v = v_max;


    // BCD

    assign_point_to_vertex(&D, vertices+18);
    vertices[18].u = u_min;
    vertices[18].v = v_max;

    assign_point_to_vertex(&C, vertices+19);
    vertices[19].u = u_min;
    vertices[19].v = v_min;

    assign_point_to_vertex(&B, vertices+20);
    vertices[20].u = u_max;
    vertices[20].v = v_min;


    // ABD

    assign_point_to_vertex(&B, vertices+21);
    vertices[21].u = u_max;
    vertices[21].v = v_min;

    assign_point_to_vertex(&A, vertices+22);
    vertices[22].u = u_max;
    vertices[22].v = v_max;

    assign_point_to_vertex(&D, vertices+23);
    vertices[23].u = u_min;
    vertices[23].v = v_max;


    // CGH

    assign_point_to_vertex(&C, vertices+24);
    vertices[24].u = u_min;
    vertices[24].v = v_max;

    assign_point_to_vertex(&G, vertices+25);
    vertices[25].u = u_min;
    vertices[25].v = v_min;

    assign_point_to_vertex(&H, vertices+26);
    vertices[26].u = u_max;
    vertices[26].v = v_min;


    // CDH

    assign_point_to_vertex(&C, vertices+27);
    vertices[27].u = u_max;
    vertices[27].v = v_min;

    assign_point_to_vertex(&D, vertices+28);
    vertices[28].u = u_max;
    vertices[28].v = v_max;

    assign_point_to_vertex(&H, vertices+29);
    vertices[29].u = u_min;
    vertices[29].v = v_max;


    // BEF

    assign_point_to_vertex(&B, vertices+30);
    vertices[30].u = u_min;
    vertices[30].v = v_max;

    assign_point_to_vertex(&F, vertices+31);
    vertices[31].u = u_min;
    vertices[31].v = v_min;

    assign_point_to_vertex(&E, vertices+32);
    vertices[32].u = u_max;
    vertices[32].v = v_min;


    // ABE

    assign_point_to_vertex(&B, vertices+33);
    vertices[33].u = u_max;
    vertices[33].v = v_min;

    assign_point_to_vertex(&A, vertices+34);
    vertices[34].u = u_max;
    vertices[34].v = v_max;

    assign_point_to_vertex(&E, vertices+35);
    vertices[35].u = u_min;
    vertices[35].v = v_max;
}