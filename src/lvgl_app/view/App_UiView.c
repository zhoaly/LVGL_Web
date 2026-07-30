/**
 * @file App_UiView.c
 * @brief 视图层（View）实现 —— 管理 LVGL 对象树和页面切换。
 *
 * 【架构位置】
 * MVC 架构中的 View 层，负责：
 *   1. 创建主界面的 LVGL 对象树（标题栏、内容区、Toast 栏）
 *   2. 根据页面描述符切换页面内容
 *   3. 触发页面的刷新回调
 *   4. 显示 Toast 消息
 *
 * 【布局结构】
 *   screen_root (flex column)
 *   ├── title_label（标题栏，蓝色 16px）
 *   ├── content（内容区，flex grow=1）
 *   │   └── 页面自定义内容（由 page->build 创建）
 *   ├── toast_label（底部消息栏，灰色 14px）
 *   └── NavBar（底部导航栏，仅非首页显示）
 */

#include "App_UiView.h"

#include <string.h>

#include "../assets/App_UiTheme.h"
#include "../components/motion/App_UiMotion.h"

enum {
    PAGE_ENTER_OFFSET = 16,
    PAGE_EXIT_OFFSET = 8,
};

static void page_transition_completed_cb(lv_anim_t *animation)
{
    app_ui_view_t *view = lv_anim_get_user_data(animation);

    if(view == NULL) {
        return;
    }
    if(view->outgoing_page_host != NULL) {
        lv_obj_delete(view->outgoing_page_host);
        view->outgoing_page_host = NULL;
    }
    view->transitioning = false;
}

static void settle_page_transition(app_ui_view_t *view)
{
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

void App_UiView_Init(app_ui_view_t *view)
{
    lv_obj_t *screen;

    if(view == NULL) {
        return;
    }

    /* 清零视图上下文 */
    memset(view, 0, sizeof(*view));

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

    /* 创建标题标签 */
    view->title_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(
        view->title_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_TITLE),
        0);
    lv_obj_set_style_text_color(
        view->title_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT),
        0);

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

    if(view == NULL || page == NULL || model == NULL) {
        return;
    }

    settle_page_transition(view);
    old_host = view->active_page_host;
    new_host = create_page_host(view->content);
    view->active_page_host = new_host;

    /* 更新活动页面指针和标题 */
    view->active_page = page;
    if(page->title != NULL && page->title[0] != '\0') {
        lv_obj_remove_flag(view->title_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(view->title_label, page->title);
    } else {
        lv_label_set_text(view->title_label, "");
        lv_obj_add_flag(view->title_label, LV_OBJ_FLAG_HIDDEN);
    }

    /* 删除上一页的导航栏，避免页面切换后残留 */
    if(view->nav_bar != NULL) {
        lv_obj_delete(view->nav_bar);
        view->nav_bar = NULL;
    }

    /* 调用页面构建回调创建自定义内容 */
    if(page->build != NULL) {
        page->build(new_host, model);
    }

    /* 非首页时在屏幕根容器末尾创建导航栏，使其固定在内容区下方 */
    if(page->id != APP_UI_PAGE_HOME) {
        view->nav_bar = App_UiComponent_CreateNavBar(view->screen_root,
                                                     can_back && page->show_back,
                                                     view->nav_bindings);
    }

    use_directional_motion =
        old_host != NULL &&
        App_UiMotion_GetLevel() == APP_UI_MOTION_LEVEL_NORMAL &&
        (transition == APP_UI_PAGE_TRANSITION_PUSH ||
         transition == APP_UI_PAGE_TRANSITION_BACK);

    if(use_directional_motion) {
        enter_x = transition == APP_UI_PAGE_TRANSITION_PUSH
                      ? PAGE_ENTER_OFFSET
                      : -PAGE_ENTER_OFFSET;
        exit_x = transition == APP_UI_PAGE_TRANSITION_PUSH
                     ? -PAGE_EXIT_OFFSET
                     : PAGE_EXIT_OFFSET;
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
    } else if(old_host != NULL) {
        lv_obj_delete(old_host);
    }

    if(page->enter != NULL && transition != APP_UI_PAGE_TRANSITION_INITIAL) {
        page->enter(transition);
    }
}

void App_UiView_Refresh(app_ui_view_t *view, const app_ui_model_t *model)
{
    /* 调用活动页面的 refresh 回调更新数据展示 */
    if(view != NULL && view->active_page != NULL && view->active_page->refresh != NULL) {
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
