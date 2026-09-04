#include "App_UiConfirmDialog.h"
#include <string.h>
#include "../../../assets/App_UiTheme.h"
#include "../../App_UiComponents.h"
#include "../../motion/App_UiMotion.h"

static void deleted(lv_event_t *event)
{
    app_ui_confirm_dialog_t *d = lv_event_get_user_data(event);
    for(lv_indev_t *input = lv_indev_get_next(NULL); input;
        input = lv_indev_get_next(input)) {
        if(lv_indev_get_group(input) == d->group)
            lv_indev_set_group(input, d->previous_group);
    }
    lv_group_set_default(d->previous_group);
    if(d->previous_focus && lv_obj_is_valid(d->previous_focus))
        lv_group_focus_obj(d->previous_focus);
    lv_group_delete(d->group);
    memset(d, 0, sizeof(*d));
}
void App_UiConfirmDialog_Destroy(app_ui_confirm_dialog_t *d)
{
    if(d && d->root) lv_obj_delete(d->root);
}
static void clicked(lv_event_t *event)
{
    app_ui_confirm_dialog_t *d = lv_event_get_user_data(event);
    bool confirmed = lv_event_get_target(event) == d->confirm;
    app_ui_confirm_dialog_callback_t callback = d->callback;
    void *user_data = d->user_data;
    App_UiConfirmDialog_Destroy(d);
    if(callback) callback(confirmed, user_data);
}
bool App_UiConfirmDialog_Update(app_ui_confirm_dialog_t *d,
    const app_ui_confirm_dialog_state_t *s)
{
    if(!d || !d->root || !s || !s->title || !s->message ||
       !s->confirm_text || !s->cancel_text) return false;
    lv_label_set_text(d->title, s->title);
    lv_label_set_text(d->message, s->message);
    lv_label_set_text(d->confirm_label, s->confirm_text);
    lv_label_set_text(d->cancel_label, s->cancel_text);
    if(s->enabled) lv_obj_remove_state(d->confirm, LV_STATE_DISABLED);
    else lv_obj_add_state(d->confirm, LV_STATE_DISABLED);
    return true;
}
static lv_obj_t *label(lv_obj_t *parent, app_ui_theme_font_id_t font)
{
    lv_obj_t *o = lv_label_create(parent);
    lv_obj_set_width(o, LV_PCT(100));
    lv_obj_set_style_text_font(o, App_UiTheme_GetFont(font), 0);
    lv_obj_set_style_text_color(o,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY), 0);
    return o;
}
lv_obj_t *App_UiConfirmDialog_Create(app_ui_confirm_dialog_t *d,
    const app_ui_confirm_dialog_state_t *s,
    app_ui_confirm_dialog_callback_t callback, void *user_data)
{
    if(!d || d->root || !s || !s->title || !s->message ||
       !s->confirm_text || !s->cancel_text) return NULL;
    d->previous_group = lv_group_get_default();
    d->previous_focus = d->previous_group ? lv_group_get_focused(d->previous_group) : NULL;
    d->group = lv_group_create();
    if(!d->group) return NULL;
    lv_group_set_default(d->group);
    d->callback = callback;
    d->user_data = user_data;
    d->root = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(d->root);
    lv_obj_set_size(d->root, LV_PCT(100), LV_PCT(100));
    lv_obj_add_flag(d->root, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(d->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_BACKGROUND), 0);
    lv_obj_set_style_bg_opa(d->root, LV_OPA_40, 0);
    lv_obj_add_event_cb(d->root, deleted, LV_EVENT_DELETE, d);
    lv_obj_t *panel = lv_obj_create(d->root);
    lv_obj_remove_style_all(panel);
    lv_obj_set_style_radius(panel, 12, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_width(panel, LV_PCT(94));
    lv_obj_set_height(panel, LV_SIZE_CONTENT);
    lv_obj_set_style_max_height(panel, LV_PCT(96), 0);
    lv_obj_set_style_bg_color(panel,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED), 0);
    lv_obj_set_style_pad_all(panel, 12, 0);
    lv_obj_set_style_pad_row(panel, 8, 0);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(panel);
    d->title = label(panel, APP_UI_THEME_FONT_EMPHASIS);
    d->message = label(panel, APP_UI_THEME_FONT_BODY);
    d->cancel = lv_button_create(panel);
    d->confirm = lv_button_create(panel);
    lv_obj_t *buttons[] = {d->cancel, d->confirm};
    for(unsigned i = 0; i < 2; ++i) {
        lv_obj_remove_style_all(buttons[i]);
        lv_obj_set_style_radius(buttons[i], 10, 0);
        lv_obj_set_style_bg_opa(buttons[i], LV_OPA_COVER, 0);
        lv_obj_set_size(buttons[i], LV_PCT(100), 40);
        lv_obj_set_style_bg_color(buttons[i],
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_BACKGROUND), 0);
        App_UiComponent_ApplyFocusStyle(buttons[i], APP_UI_COMPONENT_FOCUS_DARK);
        lv_obj_add_event_cb(buttons[i], clicked, LV_EVENT_CLICKED, d);
    }
    d->cancel_label = label(d->cancel, APP_UI_THEME_FONT_BODY);
    d->confirm_label = label(d->confirm, APP_UI_THEME_FONT_BODY);
    lv_obj_set_style_text_color(d->cancel_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND), 0);
    lv_obj_set_style_text_color(d->confirm_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND), 0);
    lv_obj_set_style_text_align(d->cancel_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(d->confirm_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(d->cancel_label);
    lv_obj_center(d->confirm_label);
    for(lv_indev_t *input = lv_indev_get_next(NULL); input;
        input = lv_indev_get_next(input)) {
        if(lv_indev_get_group(input) == d->previous_group)
            lv_indev_set_group(input, d->group);
    }
    lv_group_focus_obj(d->cancel);
    App_UiConfirmDialog_Update(d, s);
    App_UiMotion_AnimateEnter(panel, APP_UI_MOTION_OPACITY_TEXT, 4, 0);
    return d->root;
}
