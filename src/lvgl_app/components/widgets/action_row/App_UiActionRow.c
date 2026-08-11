/**
 * @file App_UiActionRow.c
 * @brief Reusable settings-style action row implementation.
 */

#include "App_UiActionRow.h"

#include <string.h>

#include "../../../assets/App_UiTheme.h"
#include "../../App_UiComponents.h"
#include "../../motion/App_UiMotion.h"

enum {
    ACTION_ROW_HEIGHT = 52,
    ACTION_ROW_SUBTITLE_HEIGHT = 64,
    ACTION_ROW_RADIUS = 10,
    ACTION_ROW_HORIZONTAL_PADDING = 12,
    ACTION_ROW_VERTICAL_PADDING = 6,
    ACTION_ROW_TEXT_GAP = 2,
    ACTION_ROW_COLUMN_GAP = 8,
    ACTION_ROW_VALUE_WIDTH = 72,
};

static bool has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static bool validate_state(const app_ui_action_row_state_t *state)
{
    return state != NULL && has_text(state->title);
}

static void set_optional_label(lv_obj_t *label, const char *text)
{
    if(has_text(text)) {
        lv_label_set_text(label, text);
        lv_obj_remove_flag(label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(label, "");
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
}

static void set_enabled(app_ui_action_row_t *row, bool enabled)
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
    app_ui_action_row_t *row = lv_event_get_user_data(event);

    if(row != NULL && row->root != NULL && row->enabled &&
       row->callbacks.on_activated != NULL) {
        row->callbacks.on_activated(row->id, row->callbacks.user_data);
    }
}

static void row_deleted_cb(lv_event_t *event)
{
    app_ui_action_row_t *row = lv_event_get_user_data(event);

    if(row != NULL) {
        App_UiMotion_StopObject(row->root);
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
    lv_obj_set_height(label, (int32_t)font->line_height);
    return label;
}

static int32_t row_height(bool has_subtitle)
{
    const lv_font_t *font = App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY);
    int32_t required_height = (ACTION_ROW_VERTICAL_PADDING * 2) +
                              (int32_t)font->line_height;
    const int32_t preferred_height = has_subtitle
                                         ? ACTION_ROW_SUBTITLE_HEIGHT
                                         : ACTION_ROW_HEIGHT;

    if(has_subtitle) {
        required_height += ACTION_ROW_TEXT_GAP +
                           (int32_t)font->line_height;
    }
    return required_height > preferred_height
               ? required_height
               : preferred_height;
}

lv_obj_t *App_UiActionRow_Create(
    lv_obj_t *parent,
    app_ui_action_row_t *row,
    const app_ui_action_row_state_t *state,
    const app_ui_action_row_callbacks_t *callbacks)
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
    lv_obj_set_style_radius(row->root, ACTION_ROW_RADIUS, 0);
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
        row->root, ACTION_ROW_HORIZONTAL_PADDING, 0);
    lv_obj_set_style_pad_ver(
        row->root, ACTION_ROW_VERTICAL_PADDING, 0);
    lv_obj_set_style_pad_column(row->root, ACTION_ROW_COLUMN_GAP, 0);
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
        row->text_column, ACTION_ROW_TEXT_GAP, 0);
    lv_obj_remove_flag(row->text_column, LV_OBJ_FLAG_SCROLLABLE);

    row->title_label = create_label(
        row->text_column,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY));
    row->subtitle_label = create_label(
        row->text_column,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED));
    row->value_label = create_label(
        row->root,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED));
    if(row->title_label == NULL || row->subtitle_label == NULL ||
       row->value_label == NULL) {
        lv_obj_delete(row->root);
        return NULL;
    }

    lv_obj_set_width(row->title_label, LV_PCT(100));
    lv_obj_set_width(row->subtitle_label, LV_PCT(100));
    lv_obj_set_width(row->value_label, ACTION_ROW_VALUE_WIDTH);
    lv_obj_set_style_text_align(
        row->value_label, LV_TEXT_ALIGN_RIGHT, 0);

    if(!App_UiActionRow_Update(row, state)) {
        lv_obj_delete(row->root);
        return NULL;
    }
    return row->root;
}

bool App_UiActionRow_Update(
    app_ui_action_row_t *row,
    const app_ui_action_row_state_t *state)
{
    if(row == NULL || row->root == NULL || row->title_label == NULL ||
       row->subtitle_label == NULL || row->value_label == NULL ||
       !validate_state(state)) {
        return false;
    }

    row->id = state->id;
    lv_label_set_text(row->title_label, state->title);
    set_optional_label(row->subtitle_label, state->subtitle);
    set_optional_label(row->value_label, state->value);
    lv_obj_set_height(
        row->root,
        row_height(has_text(state->subtitle)));
    set_enabled(row, state->enabled);
    return true;
}
