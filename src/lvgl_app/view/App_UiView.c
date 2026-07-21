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
 *   │   ├── NavBar（导航栏，仅非首页显示）
 *   │   └── 页面自定义内容（由 page->build 创建）
 *   └── toast_label（底部消息栏，灰色 14px）
 */

#include "App_UiView.h"

#include <string.h>

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
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFAFBFD), 0);

    /* 创建根容器：全屏 flex column 布局 */
    view->screen_root = lv_obj_create(screen);
    lv_obj_remove_style_all(view->screen_root);
    lv_obj_set_size(view->screen_root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(view->screen_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(view->screen_root, 12, 0);
    lv_obj_set_style_pad_row(view->screen_root, 8, 0);

    /* 创建标题标签 */
    view->title_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(view->title_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(view->title_label, lv_color_hex(0x2778FF), 0);

    /* 创建内容区：填充剩余空间 */
    view->content = lv_obj_create(view->screen_root);
    lv_obj_remove_style_all(view->content);
    lv_obj_set_width(view->content, LV_PCT(100));
    lv_obj_set_flex_grow(view->content, 1);
    lv_obj_set_flex_flow(view->content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(view->content, 8, 0);

    /* 创建 Toast 消息标签（初始为空） */
    view->toast_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(view->toast_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(view->toast_label, lv_color_hex(0x718096), 0);
    lv_label_set_text(view->toast_label, "");
}

void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back)
{
    if(view == NULL || page == NULL || model == NULL) {
        return;
    }

    /* 更新活动页面指针和标题 */
    view->active_page = page;
    lv_label_set_text(view->title_label, page->title != NULL ? page->title : "");

    /* 清空内容区并重建 */
    lv_obj_clean(view->content);
    lv_obj_set_flex_flow(view->content, LV_FLEX_FLOW_COLUMN);

    /* 非首页时显示导航栏（返回/首页按钮） */
    if(page->id != APP_UI_PAGE_HOME) {
        (void)App_UiComponent_CreateNavBar(view->content,
                                          can_back && page->show_back,
                                          view->nav_bindings);
    }

    /* 调用页面构建回调创建自定义内容 */
    if(page->build != NULL) {
        page->build(view->content, model);
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
        lv_label_set_text(view->toast_label, message != NULL ? message : "");
    }
}
