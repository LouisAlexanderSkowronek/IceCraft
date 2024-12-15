#include "IceCraft/block.h"

#include <math.h>

#include "IceCraft/point.h"


struct Block generate_block(float x, float y, float z, unsigned material_id, struct BlockVertex *vertices)
{
    struct Block block;
    block.texture_id = material_id;
    block.vertices = vertices;

    
    float u_min, u_max, v_min, v_max;

    switch (material_id)
    {
        case 0:
            u_min = 1.0f / 3.0f;
            u_max = 2.0f / 3.0f;
            v_min = 0.0f;
            v_max = 0.5f;
            break;
        case 1:
            u_min = 0.0f;
            u_max = 1.0f / 3.0f;
            v_min = 0.5f;
            v_max = 1.0f;
            break;
        case 2:
            u_min = 1.0f / 3.0f;
            u_max = 2.0f / 3.0f;
            v_min = 0.5f;
            v_max = 1.0f;
            break;
        case 3:
            u_min = 0.0f;
            u_max = 1.0f / 3.0f;
            v_min = 0.0f;
            v_max = 0.5f;
            break;
        case 4:
            u_min = 2.0f / 3.0f;
            u_max = 1.0f;
            v_min = 0.0f;
            v_max = 0.5f;
            break;
        case 5:
            u_min = 2.0f / 3.0f;
            u_max = 1.0f;
            v_min = 0.5f;
            v_max = 1.0f;
            break;
    }

    // Rückseite, unteres Dreieck

    // A
    vertices[0].x = -1;
    vertices[0].y = 1;
    vertices[0].z = -1;
    vertices[0].u = 0.0f;
    vertices[0].v = 1.0f;

    // B
    vertices[1].x = -1;
    vertices[1].y = -1;
    vertices[1].z = -1;
    vertices[1].u = 0.0f;
    vertices[1].v = 0.0f;

    // C
    vertices[2].x = 1;
    vertices[2].y = -1;
    vertices[2].z = -1;
    vertices[2].u = 1.0f;
    vertices[2].v = 0.0f;


    // rückseite, oberes Dreieck

    vertices[3].x=1;
    vertices[3].y=-1;
    vertices[3].z=-1;
    vertices[3].u=1;
    vertices[3].v=0;

    vertices[4].x=1;
    vertices[4].y=1;
    vertices[4].z=-1;
    vertices[4].u=1;
    vertices[4].v=1;

    vertices[5] = vertices[0];


    // Vordersetite, unteres Dreieck

    // A
    vertices[6].x = -1;
    vertices[6].y = 1;
    vertices[6].z = 1;
    vertices[6].u = 0.0f;
    vertices[6].v = 1.0f;

    // B
    vertices[7].x = -1;
    vertices[7].y = -1;
    vertices[7].z = 1;
    vertices[7].u = 0.0f;
    vertices[7].v = 0.0f;

    // C
    vertices[8].x = 1;
    vertices[8].y = -1;
    vertices[8].z = 1;
    vertices[8].u = 1.0f;
    vertices[8].v = 0.0f;////


    // Vorderseite, oberes Dreieck

    vertices[9].x=1;
    vertices[9].y=-1;
    vertices[9].z=1;
    vertices[9].u=1;
    vertices[9].v=0;

    vertices[10].x=1;
    vertices[10].y=1;
    vertices[10].z=1;
    vertices[10].u=1;
    vertices[10].v=1;

    vertices[11] = vertices[6];

    // A, unteres Dreieck, unten
    vertices[12].x = -1;
    vertices[12].y = -1;
    vertices[12].z = 1;
    vertices[12].u = 0.0f;
    vertices[12].v = 1.0f;

    // B
    vertices[13].x = -1;
    vertices[13].y = -1;
    vertices[13].z = -1;
    vertices[13].u = 0.0f;
    vertices[13].v = 0.0f;

    // C
    vertices[14].x = 1;
    vertices[14].y = -1;
    vertices[14].z = -1;
    vertices[14].u = 1.0f;
    vertices[14].v = 0.0f;

    vertices[15].x=1;
    vertices[15].y=-1;
    vertices[15].z=-1;
    vertices[15].u=1;
    vertices[15].v=0;

    vertices[16].x=1;
    vertices[16].y=-1;
    vertices[16].z=1;
    vertices[16].u=1;
    vertices[16].v=1;

    vertices[17].x = -1;
    vertices[17].y = -1;
    vertices[17].z = 1;
    vertices[17].u = 0.0f;
    vertices[17].v = 1.0f;

    // Top, A
    vertices[18].x = -1;
    vertices[18].y = 1;
    vertices[18].z = 1;
    vertices[18].u = 0.0f;
    vertices[18].v = 1.0f;

    // B
    vertices[19].x = -1;
    vertices[19].y = 1;
    vertices[19].z = -1;
    vertices[19].u = 0.0f;
    vertices[19].v = 0.0f;

    // C
    vertices[20].x = 1;
    vertices[20].y = 1;
    vertices[20].z = -1;
    vertices[20].u = 1.0f;
    vertices[20].v = 0.0f;

    vertices[21].x=1;
    vertices[21].y=1;
    vertices[21].z=-1;
    vertices[21].u=1;
    vertices[21].v=0;

    vertices[22].x=1;
    vertices[22].y=1;
    vertices[22].z=1;
    vertices[22].u=1;
    vertices[22].v=1;

    vertices[23].x = -1;
    vertices[23].y = 1;
    vertices[23].z = 1;
    vertices[23].u = 0.0f;
    vertices[23].v = 1.0f;

    // Left, A
    vertices[24].x = -1;
    vertices[24].y = 1;
    vertices[24].z = -1;
    vertices[24].u = 0.0f;
    vertices[24].v = 1.0f;

    // B
    vertices[25].x = -1;
    vertices[25].y = -1;
    vertices[25].z = -1;
    vertices[25].u = 0.0f;
    vertices[25].v = 0.0f;

    // C
    vertices[26].x = -1;
    vertices[26].y = -1;
    vertices[26].z = 1;
    vertices[26].u = 1.0f;
    vertices[26].v = 0.0f;

    vertices[27].x=-1;
    vertices[27].y=1;
    vertices[27].z=-1;
    vertices[27].u=1;
    vertices[27].v=0;

    vertices[28].x=-1;
    vertices[28].y=1;
    vertices[28].z=1;
    vertices[28].u=1;
    vertices[28].v=1;

    vertices[29].x = -1;
    vertices[29].y = -1;
    vertices[29].z = 1;
    vertices[29].u = 0.0f;
    vertices[29].v = 1.0f;

    // Right, A
    vertices[30].x = 1;
    vertices[30].y = 1;
    vertices[30].z = -1;
    vertices[30].u = 0.0f;
    vertices[30].v = 1.0f;

    // B
    vertices[31].x = 1;
    vertices[31].y = -1;
    vertices[31].z = -1;
    vertices[31].u = 0.0f;
    vertices[31].v = 0.0f;

    // C
    vertices[32].x = 1;
    vertices[32].y = -1;
    vertices[32].z = 1;
    vertices[32].u = 1.0f;
    vertices[32].v = 0.0f;

    vertices[33].x=1;
    vertices[33].y=1;
    vertices[33].z=-1;
    vertices[33].u=1;
    vertices[33].v=0;

    vertices[34].x=1;
    vertices[34].y=1;
    vertices[34].z=1;
    vertices[34].u=1;
    vertices[34].v=1;

    vertices[35].x = 1;
    vertices[35].y = -1;
    vertices[35].z = 1;
    vertices[35].u = 0.0f;
    vertices[35].v = 1.0f;

    for (unsigned i = 0; i < BLOCK_N_VERTICES; i++)
    {
        vertices[i].x = vertices[i].x/2.0f + x;
        vertices[i].y = vertices[i].y/2.0f + y;
        vertices[i].z = vertices[i].z/2.0f + z;
        vertices[i].u = (vertices[i].u < 0.1f) ? u_min : u_max;
        vertices[i].v = (vertices[i].v < 0.1f) ? v_min : v_max;
    }

    return block;
}
