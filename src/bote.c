#include "IceCraft/bote.h"

#include "IceCraft/block.h"
#include "IceCraft/point.h"
#include "IceCraft/helper_funcs.h"

#include "stdio.h"

inline static void assign_point_to_vertex(const struct Point *p, struct BlockVertex *v)
{
    *v = *((struct BlockVertex*)p);
}


static void bote_generate_vertices(struct Bote *bote)
{
    bote->n_vertices = 36 * 4;
    bote->vertices = malloc(bote->n_vertices * sizeof(struct BlockVertex));

    if (!bote->vertices)
    {
        fprintf(stderr, "Couldn't allocate memory for vertices of Bote!\n");
        exit(1);
    }

    const float u_min = 1.0f / 3.0f;
    const float u_max = 2.0f / 3.0f;
    const float v_min = 0.0f;
    const float v_max = 0.5f;


    struct Point points_body[8] =
    {
        (struct Point) { +0.4f, +0.75f, +0.2f },
        (struct Point) { +0.4f, +0.75f, -0.2f },
        (struct Point) { -0.4f, +0.75f, -0.2f },
        (struct Point) { -0.4f, +0.75f, +0.2f },
        (struct Point) { +0.4f, -0.75f, +0.2f },
        (struct Point) { +0.4f, -0.75f, -0.2f },
        (struct Point) { -0.4f, -0.75f, -0.2f },
        (struct Point) { -0.4f, -0.75f, +0.2f }
    };

    for (unsigned i = 0; i < 8; i++)
    {
        points_body[i].x += bote->position[0];
        points_body[i].y += bote->position[1];
        points_body[i].z += bote->position[2];
    }

    generate_cube_vertices_from_points(bote->vertices, points_body, u_min, u_max, v_min, v_max);

    struct Point points_head[8] =
    {
        (struct Point) { +0.25f, +0.25f, +0.25f },
        (struct Point) { +0.25f, +0.25f, -0.25f },
        (struct Point) { -0.25f, +0.25f, -0.25f },
        (struct Point) { -0.25f, +0.25f, +0.25f },
        (struct Point) { +0.25f, -0.25f, +0.25f },
        (struct Point) { +0.25f, -0.25f, -0.25f },
        (struct Point) { -0.25f, -0.25f, -0.25f },
        (struct Point) { -0.25f, -0.25f, +0.25f }
    };

    for (unsigned i = 0; i < 8; i++)
    {
        points_head[i].x += bote->position[0];
        points_head[i].y += bote->position[1] + 1.0f;
        points_head[i].z += bote->position[2];
    }

    generate_cube_vertices_from_points(bote->vertices + 36, points_head, u_min, u_max, v_min, v_max);

    struct Point points_leg[8] =
    {
        (struct Point) { +0.15f, +0.7f, +0.15f },
        (struct Point) { +0.15f, +0.7f, -0.15f },
        (struct Point) { -0.15f, +0.7f, -0.15f },
        (struct Point) { -0.15f, +0.7f, +0.15f },
        (struct Point) { +0.15f, -0.7f, +0.15f },
        (struct Point) { +0.15f, -0.7f, -0.15f },
        (struct Point) { -0.15f, -0.7f, -0.15f },
        (struct Point) { -0.15f, -0.7f, +0.15f }
    };

    for (unsigned i = 0; i < 8; i++)
    {
        points_leg[i].x += bote->position[0] - 0.25f;
        points_leg[i].y += bote->position[1] - 1.4f;
        points_leg[i].z += bote->position[2];
    }

    generate_cube_vertices_from_points(bote->vertices + 2*36, points_leg, u_min, u_max, v_min, v_max);

    for (unsigned i = 0; i < 8; i++)
    {
        points_leg[i].x += 0.5f;
    }

    generate_cube_vertices_from_points(bote->vertices + 3*36, points_leg, u_min, u_max, v_min, v_max);
}


void init_bote(struct Bote *bote)
{
    bote->position[0] = 7.0f;
    bote->position[1] = 5.65f;
    bote->position[2] = -7.0f;

    bote_generate_vertices(bote);

    bote_generate_vao_vbo(bote);
}


void bote_generate_vao_vbo(struct Bote *bote)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, &bote->VAO);
    glBindVertexArray(bote->VAO);

    // Generate and bind VBO
    glGenBuffers(1, &bote->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bote->VBO);
    glBufferData(GL_ARRAY_BUFFER, bote->n_vertices * sizeof(struct BlockVertex), bote->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct BlockVertex), (GLvoid*)(3 * sizeof(GLfloat)));  // Texture coordinates
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
