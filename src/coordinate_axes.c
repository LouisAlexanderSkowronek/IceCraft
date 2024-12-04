#include "IceCraft/coordinate_axes.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void generate_coordinate_axes(struct CoordinateAxes *coord_axes)
{
    for (unsigned i = 0; i < COORDINATE_AXES_N_VERTICES; i+=2)
    {
        coord_axes->vertices[i].x = coord_axes->vertices[i].y = coord_axes->vertices[i].z = 0.0f;
        coord_axes->vertices[i].r = (i == 0) ? 1.0f : 0.0f;
        coord_axes->vertices[i].g = (i == 2) ? 1.0f : 0.0f;
        coord_axes->vertices[i].b = (i == 4) ? 1.0f : 0.0f;
    }

    for (unsigned i = 1; i < COORDINATE_AXES_N_VERTICES; i+=2)
    {
        coord_axes->vertices[i].x = (i == 1) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].y = (i == 3) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].z = (i == 5) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].r = (i == 1) ? 1.0f : 0.0f;
        coord_axes->vertices[i].g = (i == 3) ? 1.0f : 0.0f;
        coord_axes->vertices[i].b = (i == 5) ? 1.0f : 0.0f;
    }
}


void generate_coord_axes_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct CoordinateAxes *coord_axes)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, VAO_ptr);
    glBindVertexArray(*VAO_ptr);

    // Generate and bind VBO
    glGenBuffers(1, VBO_ptr);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO_ptr);
    glBufferData(GL_ARRAY_BUFFER, COORDINATE_AXES_NVERTICES_SIZE, coord_axes->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct LineVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct LineVertex), (GLvoid*)(3 * sizeof(GLfloat)));  // Color attributes
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}