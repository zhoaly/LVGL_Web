/**
 * @file App_UiSliderRow.c
 * @brief Reusable labelled slider row component implementation.
 */

#include "App_UiSliderRow.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "../../../assets/App_UiTheme.h"
#include "../../App_UiComponents.h"
#include "../../motion/App_UiMotion.h"

enum {
    SLIDER_ROW_HEIGHT = 76,
    SLIDER_ROW_SUBTITLE_HEIGHT = 92,
    SLIDER_ROW_RADIUS = 10,
    SLIDER_ROW_HORIZONTAL_PADDING = 12,
    SLIDER_ROW_VERTICAL_PADDING = 8,
    SLIDER_ROW_GAP = 5,
    SLIDER_ROW_HEADER_GAP = 8,
    SLIDER_ROW_VALUE_WIDTH = 72,
    SLIDER_ROW_UNIT_WIDTH = 22,
    SLIDER_ROW_TRACK_HEIGHT = 8,
    SLIDER_ROW_TRACK_RADIUS = 4,
    SLIDER_ROW_KNOB_PADDING = 5,
    SLIDER_ROW_KNOB_RADIUS = 10,
};

static bool has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static bool validate_state(
    const app_ui_slider_row_state_t *state,
    int32_t *step_count)
{
    int64_t range;
    int64_t value_offset;
    int64_t count;

    if(state == NULL || !has_text(state->title) ||
       state->max <= state->min || state->step <= 0) {
        return false;
    }

    range = (int64_t)state->max - (int64_t)state->min;
    value_offset = (int64_t)state->value - (int64_t)state->min;
    if(value_offset < 0 || value_offset > range ||
       range % state->step != 0 ||
       value_offset % state->step != 0) {
        return false;
    }

    count = range / state->step;
    if(count <= 0 || count > INT32_MAX) {
        return false;
    }

    if(step_count != NULL) {
        *step_count = (int32_t)count;
    }
    return true;
}

static int32_t row_height(bool has_subtitle)
{
    const int32_t line_height = (int32_t)App_UiTheme_GetFont(
        APP_UI_THEME_FONT_BODY)->line_height;
    const int32_t slider_visual_height = SLIDER_ROW_TRACK_HEIGHT +
                                         (SLIDER_ROW_KNOB_PADDING * 2);
    int32_t required_height = (SLIDER_ROW_VERTICAL_PADDING * 2) +
                              line_height + SLIDER_ROW_GAP +
                              slider_visual_height;
    const int32_t preferred_height = has_subtitle
                                         ? SLIDER_ROW_SUBTITLE_HEIGHT
                                         : SLIDER_ROW_HEIGHT;

    if(has_subtitle) {
        required_height += SLIDER_ROW_GAP + line_height;
    }
    return required_height > preferred_height
               ? required_height
               : preferred_height;
}

static void set_value_text(app_ui_slider_row_t *row, int32_t value)
{
    char text[16];

    (void)snprintf(text, sizeof(text), "%ld", (long)value);
    lv_label_set_text(row->value_label, text);
}

static void apply_enabled_state(app_ui_slider_row_t *row, bool enabled)
{
    if(enabled) {
        lv_obj_remove_state(row->root, LV_STATE_DISABLED);
        lv_obj_remove_state(row->slider, LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(row->root, LV_STATE_DISABLED);
        lv_obj_add_state(row->slider, LV_STATE_DISABLED);
    }
}

static void apply_state(
    app_ui_slider_row_t *row,
    const app_ui_slider_row_state_t *state,
    int32_t step_count)
{
    int32_t step_index = (int32_t)(
        ((int64_t)state->value - (int64_t)state->min) / state->step);
    bool show_subtitle = has_text(state->subtitle);
    bool show_unit = has_text(state->unit);

    row->updating = true;
    lv_obj_set_height(
        row->root,
        row_height(show_subtitle));
    lv_label_set_text(row->title_label, state->title);

    if(show_subtitle) {
        lv_label_set_text(row->subtitle_label, state->subtitle);
        lv_obj_remove_flag(row->subtitle_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(row->subtitle_label, "");
        lv_obj_add_flag(row->subtitle_label, LV_OBJ_FLAG_HIDDEN);
    }

    set_value_text(row, state->value);
    if(show_unit) {
        lv_obj_set_width(
            row->value_label,
            SLIDER_ROW_VALUE_WIDTH - SLIDER_ROW_UNIT_WIDTH);
        lv_obj_set_width(row->unit_label, SLIDER_ROW_UNIT_WIDTH);
        lv_label_set_text(row->unit_label, state->unit);
        lv_obj_remove_flag(row->unit_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_width(row->value_label, SLIDER_ROW_VALUE_WIDTH);
        lv_label_set_text(row->unit_label, "");
        lv_obj_add_flag(row->unit_label, LV_OBJ_FLAG_HIDDEN);
    }

    lv_slider_set_range(row->slider, 0, step_count);
    lv_slider_set_value(row->slider, step_index, LV_ANIM_OFF);
    apply_enabled_state(row, state->enabled);
    row->id = state->id;
    row->min = state->min;
    row->max = state->max;
    row->step = state->step;
    row->value = state->value;
    row->enabled = state->enabled;
    row->updating = false;
}

static void slider_value_changed_cb(lv_event_t *event)
{
    app_ui_slider_row_t *row = lv_event_get_user_data(event);
    int32_t step_index;
    int64_t value;

    if(row == NULL || row->slider == NULL || row->updating ||
       !row->enabled) {
        return;
    }

    step_index = lv_slider_get_value(row->slider);
    value = (int64_t)row->min + (int64_t)step_index * row->step;
    if(value < row->min || value > row->max) {
        return;
    }

    row->value = (int32_t)value;
    set_value_text(row, row->value);
    if(row->callbacks.on_value_changed != NULL) {
        row->callbacks.on_value_changed(
            row->id,
            row->value,
            row->callbacks.user_data);
    }
}

static void row_deleted_cb(lv_event_t *event)
{
    app_ui_slider_row_t *row = lv_event_get_user_data(event);

    if(row != NULL) {
        App_UiMotion_StopObject(row->root);
        App_UiMotion_StopObject(row->slider);
        memset(row, 0, sizeof(*row));
    }
}

static void style_label(
    lv_obj_t *label,
    app_ui_theme_font_id_t font,
    app_ui_theme_color_id_t color)
{
    const lv_font_t *resolved_font = App_UiTheme_GetFont(font);

    lv_obj_set_style_text_font(label, resolved_font, 0);
    lv_obj_set_style_text_color(label, App_UiTheme_GetColor(color), 0);
    lv_obj_set_height(label, (int32_t)resolved_font->line_height);
}

static bool create_children(app_ui_slider_row_t *row)
{
    row->header = lv_obj_create(row->root);
    if(row->header == NULL) {
        return false;
    }
    lv_obj_remove_style_all(row->header);
    lv_obj_remove_flag(row->header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(row->header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row->header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row->header, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row->header, SLIDER_ROW_HEADER_GAP, 0);

    row->title_label = lv_label_create(row->header);
    if(row->title_label == NULL) {
        return false;
    }
    lv_obj_set_size(row->title_label, 0, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(row->title_label, 1);
    lv_label_set_long_mode(row->title_label, LV_LABEL_LONG_MODE_DOTS);
    style_label(row->title_label, APP_UI_THEME_FONT_BODY,
                APP_UI_THEME_COLOR_TEXT_PRIMARY);

    row->value_box = lv_obj_create(row->header);
    if(row->value_box == NULL) {
        return false;
    }
    lv_obj_remove_style_all(row->value_box);
    lv_obj_remove_flag(row->value_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(
        row->value_box, SLIDER_ROW_VALUE_WIDTH, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row->value_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row->value_box, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    row->value_label = lv_label_create(row->value_box);
    if(row->value_label == NULL) {
        return false;
    }
    lv_label_set_long_mode(row->value_label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_align(
        row->value_label, LV_TEXT_ALIGN_RIGHT, 0);
    style_label(row->value_label, APP_UI_THEME_FONT_BODY,
                APP_UI_THEME_COLOR_TEXT_PRIMARY);

    row->unit_label = lv_label_create(row->value_box);
    if(row->unit_label == NULL) {
        return false;
    }
    lv_label_set_long_mode(row->unit_label, LV_LABEL_LONG_MODE_DOTS);
    style_label(row->unit_label, APP_UI_THEME_FONT_BODY,
                APP_UI_THEME_COLOR_TEXT_PRIMARY);

    row->subtitle_label = lv_label_create(row->root);
    if(row->subtitle_label == NULL) {
        return false;
    }
    lv_obj_set_size(row->subtitle_label, LV_PCT(100), LV_SIZE_CONTENT);
    lv_label_set_long_mode(row->subtitle_label, LV_LABEL_LONG_MODE_DOTS);
    style_label(row->subtitle_label, APP_UI_THEME_FONT_BODY,
                APP_UI_THEME_COLOR_TEXT_MUTED);

    row->slider = lv_slider_create(row->root);
    if(row->slider == NULL) {
        return false;
    }
    lv_obj_remove_style_all(row->slider);
    lv_obj_set_size(row->slider, LV_PCT(100), SLIDER_ROW_TRACK_HEIGHT);
    lv_obj_add_flag(row->slider, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_radius(row->slider, SLIDER_ROW_TRACK_RADIUS, 0);
    lv_obj_set_style_bg_color(
        row->slider,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_FOCUS_BORDER),
        0);
    lv_obj_set_style_bg_opa(row->slider, LV_OPA_40, 0);
    lv_obj_set_style_radius(
        row->slider, SLIDER_ROW_TRACK_RADIUS, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(
        row->slider,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT),
        LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(row->slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(
        row->slider, SLIDER_ROW_KNOB_RADIUS, LV_PART_KNOB);
    lv_obj_set_style_bg_color(
        row->slider,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT),
        LV_PART_KNOB);
    lv_obj_set_style_bg_opa(row->slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_pad_all(
        row->slider, SLIDER_ROW_KNOB_PADDING, LV_PART_KNOB);
    lv_obj_set_style_border_width(row->slider, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(
        row->slider,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_FOREGROUND),
        LV_PART_KNOB);
    lv_obj_set_style_border_opa(row->slider, LV_OPA_COVER, LV_PART_KNOB);

    App_UiComponent_ApplyFocusRing(
        row->slider, APP_UI_COMPONENT_FOCUS_LIGHT);
    App_UiMotion_ApplySlider(row->slider);
    lv_obj_add_event_cb(
        row->slider,
        slider_value_changed_cb,
        LV_EVENT_VALUE_CHANGED,
        row);
    return true;
}

lv_obj_t *App_UiSliderRow_Create(
    lv_obj_t *parent,
    app_ui_slider_row_t *row,
    const app_ui_slider_row_state_t *state,
    const app_ui_slider_row_callbacks_t *callbacks)
{
    int32_t step_count;

    if(parent == NULL || row == NULL ||
       !validate_state(state, &step_count)) {
        return NULL;
    }

    memset(row, 0, sizeof(*row));
    if(callbacks != NULL) {
        row->callbacks = *callbacks;
    }

    row->root = lv_obj_create(parent);
    if(row->root == NULL) {
        memset(row, 0, sizeof(*row));
        return NULL;
    }
    lv_obj_remove_style_all(row->root);
    lv_obj_remove_flag(row->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(row->root, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_width(row->root, LV_PCT(100));
    lv_obj_set_flex_flow(row->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(row->root, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(row->root, SLIDER_ROW_RADIUS, 0);
    lv_obj_set_style_bg_color(
        row->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
        0);
    lv_obj_set_style_bg_opa(row->root, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(row->root, SLIDER_ROW_HORIZONTAL_PADDING, 0);
    lv_obj_set_style_pad_ver(row->root, SLIDER_ROW_VERTICAL_PADDING, 0);
    lv_obj_set_style_pad_row(row->root, SLIDER_ROW_GAP, 0);
    lv_obj_set_style_opa(row->root, LV_OPA_40, LV_STATE_DISABLED);
    lv_obj_add_event_cb(row->root, row_deleted_cb, LV_EVENT_DELETE, row);

    if(!create_children(row)) {
        lv_obj_delete(row->root);
        return NULL;
    }

    apply_state(row, state, step_count);
    return row->root;
}

bool App_UiSliderRow_Update(
    app_ui_slider_row_t *row,
    const app_ui_slider_row_state_t *state)
{
    int32_t step_count;

    if(row == NULL || row->root == NULL || row->title_label == NULL ||
       row->subtitle_label == NULL || row->value_label == NULL ||
       row->unit_label == NULL || row->slider == NULL ||
       !validate_state(state, &step_count)) {
        return false;
    }

    apply_state(row, state, step_count);
    return true;
}
