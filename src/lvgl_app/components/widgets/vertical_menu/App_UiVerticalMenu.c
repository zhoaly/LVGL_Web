/**
 * @file App_UiVerticalMenu.c
 * @brief Reusable vertically scrolling menu component implementation.
 */

#include "App_UiVerticalMenu.h"

#include <string.h>

#include "../../../assets/App_UiTheme.h"
#include "../../App_UiComponents.h"
#include "../../motion/App_UiMotion.h"

enum {
    VERTICAL_MENU_ITEM_HEIGHT = 44,
    VERTICAL_MENU_ITEM_GAP = 8,
    VERTICAL_MENU_ITEM_RADIUS = 10,
    VERTICAL_MENU_SCROLLBAR_WIDTH = 3,
};

static bool validate_items(
    const app_ui_vertical_menu_item_t *items,
    size_t item_count)
{
    size_t first;
    size_t second;

    if(items == NULL || item_count == 0u ||
       item_count > APP_UI_VERTICAL_MENU_MAX_ITEMS) {
        return false;
    }

    for(first = 0u; first < item_count; first++) {
        if(items[first].label == NULL || items[first].label[0] == '\0') {
            return false;
        }
        for(second = first + 1u; second < item_count; second++) {
            if(items[first].id == items[second].id) {
                return false;
            }
        }
    }
    return true;
}

static size_t find_item_index(
    const app_ui_vertical_menu_t *menu,
    const lv_obj_t *button)
{
    size_t index;

    if(menu == NULL || button == NULL) {
        return APP_UI_VERTICAL_MENU_MAX_ITEMS;
    }
    for(index = 0u; index < menu->item_count; index++) {
        if(menu->buttons[index] == button) {
            return index;
        }
    }
    return APP_UI_VERTICAL_MENU_MAX_ITEMS;
}

static void item_clicked_cb(lv_event_t *event)
{
    app_ui_vertical_menu_t *menu = lv_event_get_user_data(event);
    lv_obj_t *button = lv_event_get_target(event);
    size_t index = find_item_index(menu, button);

    if(index < APP_UI_VERTICAL_MENU_MAX_ITEMS &&
       menu->callbacks.on_item_activated != NULL) {
        menu->callbacks.on_item_activated(
            menu->item_ids[index], menu->callbacks.user_data);
    }
}

static void item_focused_cb(lv_event_t *event)
{
    lv_obj_t *button = lv_event_get_target(event);
    lv_anim_enable_t animation =
        App_UiMotion_GetDuration(APP_UI_MOTION_DURATION_CONTROL) == 0u
            ? LV_ANIM_OFF
            : LV_ANIM_ON;

    if(button != NULL) {
        lv_obj_scroll_to_view(button, animation);
    }
}

static void menu_deleted_cb(lv_event_t *event)
{
    app_ui_vertical_menu_t *menu = lv_event_get_user_data(event);

    if(menu != NULL) {
        memset(menu, 0, sizeof(*menu));
    }
}

static bool create_rows(
    app_ui_vertical_menu_t *menu,
    const app_ui_vertical_menu_item_t *items,
    size_t item_count,
    lv_obj_t **buttons,
    lv_obj_t **labels,
    uint32_t *item_ids)
{
    size_t index;

    for(index = 0u; index < item_count; index++) {
        lv_obj_t *button = lv_button_create(menu->root);
        lv_obj_t *label;

        if(button == NULL) {
            return false;
        }
        buttons[index] = button;
        item_ids[index] = items[index].id;

        lv_obj_remove_style_all(button);
        lv_obj_set_size(button, LV_PCT(100), VERTICAL_MENU_ITEM_HEIGHT);
        lv_obj_set_flex_flow(button, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(button, LV_FLEX_ALIGN_START,
                              LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_radius(button, VERTICAL_MENU_ITEM_RADIUS, 0);
        lv_obj_set_style_bg_color(
            button,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
            0);
        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(
            button,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_FOCUS_BORDER),
            LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_STATE_PRESSED);
        lv_obj_set_style_pad_hor(button, 12, 0);
        App_UiComponent_ApplyFocusStyle(
            button, APP_UI_COMPONENT_FOCUS_LIGHT);

        label = lv_label_create(button);
        if(label == NULL) {
            return false;
        }
        labels[index] = label;
        lv_label_set_text(label, items[index].label);
        lv_obj_set_style_text_font(
            label, App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY), 0);
        lv_obj_set_style_text_color(
            label,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
            0);

        lv_obj_add_event_cb(
            button, item_clicked_cb, LV_EVENT_CLICKED, menu);
        lv_obj_add_event_cb(
            button, item_focused_cb, LV_EVENT_FOCUSED, menu);
    }
    return true;
}

static void delete_rows(lv_obj_t *const *buttons)
{
    size_t index;

    for(index = 0u; index < APP_UI_VERTICAL_MENU_MAX_ITEMS; index++) {
        if(buttons[index] != NULL) {
            lv_obj_delete(buttons[index]);
        }
    }
}

static void clear_row_references(app_ui_vertical_menu_t *menu)
{
    memset(menu->buttons, 0, sizeof(menu->buttons));
    memset(menu->labels, 0, sizeof(menu->labels));
    memset(menu->item_ids, 0, sizeof(menu->item_ids));
    menu->item_count = 0u;
}

lv_obj_t *App_UiVerticalMenu_Create(
    lv_obj_t *parent,
    app_ui_vertical_menu_t *menu,
    const app_ui_vertical_menu_item_t *items,
    size_t item_count,
    const app_ui_vertical_menu_callbacks_t *callbacks)
{
    if(parent == NULL || menu == NULL ||
       !validate_items(items, item_count)) {
        return NULL;
    }

    memset(menu, 0, sizeof(*menu));
    if(callbacks != NULL) {
        menu->callbacks = *callbacks;
    }

    menu->root = lv_obj_create(parent);
    if(menu->root == NULL) {
        return NULL;
    }
    lv_obj_remove_style_all(menu->root);
    lv_obj_set_width(menu->root, LV_PCT(100));
    lv_obj_set_flex_grow(menu->root, 1);
    lv_obj_set_flex_flow(menu->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu->root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(menu->root, 4, 0);
    lv_obj_set_style_pad_ver(menu->root, 4, 0);
    lv_obj_set_style_pad_row(menu->root, VERTICAL_MENU_ITEM_GAP, 0);
    lv_obj_add_flag(menu->root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(menu->root, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(menu->root, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_width(
        menu->root, VERTICAL_MENU_SCROLLBAR_WIDTH, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(menu->root, VERTICAL_MENU_SCROLLBAR_WIDTH,
                            LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(
        menu->root,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
        LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(
        menu->root, LV_OPA_40, LV_PART_SCROLLBAR);
    lv_obj_add_event_cb(
        menu->root, menu_deleted_cb, LV_EVENT_DELETE, menu);

    if(!create_rows(menu, items, item_count,
                    menu->buttons, menu->labels, menu->item_ids)) {
        lv_obj_delete(menu->root);
        return NULL;
    }
    menu->item_count = item_count;
    return menu->root;
}

bool App_UiVerticalMenu_Update(
    app_ui_vertical_menu_t *menu,
    const app_ui_vertical_menu_item_t *items,
    size_t item_count)
{
    lv_obj_t *new_buttons[APP_UI_VERTICAL_MENU_MAX_ITEMS] = {0};
    lv_obj_t *new_labels[APP_UI_VERTICAL_MENU_MAX_ITEMS] = {0};
    uint32_t new_item_ids[APP_UI_VERTICAL_MENU_MAX_ITEMS] = {0};
    size_t index;

    if(menu == NULL || menu->root == NULL ||
       !validate_items(items, item_count)) {
        return false;
    }

    if(item_count == menu->item_count) {
        for(index = 0u; index < item_count; index++) {
            menu->item_ids[index] = items[index].id;
            lv_label_set_text(menu->labels[index], items[index].label);
        }
        return true;
    }

    if(!create_rows(menu, items, item_count,
                    new_buttons, new_labels, new_item_ids)) {
        delete_rows(new_buttons);
        return false;
    }

    delete_rows(menu->buttons);
    clear_row_references(menu);
    memcpy(menu->buttons, new_buttons, sizeof(new_buttons));
    memcpy(menu->labels, new_labels, sizeof(new_labels));
    memcpy(menu->item_ids, new_item_ids, sizeof(new_item_ids));
    menu->item_count = item_count;
    lv_obj_scroll_to_y(menu->root, 0, LV_ANIM_OFF);
    return true;
}
