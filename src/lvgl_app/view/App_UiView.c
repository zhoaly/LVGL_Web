/**
 * @file App_UiView.c
 * @brief 视图层（View）实现 —— 管理 LVGL 对象树和页面切换。
 *
 * 【架构位置】
 * MVC 架构中的 View 层，负责：
 *   1. 创建主界面的 LVGL 对象树（状态栏、内容区、Toast 栏）
 *   2. 根据页面描述符切换页面内容
 *   3. 触发页面的刷新回调
 *   4. 显示 Toast 消息
 *
 * 【布局结构】
 *   screen_root (flex column)
 *   ├── status_bar（屏幕级顶部状态栏）
 *   ├── content（内容区，flex grow=1）
 *   │   └── 页面自定义内容（由 page->build 创建）
 *   ├── toast_label（底部消息栏，灰色 14px）
 *   └── NavBar（底部导航栏，仅非首页显示）
 */

#include "App_UiView.h"

#include <stdio.h>
#include <string.h>

#include "../assets/App_UiTheme.h"
#include "../components/motion/App_UiMotion.h"
#include "../port/App_UiPort.h"

enum {
    NAV_BAR_ENTER_CLEARANCE = 16,
};

static const app_ui_menu_drawer_item_t s_menu_items[] = {
    {
        .icon_id = APP_UI_ICON_MENU_NETWORK,
        .label = "Network",
        .target_page = APP_UI_PAGE_NETWORK,
    },
    {
        .icon_id = APP_UI_ICON_MENU_HID_HUB,
        .label = "HID Hub",
        .target_page = APP_UI_PAGE_HID_HUB,
    },
    {
        .icon_id = APP_UI_ICON_MENU_SETTINGS,
        .label = "Settings",
        .target_page = APP_UI_PAGE_SETTINGS,
    },
    {
        .icon_id = APP_UI_ICON_MENU_DEBUG,
        .label = "Debug",
        .target_page = APP_UI_PAGE_DEBUG,
    },
};

static void menu_closed_cb(void *user_data);

static app_ui_status_wifi_state_t map_wifi_state(app_ui_wifi_state_t state)
{
    switch(state) {
    case APP_UI_WIFI_CONNECTING:
        return APP_UI_STATUS_WIFI_CONNECTING;
    case APP_UI_WIFI_CONNECTED:
        return APP_UI_STATUS_WIFI_CONNECTED;
    case APP_UI_WIFI_DISCONNECTED:
    default:
        return APP_UI_STATUS_WIFI_DISCONNECTED;
    }
}

static app_ui_status_bluetooth_state_t map_bluetooth_state(
    app_ui_bluetooth_state_t state)
{
    switch(state) {
    case APP_UI_BLUETOOTH_ADVERTISING:
        return APP_UI_STATUS_BLUETOOTH_ADVERTISING;
    case APP_UI_BLUETOOTH_CONNECTED:
        return APP_UI_STATUS_BLUETOOTH_CONNECTED;
    case APP_UI_BLUETOOTH_OFF:
    default:
        return APP_UI_STATUS_BLUETOOTH_OFF;
    }
}

static void make_status_bar_state(
    const app_ui_model_t *model,
    app_ui_status_bar_state_t *state,
    char *time_text,
    size_t time_text_size,
    char *weather_text,
    size_t weather_text_size)
{
    snprintf(time_text, time_text_size, "%s", "--:--");
    snprintf(weather_text, weather_text_size, "%s", "--C");

    state->time_text = time_text;
    state->time_synced = false;
    state->weather_text = weather_text;
    state->weather_available = false;
    state->wifi_state = APP_UI_STATUS_WIFI_DISCONNECTED;
    state->bluetooth_state = APP_UI_STATUS_BLUETOOTH_OFF;

    if(model == NULL) {
        return;
    }

    state->time_synced = model->status.time.synced;
    if(model->status.time.synced) {
        snprintf(time_text, time_text_size, "%02u:%02u",
                 (unsigned int)model->status.time.hour,
                 (unsigned int)model->status.time.minute);
    }

    state->weather_available = model->status.weather.available;
    if(model->status.weather.available) {
        snprintf(weather_text, weather_text_size, "%dC",
                 (int)model->status.weather.temperature_c);
    }

    state->wifi_state = map_wifi_state(model->status.wifi);
    state->bluetooth_state = map_bluetooth_state(model->status.bluetooth);
}

static void update_status_bar(
    app_ui_view_t *view,
    const app_ui_model_t *model)
{
    app_ui_status_bar_state_t state;
    char time_text[6];
    char weather_text[8];

    make_status_bar_state(model, &state,
                          time_text, sizeof(time_text),
                          weather_text, sizeof(weather_text));
    App_UiStatusBar_Update(&view->status_bar, &state);
}

bool App_UiView_OpenMenu(app_ui_view_t *view)
{
    lv_obj_t *drawer_root;
    bool was_open;

    if(view == NULL) {
        return false;
    }
    if(view->menu_group == NULL) {
        view->menu_group = lv_group_create();
    }
    was_open = App_UiMenuDrawer_IsOpen(&view->menu_drawer);
    if(view->menu_group == NULL) {
        return false;
    }

    lv_group_set_default(view->menu_group);
    drawer_root = App_UiMenuDrawer_Open(
        &view->menu_drawer,
        s_menu_items,
        sizeof(s_menu_items) / sizeof(s_menu_items[0]));
    lv_group_set_default(view->input_group);
    if(drawer_root != NULL) {
        App_UiMenuDrawer_SetClosedCallback(
            &view->menu_drawer, menu_closed_cb, view);
        (void)App_UiPort_SetInputGroup(view->menu_group);
        if(!was_open && lv_group_get_obj_count(view->menu_group) > 0u) {
            lv_obj_t *first = lv_group_get_obj_by_index(view->menu_group, 0u);
            if(first != NULL) {
                lv_group_focus_obj(first);
            }
        }
        return true;
    }
    return false;
}

static void menu_open_cb(void *user_data)
{
    (void)App_UiView_OpenMenu((app_ui_view_t *)user_data);
}

bool App_UiView_CloseMenu(app_ui_view_t *view)
{
    if(view == NULL) {
        return false;
    }
    App_UiMenuDrawer_Close(&view->menu_drawer);
    return true;
}

static void menu_closed_cb(void *user_data)
{
    app_ui_view_t *view = user_data;

    if(view != NULL && view->input_group != NULL) {
        (void)App_UiPort_SetInputGroup(view->input_group);
    }
}

static void screen_root_delete_cb(lv_event_t *event)
{
    app_ui_view_t *view = lv_event_get_user_data(event);

    if(view != NULL) {
        App_UiMenuDrawer_SetClosedCallback(&view->menu_drawer, NULL, NULL);
        App_UiMenuDrawer_Destroy(&view->menu_drawer);
    }
}

static void prepare_nav_bar_enter(app_ui_view_t *view)
{
    int32_t nav_height;

    view->nav_enter_pending = false;
    view->nav_enter_offset = 0;
    if(view->nav_bar == NULL ||
       App_UiMotion_GetLevel() != APP_UI_MOTION_LEVEL_NORMAL) {
        return;
    }

    lv_obj_update_layout(view->screen_root);
    nav_height = lv_obj_get_height(view->nav_bar);
    if(nav_height <= 0) {
        return;
    }

    view->nav_enter_offset = nav_height + NAV_BAR_ENTER_CLEARANCE;
    view->nav_enter_pending = true;
    lv_obj_set_style_translate_y(
        view->nav_bar, view->nav_enter_offset, 0);
}

static void start_nav_bar_enter(app_ui_view_t *view)
{
    int32_t offset = view->nav_enter_offset;

    view->nav_enter_pending = false;
    view->nav_enter_offset = 0;
    if(view->nav_bar == NULL) {
        return;
    }
    if(App_UiMotion_GetLevel() != APP_UI_MOTION_LEVEL_NORMAL ||
       offset <= 0) {
        lv_obj_set_style_translate_y(view->nav_bar, 0, 0);
        return;
    }

    App_UiMotion_AnimateEnter(
        view->nav_bar,
        APP_UI_MOTION_OPACITY_NONE,
        offset,
        0);
}

static void complete_page_transition(app_ui_view_t *view)
{
    app_ui_page_transition_t transition = view->pending_transition;

    if(view->outgoing_page_host != NULL) {
        App_UiMotion_StopObject(view->outgoing_page_host);
        lv_obj_delete(view->outgoing_page_host);
        view->outgoing_page_host = NULL;
    }
    if(view->active_page_host != NULL) {
        App_UiMotion_StopObject(view->active_page_host);
        lv_obj_set_x(view->active_page_host, 0);
    }

    view->transitioning = false;
    view->pending_transition = APP_UI_PAGE_TRANSITION_INITIAL;

    if(view->nav_enter_pending) {
        start_nav_bar_enter(view);
    }

    if(view->active_page != NULL &&
       view->active_page->enter != NULL &&
       transition != APP_UI_PAGE_TRANSITION_INITIAL) {
        view->active_page->enter(transition);
    }
}

static void page_transition_completed_cb(lv_anim_t *animation)
{
    app_ui_view_t *view = lv_anim_get_user_data(animation);

    if(view == NULL) {
        return;
    }

    /*
     * 当前回调属于 outgoing_page_host 的动画。不要在完成回调中再次
     * lv_anim_delete() 当前动画，直接删除宿主后再统一收尾目标页面。
     */
    if(view->outgoing_page_host != NULL) {
        lv_obj_delete(view->outgoing_page_host);
        view->outgoing_page_host = NULL;
    }
    complete_page_transition(view);
}

static void settle_page_transition(app_ui_view_t *view)
{
    if(view->transitioning || view->outgoing_page_host != NULL) {
        complete_page_transition(view);
        return;
    }

    if(view->active_page_host != NULL) {
        App_UiMotion_StopObject(view->active_page_host);
        lv_obj_set_x(view->active_page_host, 0);
    }
}

static lv_obj_t *create_page_host(lv_obj_t *parent)
{
    lv_obj_t *host = lv_obj_create(parent);

    lv_obj_remove_style_all(host);
    lv_obj_remove_flag(host, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(host, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(host, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(host, 8, 0);
    return host;
}

bool App_UiView_Init(app_ui_view_t *view)
{
    app_ui_status_bar_callbacks_t status_callbacks;
    lv_obj_t *screen;

    if(view == NULL) {
        return false;
    }

    /* 清零视图上下文 */
    memset(view, 0, sizeof(*view));

    view->input_group = lv_group_get_default();
    if(view->input_group == NULL) {
        view->input_group = lv_group_create();
    }
    if(view->input_group == NULL) {
        return false;
    }
    lv_group_set_default(view->input_group);
    if(!App_UiPort_SetInputGroup(view->input_group)) {
        return false;
    }

    /* 获取当前活动屏幕并设置背景色 */
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(
        screen,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SCREEN_BACKGROUND),
        0);

    /* 创建根容器：全屏 flex column 布局 */
    view->screen_root = lv_obj_create(screen);
    lv_obj_remove_style_all(view->screen_root);
    lv_obj_set_size(view->screen_root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(view->screen_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(view->screen_root, 12, 0);
    lv_obj_set_style_pad_row(view->screen_root, 8, 0);
    lv_obj_add_event_cb(view->screen_root,
                        screen_root_delete_cb,
                        LV_EVENT_DELETE,
                        view);

    /* 创建屏幕级顶部状态栏；页面标题暂时仅保留元数据，不参与布局。 */
    memset(&status_callbacks, 0, sizeof(status_callbacks));
    status_callbacks.on_menu = menu_open_cb;
    status_callbacks.user_data = view;
    (void)App_UiStatusBar_Create(view->screen_root,
                                 &view->status_bar,
                                 NULL,
                                 &status_callbacks);

    /* 创建内容区：填充剩余空间 */
    view->content = lv_obj_create(view->screen_root);
    lv_obj_remove_style_all(view->content);
    lv_obj_set_width(view->content, LV_PCT(100));
    lv_obj_set_flex_grow(view->content, 1);
    lv_obj_remove_flag(view->content, LV_OBJ_FLAG_SCROLLABLE);

    /* 创建 Toast 消息标签（初始为空） */
    view->toast_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(
        view->toast_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        0);
    lv_obj_set_style_text_color(
        view->toast_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
        0);
    lv_label_set_text(view->toast_label, "");
    return true;
}

void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back,
                         app_ui_page_transition_t transition)
{
    lv_obj_t *old_host;
    lv_obj_t *new_host;
    bool use_directional_motion;
    int32_t enter_x;
    int32_t exit_x;
    int32_t page_width;
    bool nav_was_visible;
    uint32_t focus_start_index;

    if(view == NULL || page == NULL || model == NULL) {
        return;
    }

    settle_page_transition(view);
    old_host = view->active_page_host;
    new_host = create_page_host(view->content);
    view->active_page_host = new_host;

    /* 更新活动页面指针；屏幕级状态栏保持在页面宿主之外。 */
    view->active_page = page;
    view->pending_transition = transition;
    update_status_bar(view, model);

    /* 删除上一页的导航栏，避免页面切换后残留 */
    nav_was_visible = view->nav_bar != NULL;
    if(view->nav_bar != NULL) {
        App_UiMotion_StopObject(view->nav_bar);
        lv_obj_delete(view->nav_bar);
        view->nav_bar = NULL;
    }
    view->nav_enter_pending = false;
    view->nav_enter_offset = 0;

    focus_start_index = view->input_group != NULL
                            ? lv_group_get_obj_count(view->input_group)
                            : 0u;

    /* 调用页面构建回调创建自定义内容 */
    if(page->build != NULL) {
        page->build(new_host, model);
    }

    /* 非首页时创建屏幕级浮动导航栏，使其固定在屏幕底部。 */
    if(page->id != APP_UI_PAGE_HOME) {
        view->nav_bar = App_UiComponent_CreateNavBar(view->screen_root,
                                                     can_back && page->show_back,
                                                     view->nav_bindings);
        if(!nav_was_visible &&
           transition != APP_UI_PAGE_TRANSITION_INITIAL) {
            prepare_nav_bar_enter(view);
        }
    }

    if(view->input_group != NULL &&
       lv_group_get_obj_count(view->input_group) > focus_start_index) {
        lv_obj_t *first = lv_group_get_obj_by_index(
            view->input_group, focus_start_index);
        if(first != NULL) {
            lv_group_focus_obj(first);
        }
    }

    use_directional_motion =
        old_host != NULL &&
        App_UiMotion_GetLevel() == APP_UI_MOTION_LEVEL_NORMAL &&
        (transition == APP_UI_PAGE_TRANSITION_PUSH ||
         transition == APP_UI_PAGE_TRANSITION_BACK);

    if(use_directional_motion) {
        lv_obj_update_layout(view->content);
        page_width = lv_obj_get_width(view->content);
        use_directional_motion = page_width > 0;
    }

    if(use_directional_motion) {
        enter_x = transition == APP_UI_PAGE_TRANSITION_PUSH
                      ? page_width
                      : -page_width;
        exit_x = -enter_x;
        lv_obj_set_x(new_host, enter_x);
        view->outgoing_page_host = old_host;
        view->transitioning = true;
        (void)App_UiMotion_AnimateX(
            new_host,
            enter_x,
            0,
            APP_UI_MOTION_DURATION_PAGE,
            NULL,
            NULL);
        (void)App_UiMotion_AnimateX(old_host,
                                   0,
                                   exit_x,
                                   APP_UI_MOTION_DURATION_PAGE,
                                   page_transition_completed_cb,
                                   view);
    } else {
        if(old_host != NULL) {
            lv_obj_delete(old_host);
        }
        complete_page_transition(view);
    }
}

void App_UiView_Refresh(app_ui_view_t *view,
                        const app_ui_model_t *model,
                        uint32_t dirty_mask)
{
    if(view == NULL || model == NULL) {
        return;
    }

    if((dirty_mask & APP_UI_DIRTY_STATUS) != 0u) {
        update_status_bar(view, model);
    }

    if(view->active_page != NULL &&
       view->active_page->refresh != NULL &&
       (dirty_mask & view->active_page->dirty_mask) != 0u) {
        view->active_page->refresh(model);
    }
}

void App_UiView_ShowToast(app_ui_view_t *view, const char *message)
{
    /* 在底部标签显示消息（空消息则清空） */
    if(view != NULL && view->toast_label != NULL) {
        App_UiMotion_StopObject(view->toast_label);
        lv_obj_set_style_translate_y(view->toast_label, 0, 0);
        lv_obj_set_style_text_opa(view->toast_label, LV_OPA_COVER, 0);
        if(message == NULL || message[0] == '\0') {
            lv_label_set_text(view->toast_label, "");
            return;
        }

        lv_label_set_text(view->toast_label, message);
        App_UiMotion_AnimateEnter(view->toast_label,
                                 APP_UI_MOTION_OPACITY_TEXT,
                                 4,
                                 0);
    }
}
