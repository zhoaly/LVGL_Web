/**
 * @file App_UiComponents.h
 * @brief UI 可复用组件头文件 —— 定义 Action 绑定结构和组件构建函数。
 *
 * 【设计说明】
 * 此文件提供 UI 层通用的组件构建 API，各页面通过调用这些函数
 * 统一创建界面元素，确保视觉风格一致。
 *
 * 组件列表：
 *   - NavBar：导航栏（返回/首页按钮）
 *   - StatusCard：状态概览卡片（可点击跳转详情页）
 *   - InfoRow：键值信息行
 */

#ifndef APP_UI_COMPONENTS_H
#define APP_UI_COMPONENTS_H

#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "app_action.h"

/**
 * @brief UI 动作绑定结构体
 *
 * 将 LVGL 控件的点击事件与 Action 系统的请求关联起来。
 * 当控件被点击时，绑定中存储的 request 会被提交到 Action 系统。
 */
typedef struct {
    app_action_request_t request;   /**< 点击时要提交的 Action 请求 */
} app_ui_action_binding_t;

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
 * @brief 将 Action 绑定到 LVGL 控件上
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
 * 包含可选的返回按钮和始终显示的首页按钮。
 * 按钮的 Action 绑定由调用者提供的 bindings 数组管理。
 *
 * @param parent 父容器
 * @param can_back 是否显示返回按钮
 * @param bindings 长度为 2 的绑定数组（[0]=返回, [1]=首页）
 * @return 导航栏容器对象
 */
lv_obj_t *App_UiComponent_CreateNavBar(lv_obj_t *parent,
                                      bool can_back,
                                      app_ui_action_binding_t bindings[2]);

/**
 * @brief 创建状态概览卡片
 *
 * 用于首页展示各业务域的概要信息。
 * 如果提供 binding，卡片为可点击按钮（可跳转到详情页）；
 * 否则为只读容器。
 *
 * @param parent 父容器
 * @param title 卡片标题
 * @param binding 可选的动作绑定（点击跳转）
 * @return 数值标签对象指针（用于后续更新数值）
 */
lv_obj_t *App_UiComponent_CreateStatusCard(lv_obj_t *parent,
                                          const char *title,
                                          app_ui_action_binding_t *binding);

/**
 * @brief 创建键值信息行
 *
 * 用于详情页展示键值对信息。
 * 布局为标签左对齐、数值右对齐的 flex row。
 *
 * @param parent 父容器
 * @param label 左侧标签文本
 * @return 右侧数值标签对象指针（用于后续更新数值）
 */
lv_obj_t *App_UiComponent_CreateInfoRow(lv_obj_t *parent, const char *label);

#endif /* APP_UI_COMPONENTS_H */
