#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum {
   COLLISION_NONE = 0,
   COLLISION_BOX,
   COLLISION_CIRCLE,
   COLLISION_SLOPE
} CollisionShape;

typedef enum {
   HITBOX_SPECIAL = 0,
   HITBOX_SOLID,
   HITBOX_HAZARD
} HitboxType;

typedef struct {
    int texture;
    float x, y;
    float scale_x, scale_y;
    int flip_x, flip_y;
    int z;
    float rot;
    int color_type;
} ChildSprite;

typedef struct {
   int collision_type;
   float width, height;
   float x, y;
   int type;
} ObjectHitbox;

typedef struct {
    int texture;
    float x, y;
    int z_layer;
    int z_order;
    int base_color;
    int color_type;
    int child_count;
    const ChildSprite* children;
    const ObjectHitbox* hitbox;
} GameObject;

#define TEXTURE_COUNT 1635
#define GAME_OBJECT_COUNT 1912

extern const GameObject game_objects[GAME_OBJECT_COUNT];
