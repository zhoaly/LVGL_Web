/**
 * @file App_UiToggleRow.h
 * @brief Reusable settings-style row with a passive visual switch.
 */

#ifndef APP_UI_TOGGLE_ROW_H
#define APP_UI_TOGGLE_ROW_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    uint32_t id;
    const char *title;
    const char *subtitle;
    bool checked;
    bool enabled;
} app_ui_toggle_row_state_t;

typedef void (*app_ui_toggle_row_changed_cb_t)(
    uint32_t row_id,
    bool checked,
    void *user_data);

typedef struct {
    app_ui_toggle_row_changed_cb_t on_changed;
    void *user_data;
} app_ui_toggle_row_callbacks_t;

/**
 * @brief Caller-owned Toggle Row instance context.
 *
 * The row is the only focusable and clickable object. The internal switch is
 * presentation-only and must not be accessed by the composition owner.
 */
typedef struct {
    lv_obj_t *root;
    lv_obj_t *text_column;
    lv_obj_t *title_label;
    lv_obj_t *subtitle_label;
    lv_obj_t *switch_control;
    uint32_t id;
    bool checked;
    bool enabled;
    app_ui_toggle_row_callbacks_t callbacks;
} app_ui_toggle_row_t;

/**
 * @brief Create a Toggle Row and copy the initial display state.
 *
 * subtitle is optional. A NULL or empty subtitle hides its label. title must
 * be non-NULL and non-empty.
 *
 * @return Row root object, or NULL when arguments are invalid or creation
 * fails.
 */
lv_obj_t *App_UiToggleRow_Create(
    lv_obj_t *parent,
    app_ui_toggle_row_t *row,
    const app_ui_toggle_row_state_t *state,
    const app_ui_toggle_row_callbacks_t *callbacks);

/**
 * @brief Update a Toggle Row in place without invoking its callback.
 *
 * All input is validated before the existing row is changed.
 *
 * @return true when the state was applied; false for invalid arguments.
 */
bool App_UiToggleRow_Update(
    app_ui_toggle_row_t *row,
    const app_ui_toggle_row_state_t *state);

#endif /* APP_UI_TOGGLE_ROW_H */
