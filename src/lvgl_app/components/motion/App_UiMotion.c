/**
 * @file App_UiMotion.c
 * @brief Firmware-conscious motion implementation for LVGL 9.3.
 */

#include "App_UiMotion.h"

enum {
    MOTION_BUTTON_TRANSFORM = -1,
    MOTION_BUTTON_TRANSLATE_Y = 1,
    MOTION_ENTER_OPA_NORMAL = 178,
    MOTION_ENTER_OPA_REDUCED = 218,
    MOTION_PULSE_MIN_OPA = 153,
    MOTION_MAX_PULSE_OBJECTS = 2,
};

static app_ui_motion_level_t s_level = APP_UI_MOTION_LEVEL_NORMAL;
static bool s_initialized;
static lv_style_transition_dsc_t s_press_transition;
static lv_style_transition_dsc_t s_release_transition;
static lv_style_transition_dsc_t s_control_transition;
static lv_obj_t *s_pulse_objects[MOTION_MAX_PULSE_OBJECTS];

static const lv_style_prop_t s_button_transition_props[] = {
    LV_STYLE_TRANSFORM_WIDTH,
    LV_STYLE_TRANSFORM_HEIGHT,
    LV_STYLE_TRANSLATE_Y,
    LV_STYLE_BG_COLOR,
    LV_STYLE_BG_OPA,
    LV_STYLE_BORDER_COLOR,
    LV_STYLE_BORDER_OPA,
    LV_STYLE_SHADOW_OPA,
    LV_STYLE_SHADOW_OFFSET_Y,
    0,
};

static const lv_style_prop_t s_control_transition_props[] = {
    LV_STYLE_BG_COLOR,
    LV_STYLE_BG_OPA,
    LV_STYLE_BORDER_COLOR,
    LV_STYLE_BORDER_OPA,
    LV_STYLE_TRANSFORM_WIDTH,
    LV_STYLE_TRANSFORM_HEIGHT,
    0,
};

static void translate_y_anim_cb(void *object, int32_t value)
{
    lv_obj_set_style_translate_y((lv_obj_t *)object, value, 0);
}

static void x_anim_cb(void *object, int32_t value)
{
    lv_obj_set_x((lv_obj_t *)object, value);
}

static void text_opa_anim_cb(void *object, int32_t value)
{
    lv_obj_set_style_text_opa((lv_obj_t *)object, value, 0);
}

static void image_opa_anim_cb(void *object, int32_t value)
{
    lv_obj_set_style_image_opa((lv_obj_t *)object, value, 0);
}

static void background_opa_anim_cb(void *object, int32_t value)
{
    lv_obj_set_style_bg_opa((lv_obj_t *)object, value, 0);
}

static lv_anim_exec_xcb_t opacity_anim_cb(app_ui_motion_opacity_t opacity)
{
    switch(opacity) {
    case APP_UI_MOTION_OPACITY_TEXT:
        return text_opa_anim_cb;
    case APP_UI_MOTION_OPACITY_IMAGE:
        return image_opa_anim_cb;
    case APP_UI_MOTION_OPACITY_BACKGROUND:
        return background_opa_anim_cb;
    case APP_UI_MOTION_OPACITY_NONE:
    default:
        return NULL;
    }
}

static void refresh_transition_descriptors(void)
{
    lv_style_transition_dsc_init(
        &s_press_transition,
        s_button_transition_props,
        lv_anim_path_ease_out,
        App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PRESS),
        0,
        NULL);
    lv_style_transition_dsc_init(
        &s_release_transition,
        s_button_transition_props,
        lv_anim_path_ease_out,
        App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_RELEASE),
        0,
        NULL);
    lv_style_transition_dsc_init(
        &s_control_transition,
        s_control_transition_props,
        lv_anim_path_ease_out,
        App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL),
        0,
        NULL);
}

static bool register_pulse_object(lv_obj_t *object)
{
    uint32_t index;

    for(index = 0u; index < MOTION_MAX_PULSE_OBJECTS; index++) {
        if(s_pulse_objects[index] == object) {
            return true;
        }
    }
    for(index = 0u; index < MOTION_MAX_PULSE_OBJECTS; index++) {
        if(s_pulse_objects[index] == NULL) {
            s_pulse_objects[index] = object;
            return true;
        }
    }
    return false;
}

static void unregister_pulse_object(lv_obj_t *object)
{
    uint32_t index;

    for(index = 0u; index < MOTION_MAX_PULSE_OBJECTS; index++) {
        if(s_pulse_objects[index] == object) {
            s_pulse_objects[index] = NULL;
        }
    }
}

static bool start_animation(
    lv_obj_t *object,
    lv_anim_exec_xcb_t exec_cb,
    int32_t start,
    int32_t end,
    uint32_t duration,
    uint32_t delay_ms,
    lv_anim_completed_cb_t completed_cb,
    void *user_data)
{
    lv_anim_t animation;

    if(object == NULL || exec_cb == NULL) {
        return false;
    }

    lv_anim_delete(object, exec_cb);
    if(duration == 0u) {
        exec_cb(object, end);
        return false;
    }

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, object);
    lv_anim_set_exec_cb(&animation, exec_cb);
    lv_anim_set_values(&animation, start, end);
    lv_anim_set_duration(&animation, duration);
    lv_anim_set_delay(&animation, delay_ms);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
    if(completed_cb != NULL) {
        lv_anim_set_completed_cb(&animation, completed_cb);
        lv_anim_set_user_data(&animation, user_data);
    }
    lv_anim_start(&animation);
    return true;
}

void App_UiMotion_Init(void)
{
    if(s_initialized) {
        return;
    }
    s_initialized = true;
    refresh_transition_descriptors();
}

void App_UiMotion_SetLevel(app_ui_motion_level_t level)
{
    uint32_t index;

    if(level < APP_UI_MOTION_LEVEL_NORMAL ||
       level >= APP_UI_MOTION_LEVEL_COUNT) {
        level = APP_UI_MOTION_LEVEL_NORMAL;
    }

    s_level = level;
    s_initialized = true;
    refresh_transition_descriptors();

    if(level != APP_UI_MOTION_LEVEL_NORMAL) {
        for(index = 0u; index < MOTION_MAX_PULSE_OBJECTS; index++) {
            lv_obj_t *object = s_pulse_objects[index];

            s_pulse_objects[index] = NULL;
            if(object != NULL) {
                App_UiMotion_SettleImage(object);
            }
        }
    }
}

app_ui_motion_level_t App_UiMotion_GetLevel(void)
{
    return s_level;
}

uint32_t App_UiMotion_GetDuration(app_ui_motion_duration_t duration)
{
    static const uint16_t normal[APP_UI_MOTION_DURATION_COUNT] = {
        [APP_UI_MOTION_DURATION_PRESS] = 70,
        [APP_UI_MOTION_DURATION_RELEASE] = 110,
        [APP_UI_MOTION_DURATION_CONTROL] = 140,
        [APP_UI_MOTION_DURATION_PAGE] = 180,
        [APP_UI_MOTION_DURATION_DRAWER_OPEN] = 160,
        [APP_UI_MOTION_DURATION_DRAWER_CLOSE] = 120,
        [APP_UI_MOTION_DURATION_PULSE] = 600,
    };
    static const uint16_t reduced[APP_UI_MOTION_DURATION_COUNT] = {
        [APP_UI_MOTION_DURATION_PRESS] = 50,
        [APP_UI_MOTION_DURATION_RELEASE] = 70,
        [APP_UI_MOTION_DURATION_CONTROL] = 80,
        [APP_UI_MOTION_DURATION_PAGE] = 90,
        [APP_UI_MOTION_DURATION_DRAWER_OPEN] = 90,
        [APP_UI_MOTION_DURATION_DRAWER_CLOSE] = 80,
        [APP_UI_MOTION_DURATION_PULSE] = 80,
    };

    if(duration < 0 || duration >= APP_UI_MOTION_DURATION_COUNT ||
       s_level == APP_UI_MOTION_LEVEL_OFF) {
        return 0u;
    }
    return s_level == APP_UI_MOTION_LEVEL_REDUCED
        ? reduced[duration]
        : normal[duration];
}

lv_anim_path_cb_t App_UiMotion_GetPath(void)
{
    return lv_anim_path_ease_out;
}

void App_UiMotion_ApplyButton(lv_obj_t *object)
{
    if(object == NULL) {
        return;
    }
    App_UiMotion_Init();

    lv_obj_set_style_transform_width(
        object, MOTION_BUTTON_TRANSFORM, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(
        object, MOTION_BUTTON_TRANSFORM, LV_STATE_PRESSED);
    lv_obj_set_style_translate_y(
        object, MOTION_BUTTON_TRANSLATE_Y, LV_STATE_PRESSED);
    lv_obj_set_style_transition(
        object, &s_press_transition, LV_STATE_PRESSED);
    lv_obj_set_style_transition(
        object, &s_release_transition, LV_STATE_DEFAULT);
    lv_obj_set_style_transition(
        object, &s_control_transition, LV_STATE_FOCUSED);
}

void App_UiMotion_ApplySwitch(lv_obj_t *object)
{
    if(object == NULL) {
        return;
    }
    App_UiMotion_Init();

    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_KNOB | LV_STATE_CHECKED);
}

void App_UiMotion_ApplySlider(lv_obj_t *object)
{
    if(object == NULL) {
        return;
    }
    App_UiMotion_Init();

    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_transition(
        object, &s_control_transition, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_transform_width(
        object, 1, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(
        object, 1, LV_PART_KNOB | LV_STATE_PRESSED);
}

bool App_UiMotion_AnimateX(
    lv_obj_t *object,
    int32_t start,
    int32_t end,
    app_ui_motion_duration_t duration,
    lv_anim_completed_cb_t completed_cb,
    void *user_data)
{
    return start_animation(
        object,
        x_anim_cb,
        start,
        end,
        App_UiMotion_GetDuration(duration),
        0u,
        completed_cb,
        user_data);
}

void App_UiMotion_AnimateEnter(
    lv_obj_t *object,
    app_ui_motion_opacity_t opacity,
    int32_t offset_y,
    uint32_t delay_ms)
{
    lv_anim_exec_xcb_t opa_cb;
    uint32_t duration;
    int32_t start_opa;

    if(object == NULL) {
        return;
    }
    App_UiMotion_Init();

    duration = App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL);
    if(s_level != APP_UI_MOTION_LEVEL_NORMAL) {
        offset_y = 0;
    }

    (void)start_animation(
        object,
        translate_y_anim_cb,
        offset_y,
        0,
        duration,
        delay_ms,
        NULL,
        NULL);

    opa_cb = opacity_anim_cb(opacity);
    start_opa = s_level == APP_UI_MOTION_LEVEL_REDUCED
        ? MOTION_ENTER_OPA_REDUCED
        : MOTION_ENTER_OPA_NORMAL;
    (void)start_animation(
        object,
        opa_cb,
        start_opa,
        LV_OPA_COVER,
        duration,
        delay_ms,
        NULL,
        NULL);
}

void App_UiMotion_StartImagePulse(lv_obj_t *image)
{
    lv_anim_t animation;
    uint32_t duration;

    if(image == NULL) {
        return;
    }
    App_UiMotion_Init();
    lv_anim_delete(image, image_opa_anim_cb);

    if(s_level == APP_UI_MOTION_LEVEL_REDUCED) {
        App_UiMotion_SettleImage(image);
        return;
    }

    duration = App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PULSE);
    if(duration == 0u) {
        lv_obj_set_style_image_opa(image, LV_OPA_COVER, 0);
        return;
    }
    if(!register_pulse_object(image)) {
        lv_obj_set_style_image_opa(image, LV_OPA_COVER, 0);
        return;
    }

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, image);
    lv_anim_set_exec_cb(&animation, image_opa_anim_cb);
    lv_anim_set_values(&animation, MOTION_PULSE_MIN_OPA, LV_OPA_COVER);
    lv_anim_set_duration(&animation, duration);
    lv_anim_set_reverse_duration(&animation, duration);
    lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
    lv_anim_start(&animation);
}

void App_UiMotion_SettleImage(lv_obj_t *image)
{
    uint32_t duration;

    if(image == NULL) {
        return;
    }
    App_UiMotion_Init();
    unregister_pulse_object(image);
    duration = App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL);
    (void)start_animation(
        image,
        image_opa_anim_cb,
        s_level == APP_UI_MOTION_LEVEL_REDUCED
            ? MOTION_ENTER_OPA_REDUCED
            : MOTION_PULSE_MIN_OPA,
        LV_OPA_COVER,
        duration,
        0u,
        NULL,
        NULL);
}

void App_UiMotion_StopObject(lv_obj_t *object)
{
    if(object != NULL) {
        unregister_pulse_object(object);
        lv_anim_delete(object, NULL);
    }
}
