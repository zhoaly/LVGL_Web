/**
 * @file App_UiMotion.h
 * @brief UI motion tokens and reusable LVGL animation helpers.
 */

#ifndef APP_UI_MOTION_H
#define APP_UI_MOTION_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef enum {
    APP_UI_MOTION_LEVEL_NORMAL = 0,
    APP_UI_MOTION_LEVEL_REDUCED,
    APP_UI_MOTION_LEVEL_OFF,
    APP_UI_MOTION_LEVEL_COUNT,
} app_ui_motion_level_t;

typedef enum {
    APP_UI_MOTION_DURATION_PRESS = 0,
    APP_UI_MOTION_DURATION_RELEASE,
    APP_UI_MOTION_DURATION_CONTROL,
    APP_UI_MOTION_DURATION_PAGE,
    APP_UI_MOTION_DURATION_DRAWER_OPEN,
    APP_UI_MOTION_DURATION_DRAWER_CLOSE,
    APP_UI_MOTION_DURATION_PULSE,
    APP_UI_MOTION_DURATION_COUNT,
} app_ui_motion_duration_t;

typedef enum {
    APP_UI_MOTION_OPACITY_NONE = 0,
    APP_UI_MOTION_OPACITY_TEXT,
    APP_UI_MOTION_OPACITY_IMAGE,
    APP_UI_MOTION_OPACITY_BACKGROUND,
} app_ui_motion_opacity_t;

void App_UiMotion_Init(void);
void App_UiMotion_SetLevel(app_ui_motion_level_t level);
app_ui_motion_level_t App_UiMotion_GetLevel(void);
uint32_t App_UiMotion_GetDuration(app_ui_motion_duration_t duration);
lv_anim_path_cb_t App_UiMotion_GetPath(void);

void App_UiMotion_ApplyButton(lv_obj_t *object);
void App_UiMotion_ApplySwitch(lv_obj_t *object);
void App_UiMotion_ApplySlider(lv_obj_t *object);

bool App_UiMotion_AnimateX(
    lv_obj_t *object,
    int32_t start,
    int32_t end,
    app_ui_motion_duration_t duration,
    lv_anim_completed_cb_t completed_cb,
    void *user_data);

void App_UiMotion_AnimateEnter(
    lv_obj_t *object,
    app_ui_motion_opacity_t opacity,
    int32_t offset_y,
    uint32_t delay_ms);

void App_UiMotion_StartImagePulse(lv_obj_t *image);
void App_UiMotion_SettleImage(lv_obj_t *image);
void App_UiMotion_StopObject(lv_obj_t *object);

#endif /* APP_UI_MOTION_H */
