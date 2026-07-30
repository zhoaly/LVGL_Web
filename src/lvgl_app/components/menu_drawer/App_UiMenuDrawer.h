/**
 * @file App_UiMenuDrawer.h
 * @brief Home 左侧菜单抽屉组件。
 */

#ifndef APP_UI_MENU_DRAWER_H
#define APP_UI_MENU_DRAWER_H

#include <stdbool.h>
#include <stddef.h>

#include "lvgl/lvgl.h"

#include "../../app/App_Ui.h"
#include "../../assets/App_UiAssets.h"
#include "../App_UiComponents.h"

#define APP_UI_MENU_DRAWER_MAX_ITEMS 6u

typedef struct {
    app_ui_icon_id_t icon_id;
    const char *label;
    app_ui_page_id_t target_page;
} app_ui_menu_drawer_item_t;

typedef struct {
    lv_obj_t *root;
    lv_obj_t *scrim;
    lv_obj_t *panel;
    app_ui_action_binding_t bindings[APP_UI_MENU_DRAWER_MAX_ITEMS];
    size_t item_count;
    bool closing;
} app_ui_menu_drawer_t;

/**
 * @brief 在 LVGL 顶层创建并打开菜单抽屉。
 *
 * drawer 必须由静态存储期自动清零，或由调用方在首次使用前清零。
 * 已经打开时直接返回现有根对象，不会重复创建。
 *
 * @return 抽屉根对象；参数无效时返回 NULL。
 */
lv_obj_t *App_UiMenuDrawer_Open(
    app_ui_menu_drawer_t *drawer,
    const app_ui_menu_drawer_item_t *items,
    size_t item_count);

/**
 * @brief 以关闭动画收起抽屉。
 */
void App_UiMenuDrawer_Close(app_ui_menu_drawer_t *drawer);

/**
 * @brief 立即停止动画并销毁抽屉，页面离开时使用。
 */
void App_UiMenuDrawer_Destroy(app_ui_menu_drawer_t *drawer);

/**
 * @brief 判断抽屉是否存在且尚未开始关闭。
 */
bool App_UiMenuDrawer_IsOpen(const app_ui_menu_drawer_t *drawer);

#endif /* APP_UI_MENU_DRAWER_H */
