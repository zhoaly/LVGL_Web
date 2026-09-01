/**
 * @file App_UiSliderRow.h
 * @brief Reusable labelled slider row component.
 */

#ifndef APP_UI_SLIDER_ROW_H
#define APP_UI_SLIDER_ROW_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    uint32_t id;
    const char *title;
    const char *subtitle;
    int32_t min;
    int32_t max;
    int32_t step;
    int32_t value;
    const char *unit;
    bool enabled;
} app_ui_slider_row_state_t;

typedef void (*app_ui_slider_row_value_changed_cb_t)(
    uint32_t id,
    int32_t value,
    void *user_data);

typedef struct {
    app_ui_slider_row_value_changed_cb_t on_value_changed;
    void *user_data;
} app_ui_slider_row_callbacks_t;

/**
 * @brief Caller-owned Slider Row instance context.
 *
 * The composition owner keeps this context alive for at least as long as the
 * component root. Child objects are private implementation details and must
 * not be modified by the owner.
 */
typedef struct {
    lv_obj_t *root;
    lv_obj_t *header;
    lv_obj_t *title_label;
    lv_obj_t *subtitle_label;
    lv_obj_t *value_box;
    lv_obj_t *value_label;
    lv_obj_t *unit_label;
    lv_obj_t *slider;
    uint32_t id;
    int32_t min;
    int32_t max;
    int32_t step;
    int32_t value;
    bool enabled;
    app_ui_slider_row_callbacks_t callbacks;
    bool updating;
} app_ui_slider_row_t;

/**
 * @brief Create a slider row and copy its initial state.
 *
 * Title is required. Subtitle and unit are optional. All text is copied into
 * LVGL labels, so the strings only need to remain valid for this call.
 *
 * The numeric range must satisfy max > min, step > 0, and both the range and
 * value offsets must be exact multiples of step. The slider is the row's only
 * focusable object and its events bubble to ancestor containers.
 *
 * @return Row root object, or NULL when arguments or state are invalid.
 */
lv_obj_t *App_UiSliderRow_Create(
    lv_obj_t *parent,
    app_ui_slider_row_t *row,
    const app_ui_slider_row_state_t *state,
    const app_ui_slider_row_callbacks_t *callbacks);

/**
 * @brief Update a slider row without invoking its user callback.
 *
 * Invalid state is rejected before any object or retained state is changed.
 * The callback configuration supplied at creation is preserved. Input strings
 * only need to remain valid for this call.
 *
 * @return true when the state was applied; false for invalid arguments/state.
 */
bool App_UiSliderRow_Update(
    app_ui_slider_row_t *row,
    const app_ui_slider_row_state_t *state);

#endif /* APP_UI_SLIDER_ROW_H */
