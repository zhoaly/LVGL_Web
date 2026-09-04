/**
 * @file App_UiView.h
 * @brief 视图层（View）头文件 —— 定义视图上下文和 UI 布局接口。
 *
 * 【布局结构】
 *   screen_root (flex column, 全屏)
 *   ├── status_bar（屏幕级顶部状态栏）
 *   ├── content（中间内容区，flex grow=1）
 *   │   └── 页面自定义控件
 *   ├── toast_label（底部消息栏，灰色 14px）
 *   └── NavBar（底部导航栏，非首页时显示）
 */

#ifndef APP_UI_VIEW_H
#define APP_UI_VIEW_H

#include "lvgl.h"

#include "../components/App_UiComponents.h"
#include "../components/widgets/menu_drawer/App_UiMenuDrawer.h"
#include "../components/widgets/status_bar/App_UiStatusBar.h"
#include "../pages/registry/App_UiPages.h"

/**
 * @brief 视图上下文结构体
 */
typedef struct {
    lv_obj_t *screen_root;              /**< 根容器 */
    app_ui_status_bar_t status_bar;     /**< 屏幕级顶部状态栏 */
    app_ui_menu_drawer_t menu_drawer;   /**< 屏幕级菜单抽屉 */
    lv_obj_t *content;                  /**< 内容区容器 */
    lv_timer_t *toast_timer;
    lv_obj_t *toast_label;              /**< 底部 Toast 标签 */
    lv_obj_t *nav_bar;                  /**< 底部导航栏，首页时为 NULL */
    const app_ui_page_t *active_page;   /**< 当前活动页面 */
    app_ui_action_binding_t nav_bindings[2]; /**< 导航栏绑定（[0]=返回, [1]=首页） */
    lv_group_t *input_group;             /**< 页面与常驻控件的编码器焦点组 */
    lv_group_t *menu_group;              /**< 菜单打开时独占的焦点组 */
    lv_obj_t *active_page_host;
    lv_obj_t *outgoing_page_host;
    bool transitioning;
    app_ui_page_transition_t pending_transition;
    bool nav_enter_pending;
    int32_t nav_enter_offset;
} app_ui_view_t;

/**
 * @brief 初始化视图层
 * @param view 视图上下文指针
 */
bool App_UiView_Init(app_ui_view_t *view);

/** 打开屏幕级菜单抽屉；调用必须发生在 LVGL 线程/事件泵上下文。 */
bool App_UiView_OpenMenu(app_ui_view_t *view);

/** 关闭屏幕级菜单抽屉；调用必须发生在 LVGL 线程/事件泵上下文。 */
bool App_UiView_CloseMenu(app_ui_view_t *view);

/**
 * @brief 显示指定页面
 * @param view 视图上下文
 * @param page 页面描述符
 * @param model 数据模型
 * @param can_back 是否允许返回
 */
void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back,
                         app_ui_page_transition_t transition);

/**
 * @brief 按 dirty mask 刷新全局状态栏和当前页面的数据展示
 * @param view 视图上下文
 * @param model 最新数据模型
 * @param dirty_mask 本次需要处理的 dirty 标志
 */
void App_UiView_Refresh(app_ui_view_t *view,
                        const app_ui_model_t *model,
                        uint32_t dirty_mask);

/**
 * @brief 显示 Toast 消息
 * @param view 视图上下文
 * @param message 消息文本（NULL 或空字符串清除 Toast）
 */
void App_UiView_ShowToast(app_ui_view_t *view, const char *message);

#endif /* APP_UI_VIEW_H */
