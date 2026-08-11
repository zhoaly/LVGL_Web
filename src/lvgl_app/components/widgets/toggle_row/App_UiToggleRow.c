/**
 * @file App_UiToggleRow.c
 * @brief Reusable settings-style toggle row implementation.
 */

#include "App_UiToggleRow.h"

#include <string.h>

#include "../../../assets/App_UiTheme.h"
#include "../../App_UiComponents.h"
#include "../../motion/App_UiMotion.h"

enum {
    TOGGLE_ROW_HEIGHT = 52,
    TOGGLE_ROW_SUBTITLE_HEIGHT = 64,
    TOGGLE_ROW_RADIUS = 10,
    TOGGLE_ROW_HORIZONTAL_PADDING = 12,
    TOGGLE_ROW_TEXT_GAP = 2,
    TOGGLE_ROW_COLUMN_GAP = 8,
    TOGGLE_ROW_SWITCH_WIDTH = 40,
    TOGGLE_ROW_SWITCH_HEIGHT = 22,
};

static bool has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static bool validate_state(const app_ui_toggle_row_state_t *state)
{
    return state != NULL && has_text(state->title);
}

static void set_checked(app_ui_toggle_row_t *row, bool checked)
{
    row->checked = checked;
    if(checked) {
        lv_obj_add_state(row->switch_control, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(row->switch_control, LV_STATE_CHECKED);
    }
}

static void set_enabled(app_ui_toggle_row_t *row, bool enabled)
{
    row->enabled = enabled;
    if(enabled) {
        lv_obj_remove_state(row->root, LV_STATE_DISABLED);
        lv_obj_add_flag(row->root, LV_OBJ_FLAG_CLICKABLE);
    } else {
        lv_obj_add_state(row->root, LV_STATE_DISABLED);
        lv_obj_remove_flag(row->root, LV_OBJ_FLAG_CLICKABLE);
    }
}

static void row_clicked_cb(lv_event_t *event)
{
    app_ui_toggle_row_t *row = lv_event_get_user_data(event);
    bool checked;

    if(row == NULL || row->root == NULL || !row->enabled) {
        return;
    }

    checked = !row->checked;
    set_checked(row, checked);
    if(row->callbacks.on_changed != NULL) {
        row->callbacks.on_changed(
            row->id, checked, row->callbacks.user_data);
    }
}

static void row_deleted_cb(lv_event_t *event)
{
    app_ui_toggle_row_t *row = lv_event_get_user_data(event);

    if(row != NULL) {
        App_UiMotion_StopObject(row->root);
        App_UiMotion_StopObject(row->switch_control);
        memset(row, 0, sizeof(*row));
    }
}

static lv_obj_t *create_label(
    lv_obj_t *parent,
    const lv_font_t *font,
    lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);

    if(label == NULL) {
        return NULL;
    }
    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_width(label, LV_PCT(100));
    return label;
}

lv_obj_t *App_UiToggleRow_Create(
    lv_obj_t *parent,
    app_ui_toggle_row_t *row,
    const app_ui_toggle_row_state_t *state,
    const app_ui_toggle_row_callbacks_t *callbacks)
{
    if(parent == NULL || row == NULL || !validate_state(state)) {
        return NULL;
    }

    memset(row, 0, sizeof(*row));
    if(callbacks != NULL) {
        row->callbacks = *callbacks;
    }

    row->root = lv_button_create(parent);
    if(row->root == NULL) {
        memset(row, 0, sizeof(*row));
        return NULL;
    }
    lv_obj_add_event_cb(
        row->root, row_deleted_cb, LV_EVENT_DELETE, row);
    lv_obj_remove_style_all(row->root);
    lv_obj_set_width(row->root, LV_PCT(100));
    lv_obj_set_flex_flow(row->root, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row->root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(row->root, TOGGLE_ROW_RADIUS, 0);
    lv_obj_set_style_bg_color(
        row->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
        0);
    lv_obj_set_style_bg_opa(row->root, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(
        row->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_FOCUS_BORDER),
        LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(
        row->root, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_pad_hor(
        row->root, TOGGLE_ROW_HORIZONTAL_PADDING, 0);
    lv_obj_set_style_pad_column(row->root, TOGGLE_ROW_COLUMN_GAP, 0);
    lv_obj_set_style_opa(
        row->root, LV_OPA_40, LV_STATE_DISABLED);
    lv_obj_remove_flag(row->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(row->root, LV_OBJ_FLAG_EVENT_BUBBLE);
    App_UiComponent_ApplyFocusStyle(
        row->root, APP_UI_COMPONENT_FOCUS_LIGHT);
    lv_obj_add_event_cb(
        row->root, row_clicked_cb, LV_EVENT_CLICKED, row);

    row->text_column = lv_obj_create(row->root);
    if(row->text_column == NULL) {
        lv_obj_delete(row->root);
        return NULL;
    }
    lv_obj_remove_style_all(row->text_column);
    lv_obj_set_size(row->text_column, 0, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(row->text_column, 1);
    lv_obj_set_flex_flow(row->text_column, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(row->text_column, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(
        row->text_column, TOGGLE_ROW_TEXT_GAP, 0);
    lv_obj_remove_flag(row->text_column, LV_OBJ_FLAG_SCROLLABLE);

    row->title_label = create_label(
        row->text_column,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY));
    row->subtitle_label = create_label(
        row->text_column,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED));
    row->switch_control = lv_switch_create(row->root);
    if(row->title_label == NULL || row->subtitle_label == NULL ||
       row->switch_control == NULL) {
        lv_obj_delete(row->root);
        return NULL;
    }

    lv_obj_remove_style_all(row->switch_control);
    lv_obj_set_size(
        row->switch_control,
        TOGGLE_ROW_SWITCH_WIDTH,
        TOGGLE_ROW_SWITCH_HEIGHT);
    lv_obj_set_style_radius(
        row->switch_control, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(
        row->switch_control,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_FOCUS_BORDER),
        0);
    lv_obj_set_style_bg_opa(
        row->switch_control, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(
        row->switch_control, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(
        row->switch_control,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT),
        LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(
        row->switch_control,
        LV_OPA_COVER,
        LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_radius(
        row->switch_control, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_bg_color(
        row->switch_control,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_FOREGROUND),
        LV_PART_KNOB);
    lv_obj_set_style_bg_opa(
        row->switch_control, LV_OPA_COVER, LV_PART_KNOB);
    App_UiMotion_ApplySwitch(row->switch_control);
    lv_group_remove_obj(row->switch_control);
    lv_obj_remove_flag(row->switch_control, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(row->switch_control, LV_OBJ_FLAG_SCROLLABLE);

    if(!App_UiToggleRow_Update(row, state)) {
        lv_obj_delete(row->root);
        return NULL;
    }
    return row->root;
}

bool App_UiToggleRow_Update(
    app_ui_toggle_row_t *row,
    const app_ui_toggle_row_state_t *state)
{
    if(row == NULL || row->root == NULL || row->title_label == NULL ||
       row->subtitle_label == NULL || row->switch_control == NULL ||
       !validate_state(state)) {
        return false;
    }

    row->id = state->id;
    lv_label_set_text(row->title_label, state->title);
    if(has_text(state->subtitle)) {
        lv_label_set_text(row->subtitle_label, state->subtitle);
        lv_obj_remove_flag(row->subtitle_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(row->subtitle_label, "");
        lv_obj_add_flag(row->subtitle_label, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_set_height(
        row->root,
        has_text(state->subtitle)
            ? TOGGLE_ROW_SUBTITLE_HEIGHT
            : TOGGLE_ROW_HEIGHT);
    set_checked(row, state->checked);
    set_enabled(row, state->enabled);
    return true;
}
