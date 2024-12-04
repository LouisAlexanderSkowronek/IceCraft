#include "IceCraft/chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void init_chunk(struct Chunk *chunk)
{
    chunk->placed_blocks = 0;
    chunk->blocks = (struct Block*) malloc(sizeof(struct Block) * N_BLOCKS_PER_CHUNK);
    chunk->vertices = (struct BlockVertex*) malloc(BLOCK_VERTICES_SIZE * N_BLOCKS_PER_CHUNK);
}

void add_block_to_chunk(float x, float y, float z, unsigned texture_id, struct Chunk *chunk)
{
    chunk->blocks[chunk->placed_blocks] = generate_block(x, y, z, texture_id, chunk->vertices + chunk->placed_blocks*BLOCK_N_VERTICES);
    chunk->placed_blocks++;
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
