#include "VoxelRenderer.h"

#include <iostream>
#include <new>

#include "../Mesh.h"
#include "../Voxels/Chunk.h"
#include "../Voxels/Voxel.h"
#include "../defines.cpp"

#define VERTEX_SIZE    \
    (3 + 3 + 2 + 1 + 3 \
    ) /* Position, Normal, Texture coords, Light factor, Tangent */

#define CDIV(X, A) (((X) < 0) ? ((X) / (A) - 1) : ((X) / (A)))
#define LOCAL_NEG(X, SIZE) (((X) < 0) ? ((SIZE) + (X)) : (X))
#define LOCAL(X, SIZE) ((X) >= (SIZE) ? ((X) - (SIZE)) : LOCAL_NEG(X, SIZE))
#define IS_CHUNK(X, Y, Z) (GET_CHUNK(X, Y, Z) != nullptr)
#define GET_CHUNK(X, Y, Z)                                          \
    (chunks                                                         \
         [((CDIV(Y, CHUNK_H) + 1) * 3 + CDIV(Z, CHUNK_D) + 1) * 3 + \
          CDIV(X, CHUNK_W) + 1])

#define VOXEL(X, Y, Z)                                                  \
    (GET_CHUNK(X, Y, Z)->voxels                                         \
         [(LOCAL(Y, CHUNK_H) * CHUNK_D + LOCAL(Z, CHUNK_D)) * CHUNK_W + \
          LOCAL(X, CHUNK_W)])
#define IS_BLOCKED(X, Y, Z) \
    ((!IS_CHUNK(X, Y, Z)) || VOXEL(X, Y, Z).id && !VOXEL(X, Y, Z).isTransparent)

#define VERTEX(INDEX, X, Y, Z, NX, NY, NZ, U, V, L, TX, TY, TZ) \
    buffer[INDEX + 0] = (X);                                    \
    buffer[INDEX + 1] = (Y);                                    \
    buffer[INDEX + 2] = (Z);                                    \
    buffer[INDEX + 3] = (NX);                                   \
    buffer[INDEX + 4] = (NY);                                   \
    buffer[INDEX + 5] = (NZ);                                   \
    buffer[INDEX + 6] = (U);                                    \
    buffer[INDEX + 7] = (V);                                    \
    buffer[INDEX + 8] = (L);                                    \
    buffer[INDEX + 9] = (TX);                                   \
    buffer[INDEX + 10] = (TY);                                  \
    buffer[INDEX + 11] = (TZ);                                  \
    INDEX += VERTEX_SIZE;

int chunk_attrs[] = {3, 3, 2, 1, 3, 0};

VoxelRenderer::VoxelRenderer(uint64_t capacity)
    : capacity(capacity) {
    try {
        buffer = new /*(std::nothrow)*/ float[capacity * VERTEX_SIZE * 6];
    } catch (std::bad_alloc& ba) {
        CONSOLE_LOG("bad_alloc caught: ");
        CONSOLE_LOG(ba.what());
        CONSOLE_LOG('\n');
    }
}

VoxelRenderer::~VoxelRenderer() { delete[] buffer; }

Mesh* VoxelRenderer::render(
    Chunk* chunk, Chunk const** chunks, bool ambientOcclusion
) {
    float aoFactor = 0.15f;
    uint64_t index = 0;
    for (int y = 0; y < CHUNK_H; y++) {
        for (int z = 0; z < CHUNK_D; z++) {
            for (int x = 0; x < CHUNK_W; x++) {
                voxel vox = chunk->voxels[(y * CHUNK_D + z) * CHUNK_W + x];
                bool tr = vox.isTransparent;
                unsigned int id = vox.id;
                unsigned int tid = vox.TopTexId;
                unsigned int boid = vox.BottomTexId;
                unsigned int lid = vox.LeftTexId;
                unsigned int rid = vox.RightTexId;
                unsigned int fid = vox.FrontTexId;
                unsigned int baid = vox.BackTexId;

                if (!id) {
                    continue;
                }

                float l;
                float uvsize = 1.0f / 16.0f;

                float tu1 = (tid % 16) * uvsize;
                float tv1 = 1 - ((1 + tid / 16) * uvsize);
                float tu2 = tu1 + uvsize;
                float tv2 = tv1 + uvsize;

                float bou1 = (boid % 16) * uvsize;
                float bov1 = 1 - ((1 + boid / 16) * uvsize);
                float bou2 = bou1 + uvsize;
                float bov2 = bov1 + uvsize;

                float lu1 = (lid % 16) * uvsize;
                float lv1 = 1 - ((1 + lid / 16) * uvsize);
                float lu2 = lu1 + uvsize;
                float lv2 = lv1 + uvsize;

                float ru1 = (rid % 16) * uvsize;
                float rv1 = 1 - ((1 + rid / 16) * uvsize);
                float ru2 = ru1 + uvsize;
                float rv2 = rv1 + uvsize;

                float fu1 = (fid % 16) * uvsize;
                float fv1 = 1 - ((1 + fid / 16) * uvsize);
                float fu2 = fu1 + uvsize;
                float fv2 = fv1 + uvsize;

                float bau1 = (baid % 16) * uvsize;
                float bav1 = 1 - ((1 + baid / 16) * uvsize);
                float bau2 = bau1 + uvsize;
                float bav2 = bav1 + uvsize;

                /* Ambient Occlusion values */
                float a, b, c, d, e, f, g, h;
                a = b = c = d = e = f = g = h = 0.0f;

                if (!IS_BLOCKED(x, y + 1, z)) {
                    l = 1.0f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x + 1, y + 1, z) * aoFactor;
                        b = IS_BLOCKED(x, y + 1, z + 1) * aoFactor;
                        c = IS_BLOCKED(x - 1, y + 1, z) * aoFactor;
                        d = IS_BLOCKED(x, y + 1, z - 1) * aoFactor;

                        e = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
                        f = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
                        g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
                        h = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
                    }

                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f,
                        tu2, tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f,
                        tu2, tv2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f,
                        tu1, tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );

                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f,
                        tu2, tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f,
                        tu1, tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f,
                        tu1, tv1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f
                    );
                }
                if (!IS_BLOCKED(x, y - 1, z)) {
                    l = 0.75f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x + 1, y - 1, z) * aoFactor;
                        b = IS_BLOCKED(x, y - 1, z + 1) * aoFactor;
                        c = IS_BLOCKED(x - 1, y - 1, z) * aoFactor;
                        d = IS_BLOCKED(x, y - 1, z - 1) * aoFactor;

                        e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
                        f = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
                        g = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
                        h = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
                    }

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f,
                        bou1, bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f,
                        bou2, bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f,
                        bou1, bov2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f
                    );

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f,
                        bou1, bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f,
                        bou2, bov1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f,
                        bou2, bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );
                }

                if (!IS_BLOCKED(x + 1, y, z)) {
                    l = 0.95f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x + 1, y + 1, z) * aoFactor;
                        b = IS_BLOCKED(x + 1, y, z + 1) * aoFactor;
                        c = IS_BLOCKED(x + 1, y - 1, z) * aoFactor;
                        d = IS_BLOCKED(x + 1, y, z - 1) * aoFactor;

                        e = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
                        f = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
                        g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
                        h = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
                    }

                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f,
                        ru2, rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f,
                        ru2, rv2, l * (1.0f - d - a - h), 0.0f, 0.0f, -1.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f,
                        ru1, rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f
                    );

                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f,
                        ru2, rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f,
                        ru1, rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f,
                        ru1, rv1, l * (1.0f - b - c - f), 0.0f, 0.0f, -1.0f
                    );
                }
                if (!IS_BLOCKED(x - 1, y, z)) {
                    l = 0.85f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x - 1, y + 1, z) * aoFactor;
                        b = IS_BLOCKED(x - 1, y, z + 1) * aoFactor;
                        c = IS_BLOCKED(x - 1, y - 1, z) * aoFactor;
                        d = IS_BLOCKED(x - 1, y, z - 1) * aoFactor;

                        e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
                        f = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
                        g = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
                        h = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
                    }

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f,
                        lu1, lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f,
                        lu2, lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f,
                        lu1, lv2, l * (1.0f - d - a - h), 0.0f, 0.0f, 1.0f
                    );

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f,
                        lu1, lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f,
                        lu2, lv1, l * (1.0f - b - c - f), 0.0f, 0.0f, 1.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f,
                        lu2, lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f
                    );
                }

                if (!IS_BLOCKED(x, y, z + 1)) {
                    l = 0.9f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x, y + 1, z + 1) * aoFactor;
                        b = IS_BLOCKED(x + 1, y, z + 1) * aoFactor;
                        c = IS_BLOCKED(x, y - 1, z + 1) * aoFactor;
                        d = IS_BLOCKED(x - 1, y, z + 1) * aoFactor;

                        e = IS_BLOCKED(x - 1, y - 1, z + 1) * aoFactor;
                        f = IS_BLOCKED(x + 1, y - 1, z + 1) * aoFactor;
                        g = IS_BLOCKED(x + 1, y + 1, z + 1) * aoFactor;
                        h = IS_BLOCKED(x - 1, y + 1, z + 1) * aoFactor;
                    }

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau1, bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau2, bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau1, bav2, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f
                    );

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau1, bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau2, bav1, l * (1.0f - b - c - f), 1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f,
                        bau2, bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f
                    );
                }
                if (!IS_BLOCKED(x, y, z - 1)) {
                    l = 0.8f;

                    if (ambientOcclusion) {
                        a = IS_BLOCKED(x, y + 1, z - 1) * aoFactor;
                        b = IS_BLOCKED(x + 1, y, z - 1) * aoFactor;
                        c = IS_BLOCKED(x, y - 1, z - 1) * aoFactor;
                        d = IS_BLOCKED(x - 1, y, z - 1) * aoFactor;

                        e = IS_BLOCKED(x - 1, y - 1, z - 1) * aoFactor;
                        f = IS_BLOCKED(x + 1, y - 1, z - 1) * aoFactor;
                        g = IS_BLOCKED(x + 1, y + 1, z - 1) * aoFactor;
                        h = IS_BLOCKED(x - 1, y + 1, z - 1) * aoFactor;
                    }

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu2, fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu2, fv2, l * (1.0f - a - d - h), -1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu1, fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f
                    );

                    VERTEX(
                        index, x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu2, fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu1, fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f
                    );
                    VERTEX(
                        index, x + 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f,
                        fu1, fv1, l * (1.0f - b - c - f), -1.0f, 0.0f, 0.0f
                    );
                }
            }
        }
    }
    return new Mesh(buffer, index / VERTEX_SIZE, chunk_attrs);
}
