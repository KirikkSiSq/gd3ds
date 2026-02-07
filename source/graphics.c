#include "graphics.h"
#include "objects.h"
#include "main.h"

int sprite_count = 0;

C2D_SpriteSheet spriteSheet;
C2D_SpriteSheet bgSheet;
C2D_Sprite bg;

static SortItem buf_a[MAX_SPRITES];
static SortItem buf_b[MAX_SPRITES];

static SpriteObject viewable_objects[MAX_SPRITES];
static SpriteObject *viewable_objects_ptr[MAX_SPRITES];

static inline float normalize_angle(float a)
{
    while (a < 0.0f)   a += 360.0f;
    while (a >= 360.0f) a -= 360.0f;
    return a;
}

float mirror_angle(float angle, bool hflip, bool vflip)
{
    if (hflip && vflip) {
        angle += 180.0f;
    } else if (hflip) {
        angle = 180.0f - angle;
    } else if (vflip) {
        angle = -angle;
    }

    return normalize_angle(angle);
}

static void spawn_sprite_c2d(
    C2D_Sprite* out,
    C2D_SpriteSheet sheet,
    int texture,
    float x, float y,
    float scale_x, float scale_y,
    float rotation
) {
    C2D_SpriteFromSheet(out, sheet, texture);
    C2D_SpriteSetCenter(out, 0.5f, 0.5f);
    C2D_SpriteSetPos(out, x, y);
    C2D_SpriteSetScale(out, scale_x, scale_y);
    C2D_SpriteSetRotation(out, C3D_AngleFromDegrees(rotation));
	
	C3D_TexSetFilter(out->image.tex, GPU_NEAREST, GPU_NEAREST);
}

void spawn_object_at(
	Object *obj_game,
    int id,
    C2D_SpriteSheet sheet,
    float x,
    float y,
	float deg,
	unsigned char flip_x,
	unsigned char flip_y,
	float scale
) {
	const GameObject* obj = &game_objects[id];
	if (!obj)
		return;

	float cos_r = cosf(C3D_AngleFromDegrees(deg));
	float sin_r = sinf(C3D_AngleFromDegrees(deg));

	int flip_x_mult = (flip_x ? -1 : 1);
	int flip_y_mult = (flip_y ? -1 : 1);

	// Skip if no texture
	if (obj->texture >= 0 && sprite_count < MAX_SPRITES - 1) {
		C2D_Sprite root = { 0 };

		float local_x = obj->x * flip_x_mult;
		float local_y = obj->y * flip_y_mult;

		float rot_x = local_x * cos_r + local_y * sin_r;
		float rot_y = local_x * sin_r - local_y * cos_r;

		float p_x = x + rot_x * scale;
		float p_y = y + rot_y * scale;

		spawn_sprite_c2d(
			&root,
			sheet,
			obj->texture,
			floorf(p_x),
			floorf(p_y),
			scale * flip_x_mult, 
			scale * flip_y_mult,
			deg
		);

		viewable_objects[sprite_count].spr = root;
		viewable_objects[sprite_count].obj = obj_game;
		viewable_objects[sprite_count].layer = 0;
		viewable_objects_ptr[sprite_count] = &viewable_objects[sprite_count];
		sprite_count++;
	}

    // Render children
    for (int i = 0; i < obj->child_count; i++) {
        const ChildSprite* c = &obj->children[i];
        C2D_Sprite rs = { 0 };
		
		// Skip if no texture
		if (obj->children->texture >= 0 && sprite_count < MAX_SPRITES - 1) {
			float c_local_x = c->x * flip_x_mult;
			float c_local_y = c->y * flip_y_mult;

			float c_rot_x = c_local_x * cos_r + c_local_y * sin_r;
			float c_rot_y = c_local_x * sin_r - c_local_y * cos_r;

			float c_x = x + c_rot_x * scale;
			float c_y = y + c_rot_y * scale;

            int c_flip_x_mult = (c->flip_x ? -1 : 1);
            int c_flip_y_mult = (c->flip_y ? -1 : 1);

			spawn_sprite_c2d(
				&rs,
				sheet,
				c->texture,
				floorf(c_x),
				floorf(c_y),
				c->scale_x * c_flip_x_mult * scale * flip_x_mult,
				c->scale_y * c_flip_y_mult * scale * flip_y_mult,
				c->rot + deg
			);

			viewable_objects[sprite_count].spr = rs;
			viewable_objects[sprite_count].obj = obj_game;
			viewable_objects[sprite_count].layer = i + 1;
			viewable_objects_ptr[sprite_count] = &viewable_objects[sprite_count];
			sprite_count++;
		}
    }
}

static inline uint32_t make_sort_key(const SpriteObject *s)
{
    const Object *obj = s->obj;
    const GameObject *game_obj = &game_objects[obj->id];

    int zlayer = obj->zlayer ? obj->zlayer : game_obj->z_layer;
    int zorder = obj->zorder ? obj->zorder : game_obj->z_order;

    int child_z = 0;
    if (s->layer > 0) {
        child_z = game_obj->children[s->layer - 1].z;
    }

    uint32_t zl = (uint32_t)(zlayer + 8);     // fits in 4 bits
    uint32_t zo = (uint32_t)(zorder + 128);   // fits in 8 bits
    uint32_t cz = (uint32_t)(child_z + 128);  // fits in 8 bits

    return (zl << 16) | (zo << 8) | cz;
}

#define VIEW_OBJECTS (12 * 6)
#define INSERTION_SORT_THRESHOLD 16

void sort_viewable_objects(SpriteObject **objects, int count)
{
    if (count <= 1) return;

    for (int i = 0; i < count; i++) {
        buf_a[i].obj = objects[i];
        buf_a[i].key = make_sort_key(objects[i]);
    }

    SortItem *src = buf_a;
    SortItem *dst = buf_b;

    for (int pass = 0; pass < 3; pass++) {
		uint16_t buckets[256] = {0};
		int shift = pass * 8;

		for (int i = 0; i < count; i++) {
			buckets[(src[i].key >> shift) & 0xFF]++;
		}

		uint16_t sum = 0;
		for (int i = 0; i < 256; i++) {
			uint16_t t = buckets[i];
			buckets[i] = sum;
			sum += t;
		}

		for (int i = 0; i < count; i++) {
			uint8_t b = (src[i].key >> shift) & 0xFF;
			dst[buckets[b]++] = src[i];
		}

		SortItem *tmp = src;
		src = dst;
		dst = tmp;
	}

    for (int i = 0; i < count; i++) {
        objects[i] = src[i].obj;
    }
}

int get_object_layers(int id) {
	int count = 0;
	if (id < 0 || id >= GAME_OBJECT_COUNT) return 0;

	const GameObject obj = game_objects[id];
	if (obj.texture >= 0) count++;
	
	for (size_t c = 0; c < obj.child_count; c++) {
		if (obj.children[c].texture >= 0) count++;
	}
	return count;
}

void draw_objects() {
	sprite_count = 0;

    int width = (SCREEN_WIDTH / 2) / SECTION_SIZE + 2;
	int cam_sx = (int)((cam_x + SCREEN_WIDTH / 2) / SECTION_SIZE);

    // Create sprites
	for (int x = -width; x <= width; x++) {
		int section = cam_sx + x;
		if (section < 0) continue;

		Section *sec = get_or_create_section(section);
		for (int i = 0; i < sec->object_count; i++) {

			Object *obj = sec->objects[i];
			
            float calc_x = ((obj->x - cam_x));
            float calc_y = SCREEN_HEIGHT - ((obj->y - cam_y));  
			if (calc_x < -60 || calc_x >= (SCREEN_WIDTH / SCALE) + 60) continue;
			if (calc_y < -60 || calc_y >= (SCREEN_HEIGHT / SCALE) + 60) continue;

			spawn_object_at(
				obj,
				obj->id,
				spriteSheet,
				calc_x,
				calc_y,
				obj->rotation,
				obj->flippedH,
				obj->flippedV,
				1.0f
			);
		}
	}

    // Sort
    sort_viewable_objects(viewable_objects_ptr, sprite_count);

	// Draw
	for (size_t s = 0; s < sprite_count; s++) {
		C2D_DrawSprite(&viewable_objects_ptr[s]->spr);
	}
}