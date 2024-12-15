#include "IceCraft/chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>

#include "IceCraft/block.h"

void init_chunk(float x, float z, struct Chunk *chunk, unsigned block_capacity)
{
    chunk->placed_blocks = 0;
    chunk->x = x;
    chunk->z = z;
    chunk->block_capacity = block_capacity;
    chunk->blocks = (struct Block*) malloc(sizeof(struct Block) * chunk->block_capacity);
    chunk->vertices = (struct BlockVertex*) malloc(BLOCK_VERTICES_SIZE * chunk->block_capacity);
    generate_chunk_vao_and_vbo(&chunk->VAO, &chunk->VBO, chunk);
}

void add_block_to_chunk(float global_x, float global_y, float global_z, unsigned texture_id, struct Chunk *chunk)
{
    if (block_does_exist(global_x, global_y, global_z, chunk))
    {
        return;
    }
    
    if (chunk->placed_blocks == chunk->block_capacity)
    {
        printf("Capacity doubling from: %u blocks!\n", chunk->block_capacity);
        struct Block *new_blocks;
        struct BlockVertex *new_vertices;
        chunk->block_capacity *= 2;
        new_blocks = (struct Block*) realloc(chunk->blocks, sizeof(struct Block) * chunk->block_capacity);
        if (!new_blocks)
        {
            fprintf(stderr, "Memory Error: Failed to double capacity of chunk located at (%f | %f) to %u\n", chunk->x, chunk->z, chunk->block_capacity);
            free(chunk->blocks);
            exit(1);
        }
        chunk->blocks = new_blocks;

        new_vertices = (struct BlockVertex*) realloc(chunk->vertices, BLOCK_VERTICES_SIZE * chunk->block_capacity);
        if (!new_vertices)
        {
            fprintf(stderr, "Memory Error: Failed to double capacity of chunk located at (%f | %f) to %u\n", chunk->x, chunk->z, chunk->block_capacity);
            free(chunk->blocks);
            free(chunk->vertices);
            exit(1);
        }
        chunk->vertices = new_vertices;
    }

    chunk->blocks[chunk->placed_blocks] = generate_block(global_x, global_y, global_z, texture_id, chunk->vertices + chunk->placed_blocks*BLOCK_N_VERTICES);

    chunk->placed_blocks++;

    generate_chunk_vao_and_vbo(&chunk->VAO, &chunk->VBO, chunk);
}


void remove_block_from_chunk(unsigned index, struct Chunk *chunk)
{
    if (chunk->placed_blocks == 0 || index >= chunk->placed_blocks) { return; }

    memmove(&chunk->blocks[index], &chunk->blocks[index+1], (chunk->placed_blocks - index - 1) * sizeof(struct Block));

    memmove(&chunk->vertices[index*BLOCK_N_VERTICES], &chunk->vertices[(index+1)*BLOCK_N_VERTICES], (chunk->placed_blocks - index - 1)*BLOCK_N_VERTICES * sizeof(struct BlockVertex));

    chunk->placed_blocks--;

    generate_chunk_vao_and_vbo(&chunk->VAO, &chunk->VBO, chunk);
}

void generate_chunk_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct Chunk *chunk)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, VAO_ptr);
    glBindVertexArray(*VAO_ptr);

    // Generate and bind VBO
    glGenBuffers(1, VBO_ptr);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO_ptr);
    glBufferData(GL_ARRAY_BUFFER, chunk->placed_blocks*BLOCK_VERTICES_SIZE, chunk->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)(3 * sizeof(GLfloat)));  // Texture coordinates
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int block_does_exist(float x, float y, float z, struct Chunk *chunk)
{
    for (unsigned i = 0; i < chunk->placed_blocks; i++)
    {
        struct Block *selected_block = chunk->blocks + i;
        if (selected_block->x == x && selected_block->y == y && selected_block->z == z)
        {
            return 1;
        }
    }

    return 0;
}