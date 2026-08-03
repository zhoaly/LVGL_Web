/**
 * @file App_UiPageDebug.c
 * @brief Debug page with a reusable vertical menu.
 */

#include "App_UiPageDebug.h"

#include "lvgl/lvgl.h"

#include "../../components/widgets/vertical_menu/App_UiVerticalMenu.h"

enum {
    DEBUG_NAV_CLEARANCE = 60,
};

static app_ui_vertical_menu_t s_debug_menu;

static const app_ui_vertical_menu_item_t s_debug_items[] = {
    {.id = 1u, .label = "Debug Item 1"},
    {.id = 2u, .label = "Debug Item 2"},
    {.id = 3u, .label = "Debug Item 3"},
    {.id = 4u, .label = "Debug Item 4"},
    {.id = 5u, .label = "Debug Item 5"},
    {.id = 6u, .label = "Debug Item 6"},
    {.id = 7u, .label = "Debug Item 7"},
    {.id = 8u, .label = "Debug Item 8"},
};

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *nav_clearance;
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    (void)App_UiVerticalMenu_Create(
        parent,
        &s_debug_menu,
        s_debug_items,
        sizeof(s_debug_items) / sizeof(s_debug_items[0]),
        NULL);

    nav_clearance = lv_obj_create(parent);
    lv_obj_remove_style_all(nav_clearance);
    lv_obj_set_size(nav_clearance, LV_PCT(100), DEBUG_NAV_CLEARANCE);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageDebug_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_DEBUG,
        .title = "Debug",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
