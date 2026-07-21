/**
 * @file App_UiView.h
 * @brief 视图层（View）头文件 —— 定义视图上下文和 UI 布局接口。
 *
 * 【布局结构】
 *   screen_root (flex column, 全屏)
 *   ├── title_label（顶部标题栏，蓝色 16px）
 *   ├── content（中间内容区，flex grow=1）
 *   │   ├── NavBar（导航栏，非首页时显示）
 *   │   └── 页面自定义控件
 *   └── toast_label（底部消息栏，灰色 14px）
 */

#ifndef APP_UI_VIEW_H
#define APP_UI_VIEW_H

#include "lvgl/lvgl.h"

#include "../components/App_UiComponents.h"
#include "../pages/registry/App_UiPages.h"

/**
 * @brief 视图上下文结构体
 */
typedef struct {
    lv_obj_t *screen_root;              /**< 根容器 */
    lv_obj_t *title_label;              /**< 标题栏标签 */
    lv_obj_t *content;                  /**< 内容区容器 */
    lv_obj_t *toast_label;              /**< 底部 Toast 标签 */
    const app_ui_page_t *active_page;   /**< 当前活动页面 */
    app_ui_action_binding_t nav_bindings[2]; /**< 导航栏绑定（[0]=返回, [1]=首页） */
} app_ui_view_t;

/**
 * @brief 初始化视图层
 * @param view 视图上下文指针
 */
void App_UiView_Init(app_ui_view_t *view);

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
                         bool can_back);

/**
 * @brief 刷新当前页面的数据展示
 * @param view 视图上下文
 * @param model 最新数据模型
 */
void App_UiView_Refresh(app_ui_view_t *view, const app_ui_model_t *model);

/**
 * @brief 显示 Toast 消息
 * @param view 视图上下文
 * @param message 消息文本（NULL 或空字符串清除 Toast）
 */
void App_UiView_ShowToast(app_ui_view_t *view, const char *message);

#endif /* APP_UI_VIEW_H */
