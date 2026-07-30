/**
 * @file App_UiMenuDrawer.c
 * @brief 黑白风格左侧菜单抽屉实现。
 */

#include "App_UiMenuDrawer.h"

#include <string.h>

#include "../../assets/App_UiTheme.h"
#include "../motion/App_UiMotion.h"

enum {
    MENU_DRAWER_WIDTH = 180,
    MENU_DRAWER_ITEM_HEIGHT = 44,
    MENU_DRAWER_ICON_SIZE = 18,
    MENU_DRAWER_CLOSE_SIZE = 32,
};

static void panel_x_anim_cb(void *object, int32_t value)
{
    lv_obj_set_x((lv_obj_t *)object, value);
}

static void scrim_opa_anim_cb(void *object, int32_t value)
{
    lv_obj_set_style_bg_opa((lv_obj_t *)object, (lv_opa_t)value, 0);
}

static void drawer_delete_cb(lv_event_t *event)
{
    app_ui_menu_drawer_t *drawer = lv_event_get_user_data(event);

    if(drawer == NULL) {
        return;
    }

    drawer->root = NULL;
    drawer->scrim = NULL;
    drawer->panel = NULL;
    drawer->item_count = 0u;
    drawer->closing = false;
}

static void close_anim_completed_cb(lv_anim_t *animation)
{
    app_ui_menu_drawer_t *drawer = lv_anim_get_user_data(animation);

    if(drawer != NULL && drawer->root != NULL) {
        lv_obj_delete(drawer->root);
    }
}

static void close_click_cb(lv_event_t *event)
{
    App_UiMenuDrawer_Close(lv_event_get_user_data(event));
}

static void item_click_cb(lv_event_t *event)
{
    App_UiMenuDrawer_Close(lv_event_get_user_data(event));
}

static lv_obj_t *create_icon(lv_obj_t *parent, app_ui_icon_id_t icon_id)
{
    lv_obj_t *icon = lv_image_create(parent);

    lv_image_set_src(icon, App_UiAssets_GetIcon(icon_id));
    lv_obj_set_size(icon, MENU_DRAWER_ICON_SIZE, MENU_DRAWER_ICON_SIZE);
    lv_obj_set_style_image_recolor(
        icon,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_image_recolor_opa(icon, LV_OPA_COVER, 0);
    return icon;
}

static lv_obj_t *create_close_button(
    lv_obj_t *parent,
    app_ui_menu_drawer_t *drawer)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_t *icon;

    lv_obj_remove_style_all(button);
    lv_obj_set_size(button, MENU_DRAWER_CLOSE_SIZE, MENU_DRAWER_CLOSE_SIZE);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(
        button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_20, LV_STATE_PRESSED);
    App_UiComponent_ApplyFocusStyle(
        button, APP_UI_COMPONENT_FOCUS_DARK);
    lv_obj_add_event_cb(button, close_click_cb, LV_EVENT_CLICKED, drawer);

    icon = create_icon(button, APP_UI_ICON_MENU_CLOSE);
    lv_obj_center(icon);
    return button;
}

static lv_obj_t *create_menu_item(
    lv_obj_t *parent,
    app_ui_menu_drawer_t *drawer,
    const app_ui_menu_drawer_item_t *item,
    app_ui_action_binding_t *binding)
{
    lv_obj_t *button;
    lv_obj_t *label;
    lv_obj_t *spacer;

    button = lv_button_create(parent);
    lv_obj_remove_style_all(button);
    lv_obj_set_size(button, LV_PCT(100), MENU_DRAWER_ITEM_HEIGHT);
    lv_obj_set_flex_flow(button, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(button, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_bg_color(
        button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_20, LV_STATE_PRESSED);
    lv_obj_set_style_pad_hor(button, 12, 0);
    App_UiComponent_ApplyFocusStyle(
        button, APP_UI_COMPONENT_FOCUS_DARK);

    (void)create_icon(button, item->icon_id);

    spacer = lv_obj_create(button);
    lv_obj_remove_style_all(spacer);
    lv_obj_set_size(spacer, 10, 1);

    label = lv_label_create(button);
    lv_label_set_text(label, item->label);
    lv_obj_set_style_text_font(
        label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        0);
    lv_obj_set_style_text_color(
        label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);

    App_UiComponent_InitAction(
        binding, APP_ACTION_ID_UI_NAV_PUSH, item->target_page);
    App_UiComponent_BindAction(button, binding);
    lv_obj_add_event_cb(button, item_click_cb, LV_EVENT_CLICKED, drawer);
    return button;
}

static bool validate_items(
    const app_ui_menu_drawer_item_t *items,
    size_t item_count)
{
    size_t index;

    if(items == NULL || item_count == 0u ||
       item_count > APP_UI_MENU_DRAWER_MAX_ITEMS) {
        return false;
    }

    for(index = 0u; index < item_count; index++) {
        if(items[index].label == NULL ||
           items[index].target_page <= APP_UI_PAGE_NONE ||
           items[index].target_page >= APP_UI_PAGE_COUNT ||
           App_UiAssets_GetIcon(items[index].icon_id) == NULL) {
            return false;
        }
    }
    return true;
}

lv_obj_t *App_UiMenuDrawer_Open(
    app_ui_menu_drawer_t *drawer,
    const app_ui_menu_drawer_item_t *items,
    size_t item_count)
{
    lv_anim_t animation;
    uint32_t duration;
    lv_obj_t *footer;
    lv_obj_t *header;
    lv_obj_t *header_label;
    lv_obj_t *divider;
    lv_obj_t *footer_spacer;
    size_t index;

    if(drawer == NULL || !validate_items(items, item_count)) {
        return NULL;
    }
    if(drawer->root != NULL) {
        return drawer->root;
    }

    memset(drawer, 0, sizeof(*drawer));
    drawer->item_count = item_count;

    drawer->root = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(drawer->root);
    lv_obj_set_size(drawer->root, LV_PCT(100), LV_PCT(100));
    lv_obj_add_event_cb(
        drawer->root, drawer_delete_cb, LV_EVENT_DELETE, drawer);

    drawer->scrim = lv_obj_create(drawer->root);
    lv_obj_remove_style_all(drawer->scrim);
    lv_obj_set_size(drawer->scrim, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(
        drawer->scrim,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_SHADOW),
        0);
    lv_obj_set_style_bg_opa(drawer->scrim, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(drawer->scrim, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        drawer->scrim, close_click_cb, LV_EVENT_CLICKED, drawer);

    drawer->panel = lv_obj_create(drawer->root);
    lv_obj_remove_style_all(drawer->panel);
    lv_obj_set_size(drawer->panel, MENU_DRAWER_WIDTH, LV_PCT(100));
    lv_obj_set_x(drawer->panel, -MENU_DRAWER_WIDTH);
    lv_obj_set_flex_flow(drawer->panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(
        drawer->panel,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_BACKGROUND),
        0);
    lv_obj_set_style_bg_opa(drawer->panel, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(drawer->panel, 14, 0);
    lv_obj_set_style_pad_row(drawer->panel, 8, 0);
    lv_obj_set_style_shadow_width(drawer->panel, 18, 0);
    lv_obj_set_style_shadow_color(
        drawer->panel,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_SHADOW),
        0);
    lv_obj_set_style_shadow_opa(drawer->panel, LV_OPA_40, 0);
    lv_obj_set_style_shadow_offset_x(drawer->panel, 4, 0);
    lv_obj_add_flag(drawer->panel, LV_OBJ_FLAG_CLICKABLE);

    header = lv_obj_create(drawer->panel);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, LV_PCT(100), 40);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    header_label = lv_label_create(header);
    lv_label_set_text(header_label, "USB HUB");
    lv_obj_set_style_text_font(
        header_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);
    lv_obj_set_style_text_color(
        header_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    (void)create_close_button(header, drawer);

    divider = lv_obj_create(drawer->panel);
    lv_obj_remove_style_all(divider);
    lv_obj_set_size(divider, LV_PCT(100), 1);
    lv_obj_set_style_bg_color(
        divider,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_bg_opa(divider, LV_OPA_20, 0);

    for(index = 0u; index < item_count; index++) {
        (void)create_menu_item(
            drawer->panel,
            drawer,
            &items[index],
            &drawer->bindings[index]);
    }

    footer_spacer = lv_obj_create(drawer->panel);
    lv_obj_remove_style_all(footer_spacer);
    lv_obj_set_width(footer_spacer, LV_PCT(100));
    lv_obj_set_height(footer_spacer, 0);
    lv_obj_set_flex_grow(footer_spacer, 1);

    footer = lv_label_create(drawer->panel);
    lv_label_set_text(footer, "ESP32 / 2 PORTS");
    lv_obj_set_style_text_font(
        footer,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        0);
    lv_obj_set_style_text_color(
        footer,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
        0);

    duration = App_UiMotion_GetDuration(
        APP_UI_MOTION_DURATION_DRAWER_OPEN);
    if(duration == 0u) {
        scrim_opa_anim_cb(drawer->scrim, LV_OPA_40);
        panel_x_anim_cb(drawer->panel, 0);
    } else {
        lv_anim_init(&animation);
        lv_anim_set_var(&animation, drawer->scrim);
        lv_anim_set_exec_cb(&animation, scrim_opa_anim_cb);
        lv_anim_set_values(&animation, LV_OPA_TRANSP, LV_OPA_40);
        lv_anim_set_duration(&animation, duration);
        lv_anim_set_path_cb(&animation, App_UiMotion_GetPath());
        lv_anim_start(&animation);

        lv_anim_init(&animation);
        lv_anim_set_var(&animation, drawer->panel);
        lv_anim_set_exec_cb(&animation, panel_x_anim_cb);
        lv_anim_set_values(&animation, -MENU_DRAWER_WIDTH, 0);
        lv_anim_set_duration(&animation, duration);
        lv_anim_set_path_cb(&animation, App_UiMotion_GetPath());
        lv_anim_start(&animation);
    }
    return drawer->root;
}

void App_UiMenuDrawer_Close(app_ui_menu_drawer_t *drawer)
{
    lv_anim_t animation;
    uint32_t duration;

    if(drawer == NULL || drawer->root == NULL || drawer->closing) {
        return;
    }
    drawer->closing = true;

    lv_anim_delete(drawer->panel, panel_x_anim_cb);
    lv_anim_delete(drawer->scrim, scrim_opa_anim_cb);
    duration = App_UiMotion_GetDuration(
        APP_UI_MOTION_DURATION_DRAWER_CLOSE);
    if(duration == 0u) {
        lv_obj_delete(drawer->root);
        return;
    }

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, drawer->scrim);
    lv_anim_set_exec_cb(&animation, scrim_opa_anim_cb);
    lv_anim_set_values(&animation, LV_OPA_40, LV_OPA_TRANSP);
    lv_anim_set_duration(&animation, duration);
    lv_anim_set_path_cb(&animation, App_UiMotion_GetPath());
    lv_anim_start(&animation);

    lv_anim_init(&animation);
    lv_anim_set_var(&animation, drawer->panel);
    lv_anim_set_exec_cb(&animation, panel_x_anim_cb);
    lv_anim_set_values(&animation, 0, -MENU_DRAWER_WIDTH);
    lv_anim_set_duration(&animation, duration);
    lv_anim_set_path_cb(&animation, App_UiMotion_GetPath());
    lv_anim_set_user_data(&animation, drawer);
    lv_anim_set_completed_cb(&animation, close_anim_completed_cb);
    lv_anim_start(&animation);
}

void App_UiMenuDrawer_Destroy(app_ui_menu_drawer_t *drawer)
{
    if(drawer == NULL || drawer->root == NULL) {
        return;
    }

    lv_anim_delete(drawer->panel, panel_x_anim_cb);
    lv_anim_delete(drawer->scrim, scrim_opa_anim_cb);
    lv_obj_delete(drawer->root);
}

bool App_UiMenuDrawer_IsOpen(const app_ui_menu_drawer_t *drawer)
{
    return drawer != NULL && drawer->root != NULL && !drawer->closing;
}
