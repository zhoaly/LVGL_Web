/**
 * @file App_UiVerticalMenu.h
 * @brief Reusable vertically scrolling menu component.
 */

#ifndef APP_UI_VERTICAL_MENU_H
#define APP_UI_VERTICAL_MENU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lvgl/lvgl.h"

#define APP_UI_VERTICAL_MENU_MAX_ITEMS 16u

typedef struct {
    uint32_t id;
    const char *label;
} app_ui_vertical_menu_item_t;

typedef void (*app_ui_vertical_menu_activate_cb_t)(
    uint32_t item_id,
    void *user_data);

typedef struct {
    app_ui_vertical_menu_activate_cb_t on_item_activated;
    void *user_data;
} app_ui_vertical_menu_callbacks_t;

typedef struct {
    lv_obj_t *root;
    lv_obj_t *buttons[APP_UI_VERTICAL_MENU_MAX_ITEMS];
    lv_obj_t *labels[APP_UI_VERTICAL_MENU_MAX_ITEMS];
    uint32_t item_ids[APP_UI_VERTICAL_MENU_MAX_ITEMS];
    size_t item_count;
    app_ui_vertical_menu_callbacks_t callbacks;
} app_ui_vertical_menu_t;

/**
 * @brief Create a vertically scrolling menu and copy the initial item state.
 * @return Menu root object, or NULL when the arguments are invalid.
 */
lv_obj_t *App_UiVerticalMenu_Create(
    lv_obj_t *parent,
    app_ui_vertical_menu_t *menu,
    const app_ui_vertical_menu_item_t *items,
    size_t item_count,
    const app_ui_vertical_menu_callbacks_t *callbacks);

/**
 * @brief Update menu items without changing the configured callbacks.
 *
 * When the item count is unchanged, rows are updated in place. A count change
 * rebuilds the rows and resets the menu to the top.
 *
 * @return true when the update was applied; false for invalid arguments.
 */
bool App_UiVerticalMenu_Update(
    app_ui_vertical_menu_t *menu,
    const app_ui_vertical_menu_item_t *items,
    size_t item_count);

#endif /* APP_UI_VERTICAL_MENU_H */
