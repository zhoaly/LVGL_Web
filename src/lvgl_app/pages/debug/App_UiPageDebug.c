/**
 * @file App_UiPageDebug.c
 * @brief Debug page with a reusable vertical menu.
 */

#include "App_UiPageDebug.h"

#include "lvgl/lvgl.h"

#include "../../action/app_action.h"
#include "../../components/widgets/vertical_menu/App_UiVerticalMenu.h"

enum {
    DEBUG_MENU_BOTTOM_SCROLL_SPACE = 60,
    DEBUG_ITEM_CONTROLS_GALLERY = 1,
};

static app_ui_vertical_menu_t s_debug_menu;

static const app_ui_vertical_menu_item_t s_debug_items[] = {
    {.id = DEBUG_ITEM_CONTROLS_GALLERY, .label = "Controls Gallery"},
    {.id = 2u, .label = "Debug Item 2"},
    {.id = 3u, .label = "Debug Item 3"},
    {.id = 4u, .label = "Debug Item 4"},
    {.id = 5u, .label = "Debug Item 5"},
    {.id = 6u, .label = "Debug Item 6"},
    {.id = 7u, .label = "Debug Item 7"},
    {.id = 8u, .label = "Debug Item 8"},
};

static void item_activated(uint32_t item_id, void *user_data)
{
    app_action_job_id_t job_id;
    app_action_request_t request = {0};
    (void)user_data;

    if(item_id != DEBUG_ITEM_CONTROLS_GALLERY) {
        return;
    }

    request.id = APP_ACTION_ID_UI_NAV_PUSH;
    request.params.ui_navigation.page_id = APP_UI_PAGE_CONTROLS_GALLERY;
    (void)app_action_submit(&request, &job_id);
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    static const app_ui_vertical_menu_callbacks_t callbacks = {
        .on_item_activated = item_activated,
        .user_data = NULL,
    };
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    if(App_UiVerticalMenu_Create(
        parent,
        &s_debug_menu,
        s_debug_items,
        sizeof(s_debug_items) / sizeof(s_debug_items[0]),
        &callbacks) != NULL) {
        (void)App_UiVerticalMenu_SetBottomSpace(
            &s_debug_menu, DEBUG_MENU_BOTTOM_SCROLL_SPACE);
    }
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
