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
