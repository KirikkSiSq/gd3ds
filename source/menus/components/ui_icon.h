#pragma once
#include "ui_element.h"


#define BUTTON_HOVER_SCALE 1.25f
#define BUTTON_HOVER_ANIM_TIME 0.4f

void ui_icon_set_gamemode_index(UIElement *e, int gamemode, int index);
UIElement ui_create_icon(
    int x, int y, float scale, int index, int gamemode, 
    UIActionFn action,
    char (*tag)[TAG_LENGTH]
);