/**
 * @file App_UiScrollStack.c
 * @brief Reusable transparent vertical scroll container implementation.
 */

#include "App_UiScrollStack.h"

#include <limits.h>
#include <string.h>

#include "../../../assets/App_UiTheme.h"
#include "../../motion/App_UiMotion.h"

enum {
    SCROLL_STACK_PADDING = 4,
    SCROLL_STACK_ITEM_GAP = 8,
    SCROLL_STACK_SCROLLBAR_WIDTH = 3,
};

static lv_obj_t *find_top_level_child(
    const app_ui_scroll_stack_t *stack,
    lv_obj_t *object)
{
    lv_obj_t *parent;

    if(stack == NULL || stack->root == NULL || object == NULL ||
       object == stack->root) {
        return NULL;
    }

    parent = lv_obj_get_parent(object);
    while(parent != NULL && parent != stack->root) {
        object = parent;
        parent = lv_obj_get_parent(object);
    }

    return parent == stack->root ? object : NULL;
}

static lv_anim_enable_t get_scroll_animation(void)
{
    return App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL) == 0u
               ? LV_ANIM_OFF
               : LV_ANIM_ON;
}

static void child_created_cb(lv_event_t *event)
{
    app_ui_scroll_stack_t *stack = lv_event_get_user_data(event);
    /* LVGL sends CHILD_CREATED to the parent and passes the child as param. */
    lv_obj_t *child = lv_event_get_param(event);

    if(stack != NULL && stack->root != NULL && child != NULL) {
        lv_obj_add_flag(child, LV_OBJ_FLAG_EVENT_BUBBLE);
    }
}

static void child_focused_cb(lv_event_t *event)
{
    app_ui_scroll_stack_t *stack = lv_event_get_user_data(event);
    lv_obj_t *target = lv_event_get_target(event);
    lv_obj_t *row = find_top_level_child(stack, target);
    lv_area_t stack_area;
    lv_area_t row_area;
    lv_anim_enable_t animation;
    int32_t scroll_y;
    int32_t destination_y;
    int32_t safe_top;
    int32_t safe_bottom;
    int64_t proposed_safe_bottom;

    if(row == NULL) {
        return;
    }

    animation = get_scroll_animation();
    lv_obj_update_layout(stack->root);
    lv_obj_get_coords(stack->root, &stack_area);
    lv_obj_get_coords(row, &row_area);

    safe_top = stack_area.y1 + SCROLL_STACK_PADDING;
    proposed_safe_bottom =
        (int64_t)stack_area.y2 - SCROLL_STACK_PADDING -
        stack->bottom_space;
    safe_bottom = proposed_safe_bottom < safe_top
                      ? safe_top
                      : (int32_t)proposed_safe_bottom;

    scroll_y = lv_obj_get_scroll_y(stack->root);
    destination_y = scroll_y;
    if(row_area.y1 < safe_top) {
        destination_y -= safe_top - row_area.y1;
    }
    else if(row_area.y2 > safe_bottom) {
        destination_y += row_area.y2 - safe_bottom;
    }

    if(destination_y != scroll_y) {
        lv_obj_scroll_to_y(stack->root, destination_y, animation);
    }
}

static void stack_deleted_cb(lv_event_t *event)
{
    app_ui_scroll_stack_t *stack = lv_event_get_user_data(event);

    if(stack != NULL) {
        App_UiMotion_StopObject(stack->root);
        memset(stack, 0, sizeof(*stack));
    }
}

lv_obj_t *App_UiScrollStack_Create(
    lv_obj_t *parent,
    app_ui_scroll_stack_t *stack)
{
    if(parent == NULL || stack == NULL) {
        return NULL;
    }

    memset(stack, 0, sizeof(*stack));
    stack->root = lv_obj_create(parent);
    if(stack->root == NULL) {
        return NULL;
    }

    lv_obj_remove_style_all(stack->root);
    lv_obj_set_width(stack->root, LV_PCT(100));
    lv_obj_set_flex_grow(stack->root, 1);
    lv_obj_set_flex_flow(stack->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(stack->root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(stack->root, SCROLL_STACK_PADDING, 0);
    lv_obj_set_style_pad_row(stack->root, SCROLL_STACK_ITEM_GAP, 0);
    lv_obj_add_flag(stack->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(stack->root, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(stack->root, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_width(
        stack->root, SCROLL_STACK_SCROLLBAR_WIDTH, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(
        stack->root, SCROLL_STACK_SCROLLBAR_WIDTH, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(
        stack->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
        LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(
        stack->root, LV_OPA_40, LV_PART_SCROLLBAR);

    lv_obj_add_event_cb(
        stack->root, child_created_cb, LV_EVENT_CHILD_CREATED, stack);
    lv_obj_add_event_cb(
        stack->root, child_focused_cb, LV_EVENT_FOCUSED, stack);
    lv_obj_add_event_cb(
        stack->root, stack_deleted_cb, LV_EVENT_DELETE, stack);

    return stack->root;
}

bool App_UiScrollStack_SetBottomSpace(
    app_ui_scroll_stack_t *stack,
    int32_t height)
{
    if(stack == NULL || stack->root == NULL || height < 0 ||
       height > INT32_MAX - SCROLL_STACK_PADDING) {
        return false;
    }

    stack->bottom_space = height;
    lv_obj_set_style_pad_bottom(
        stack->root, SCROLL_STACK_PADDING + height, 0);
    return true;
}
