/**
 * @file App_UiActionRow.h
 * @brief Reusable settings-style row that reports a stable action ID.
 */

#ifndef APP_UI_ACTION_ROW_H
#define APP_UI_ACTION_ROW_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    uint32_t id;
    const char *title;
    const char *subtitle;
    const char *value;
    bool enabled;
} app_ui_action_row_state_t;

typedef void (*app_ui_action_row_activate_cb_t)(
    uint32_t row_id,
    void *user_data);

typedef struct {
    app_ui_action_row_activate_cb_t on_activated;
    void *user_data;
} app_ui_action_row_callbacks_t;

/**
 * @brief Caller-owned Action Row instance context.
 *
 * The composition owner keeps this context alive for at least as long as the
 * component root. Child objects are private implementation details and must
 * not be modified by the owner.
 */
typedef struct {
    lv_obj_t *root;
    lv_obj_t *text_column;
    lv_obj_t *title_label;
    lv_obj_t *subtitle_label;
    lv_obj_t *value_label;
    uint32_t id;
    bool enabled;
    app_ui_action_row_callbacks_t callbacks;
} app_ui_action_row_t;

/**
 * @brief Create an Action Row and copy the initial display state.
 *
 * subtitle and value are optional. A NULL or empty optional string hides its
 * label. title must be non-NULL and non-empty.
 *
 * @return Row root object, or NULL when arguments are invalid or creation
 * fails.
 */
lv_obj_t *App_UiActionRow_Create(
    lv_obj_t *parent,
    app_ui_action_row_t *row,
    const app_ui_action_row_state_t *state,
    const app_ui_action_row_callbacks_t *callbacks);

/**
 * @brief Update an Action Row in place without invoking its callback.
 *
 * All input is validated before the existing row is changed.
 *
 * @return true when the state was applied; false for invalid arguments.
 */
bool App_UiActionRow_Update(
    app_ui_action_row_t *row,
    const app_ui_action_row_state_t *state);

#endif /* APP_UI_ACTION_ROW_H */
