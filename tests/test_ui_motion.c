#include <assert.h>
#include <stdint.h>

#include "lvgl/lvgl.h"

#include "lvgl_app/components/motion/App_UiMotion.h"

struct _lv_obj_t {
    int32_t x;
    int32_t image_opa;
};

static unsigned int s_anim_start_count;
static unsigned int s_transition_init_count;
static uint32_t s_duration;
static uint32_t s_reverse_duration;
static uint32_t s_repeat_count;
static int32_t s_end_value;
static lv_anim_exec_xcb_t s_exec_cb;
static lv_obj_t *s_variable;

static void reset_animation_probe(void)
{
    s_anim_start_count = 0u;
    s_duration = 0u;
    s_reverse_duration = 0u;
    s_repeat_count = 0u;
    s_end_value = 0;
    s_exec_cb = NULL;
    s_variable = NULL;
}

void lv_obj_set_x(lv_obj_t *object, int32_t x)
{
    object->x = x;
}

void lv_obj_set_style_transform_width(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)object;
    (void)value;
    (void)selector;
}

void lv_obj_set_style_transform_height(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)object;
    (void)value;
    (void)selector;
}

void lv_obj_set_style_translate_y(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)object;
    (void)value;
    (void)selector;
}

void lv_obj_set_style_text_opa(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)object;
    (void)value;
    (void)selector;
}

void lv_obj_set_style_image_opa(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)selector;
    object->image_opa = value;
}

void lv_obj_set_style_bg_opa(
    lv_obj_t *object, int32_t value, int32_t selector)
{
    (void)object;
    (void)value;
    (void)selector;
}

void lv_obj_set_style_transition(
    lv_obj_t *object,
    const lv_style_transition_dsc_t *transition,
    int32_t selector)
{
    (void)object;
    (void)transition;
    (void)selector;
}

void lv_style_transition_dsc_init(
    lv_style_transition_dsc_t *transition,
    const lv_style_prop_t properties[],
    lv_anim_path_cb_t path,
    uint32_t duration,
    uint32_t delay,
    void *user_data)
{
    (void)transition;
    (void)properties;
    (void)path;
    (void)duration;
    (void)delay;
    (void)user_data;
    s_transition_init_count++;
}

void lv_anim_init(lv_anim_t *animation)
{
    (void)animation;
}

void lv_anim_set_var(lv_anim_t *animation, void *variable)
{
    (void)animation;
    s_variable = variable;
}

void lv_anim_set_exec_cb(
    lv_anim_t *animation, lv_anim_exec_xcb_t callback)
{
    (void)animation;
    s_exec_cb = callback;
}

void lv_anim_set_values(
    lv_anim_t *animation, int32_t start, int32_t end)
{
    (void)animation;
    (void)start;
    s_end_value = end;
}

void lv_anim_set_duration(lv_anim_t *animation, uint32_t duration)
{
    (void)animation;
    s_duration = duration;
}

void lv_anim_set_delay(lv_anim_t *animation, uint32_t delay)
{
    (void)animation;
    (void)delay;
}

void lv_anim_set_reverse_duration(
    lv_anim_t *animation, uint32_t duration)
{
    (void)animation;
    s_reverse_duration = duration;
}

void lv_anim_set_repeat_count(lv_anim_t *animation, uint32_t count)
{
    (void)animation;
    s_repeat_count = count;
}

void lv_anim_set_path_cb(
    lv_anim_t *animation, lv_anim_path_cb_t callback)
{
    (void)animation;
    (void)callback;
}

void lv_anim_set_user_data(lv_anim_t *animation, void *user_data)
{
    (void)animation;
    (void)user_data;
}

void lv_anim_set_completed_cb(
    lv_anim_t *animation, lv_anim_completed_cb_t callback)
{
    (void)animation;
    (void)callback;
}

lv_anim_t *lv_anim_start(lv_anim_t *animation)
{
    s_anim_start_count++;
    return animation;
}

uint32_t lv_anim_delete(void *variable, lv_anim_exec_xcb_t callback)
{
    (void)variable;
    (void)callback;
    return 0u;
}

int32_t lv_anim_path_ease_out(const lv_anim_t *animation)
{
    (void)animation;
    return 0;
}

int main(void)
{
    lv_obj_t object = {0};

    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_NORMAL);
    assert(App_UiMotion_GetLevel() == APP_UI_MOTION_LEVEL_NORMAL);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PRESS) == 70u);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_RELEASE) == 110u);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL) == 140u);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PAGE) == 180u);
    assert(s_transition_init_count >= 3u);

    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_REDUCED);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PAGE) == 90u);
    assert(App_UiMotion_GetDuration(
               APP_UI_MOTION_DURATION_DRAWER_OPEN) == 90u);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_PULSE) == 80u);

    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_OFF);
    reset_animation_probe();
    object.x = 4;
    assert(!App_UiMotion_AnimateX(
        &object,
        4,
        12,
        APP_UI_MOTION_DURATION_PAGE,
        NULL,
        NULL));
    assert(s_anim_start_count == 0u);
    assert(object.x == 12);
    assert(App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL) == 0u);

    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_NORMAL);
    reset_animation_probe();
    assert(App_UiMotion_AnimateX(
        &object,
        12,
        0,
        APP_UI_MOTION_DURATION_PAGE,
        NULL,
        NULL));
    assert(s_anim_start_count == 1u);
    assert(s_duration == 180u);
    assert(s_end_value == 0);
    assert(s_variable == &object);
    assert(s_exec_cb != NULL);

    reset_animation_probe();
    App_UiMotion_StartImagePulse(&object);
    assert(s_anim_start_count == 1u);
    assert(s_duration == 600u);
    assert(s_reverse_duration == 600u);
    assert(s_repeat_count == LV_ANIM_REPEAT_INFINITE);

    reset_animation_probe();
    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_OFF);
    assert(s_anim_start_count == 0u);
    assert(object.image_opa == LV_OPA_COVER);

    App_UiMotion_SetLevel(APP_UI_MOTION_LEVEL_REDUCED);
    reset_animation_probe();
    App_UiMotion_StartImagePulse(&object);
    assert(s_anim_start_count == 1u);
    assert(s_duration == 80u);
    assert(s_reverse_duration == 0u);
    assert(s_repeat_count == 0u);

    App_UiMotion_SetLevel((app_ui_motion_level_t)99);
    assert(App_UiMotion_GetLevel() == APP_UI_MOTION_LEVEL_NORMAL);
    return 0;
}
