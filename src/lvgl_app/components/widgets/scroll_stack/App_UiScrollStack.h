/**
 * @file App_UiScrollStack.h
 * @brief Reusable transparent vertical scroll container.
 */

#ifndef APP_UI_SCROLL_STACK_H
#define APP_UI_SCROLL_STACK_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    lv_obj_t *root;
    int32_t bottom_space;
} app_ui_scroll_stack_t;

/**
 * @brief Create a transparent vertically scrolling composition container.
 *
 * Children can be created directly under the returned object. Focus events
 * from nested descendants are bubbled to the stack automatically so the
 * corresponding top-level child remains visible.
 *
 * @param parent Parent object that owns the stack.
 * @param stack Caller-owned instance context.
 * @return Stack root object, or NULL when the arguments are invalid or the
 *         object could not be created.
 */
lv_obj_t *App_UiScrollStack_Create(
    lv_obj_t *parent,
    app_ui_scroll_stack_t *stack);

/**
 * @brief Set transparent scrollable space below the final child.
 *
 * The same height is reserved as a focus-safe area above persistent overlays
 * such as the bottom navigation dock.
 *
 * @param stack Instance returned by App_UiScrollStack_Create().
 * @param height Space height in pixels; must be non-negative.
 * @return true when the space was applied; false for invalid arguments.
 */
bool App_UiScrollStack_SetBottomSpace(
    app_ui_scroll_stack_t *stack,
    int32_t height);

#endif /* APP_UI_SCROLL_STACK_H */
