// Adapted from OpenGD

#ifndef MOTION_TRAIL_H
#define MOTION_TRAIL_H

#include <citro2d.h>
#include "objects.h"
#include "color_channels.h"
#include "main.h"

#define MAX_TRAIL_POINTS 128
#define TRAIL_CLEAR_DISTANCE 30.f

typedef struct {
    float u, v;
} Tex2F;

typedef struct {
    bool appendNewPoints;
    bool startingPositionInitialized;
    bool waveTrail;

    int nuPoints;
    int previousNuPoints;
    int actualNuPoints;
    int maxPoints;
    int offscreenCount;

    float opacity;
    float fadeDelta;
    float minSeg;
    float stroke;

    Vec2D positionR;
    Color color;

    C2D_Image image;

    float pointState[MAX_TRAIL_POINTS];
    Vec2D pointVertexes[MAX_TRAIL_POINTS];
    Vec2D vertices[MAX_TRAIL_POINTS * 2];
    u8 opacities[MAX_TRAIL_POINTS * 2];
    Tex2F texCoords[MAX_TRAIL_POINTS * 2];
    
    Vec2D centerVertices[MAX_TRAIL_POINTS * 2];

    Vec2D lastStopPosition;
    bool wasStopped;
    bool blending;
} MotionTrail;

void MotionTrail_Init(MotionTrail* trail, float fade, float minSeg, float stroke, bool waveTrail, bool blending, Color color, C2D_Image tex);
void MotionTrail_UpdateWaveTrail(MotionTrail *trail, float delta);
void MotionTrail_Update(MotionTrail* trail, float delta);
void MotionTrail_ResumeStroke(MotionTrail* trail);
void MotionTrail_Clear(MotionTrail *trail);
void MotionTrail_StopStroke(MotionTrail* trail);
void MotionTrail_Draw(MotionTrail* trail);
void MotionTrail_DrawWaveTrail(MotionTrail *trail);

void MotionTrail_CopyTrail(MotionTrail *dst, MotionTrail *src);

void MotionTrail_AddWavePoint(MotionTrail* trail);

#endif