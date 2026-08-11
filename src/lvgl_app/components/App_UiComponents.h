/**
 * @file App_UiComponents.h
 * @brief UI 可复用组件头文件 —— 定义 Action 绑定结构和导航栏创建函数。
 *
 * 【Action 绑定机制】
 * app_ui_action_binding_t 将 LVGL 控件的点击事件与 Action 系统关联。
 * 当用户点击绑定了 Action 的控件时，binding 中存储的 request 被提交到 Action 系统。
 */

#ifndef APP_UI_COMPONENTS_H
#define APP_UI_COMPONENTS_H

#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "../action/app_action.h"

/**
 * @brief UI 动作绑定结构体
 *
 * 将 LVGL 控件的点击事件与 Action 请求关联。
 */
typedef struct {
    app_action_request_t request;  /**< 点击时要提交的 Action 请求 */
} app_ui_action_binding_t;

typedef enum {
    APP_UI_COMPONENT_FOCUS_LIGHT = 0,
    APP_UI_COMPONENT_FOCUS_DARK,
} app_ui_component_focus_style_t;

/**
 * @brief Apply the shared focus ring without button press motion.
 *
 * Use this for interactive controls such as sliders whose motion is supplied
 * by a control-specific App_UiMotion helper.
 */
void App_UiComponent_ApplyFocusRing(
    lv_obj_t *object,
    app_ui_component_focus_style_t style);

/**
 * @brief 为按钮应用统一、非蓝色的编码器焦点反馈。
 *
 * LIGHT 用于浅色页面上的图标按钮；DARK 用于黑色导航和菜单控件。
 */
void App_UiComponent_ApplyFocusStyle(
    lv_obj_t *object,
    app_ui_component_focus_style_t style);

/**
 * @brief 初始化 Action 绑定
 * @param binding 绑定结构体指针
 * @param action_id 动作 ID
 * @param page_id 目标页面 ID（用于导航动作）
 */
void App_UiComponent_InitAction(app_ui_action_binding_t *binding,
                                app_action_id_t action_id,
                                uint32_t page_id);

/**
 * @brief 将 Action 绑定到 LVGL 控件
 *
 * 使控件可点击，并在点击时提交绑定的 Action 请求。
 *
 * @param object LVGL 控件指针
 * @param binding Action 绑定结构体指针
 */
void App_UiComponent_BindAction(lv_obj_t *object, app_ui_action_binding_t *binding);

/**
 * @brief 创建导航栏组件
 *
 * 黑色圆角 Dock，包含可选的返回图标和始终显示的首页图标。
 *
 * @param parent 父容器
 * @param can_back 是否显示返回按钮
 * @param bindings 长度为 2 的绑定数组（[0]=返回, [1]=首页）
 * @return 导航栏容器对象，失败返回 NULL
 */
lv_obj_t *App_UiComponent_CreateNavBar(lv_obj_t *parent,
                                      bool can_back,
                                      app_ui_action_binding_t bindings[2]);

#endif /* APP_UI_COMPONENTS_H */
