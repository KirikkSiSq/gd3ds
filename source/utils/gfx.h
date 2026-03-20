#pragma once
#include <3ds.h>
#include <citro2d.h>

#include <math.h>

#define FADE_DURATION 0.25f
#define FADE_SPEED (255 / FADE_DURATION)

enum FadeStatus {
    FADE_STATUS_NONE,
    FADE_STATUS_OUT,
    FADE_STATUS_IN
};

// Fading
bool handle_fading();
void draw_fade();
void set_fade_status(int status);
C3D_RenderTarget* C2D_CreateScreenTargetExt(gfxScreen_t screen, gfx3dSide_t side, bool aa);

void set_wide(bool wide);
void reinitialize_screens();

float get_mirror_x(float x, float factor);

void draw_9_slice(const C2D_Image atlas, const float x, const float y, const int width, const int height, const float border, const u32 color);
Tex3DS_SubTexture select_box(const C2D_Image *image, int x, int y, int endX, int endY);
void set_scissor(GPU_SCISSORMODE mode, int x, int y, int width, int height);
bool C2D_DrawTriangleUV(float x0, float y0, float u0, float v0, u32 clr0, float x1, float y1, float u1, float v1, u32 clr1, float x2, float y2, float u2, float v2, u32 clr2, float depth, C2D_Image img);
