/**
 * @file App_UiView.h
 * @brief 视图层（View）头文件 —— 定义视图上下文和 UI 布局接口。
 *
 * 【布局结构】
 *   screen_root (flex column, 全屏)
 *   ├── title_label（顶部标题栏，蓝色 16px 字体）
 *   ├── content（中间内容区，flex grow，填充剩余空间）
 *   │   ├── NavBar（导航栏：包含返回/首页按钮）
 *   │   └── 页面自定义控件（由各页面的 build 回调创建）
 *   └── toast_label（底部消息栏，灰色 14px 字体）
 */

#ifndef APP_UI_VIEW_H
#define APP_UI_VIEW_H

#include "lvgl/lvgl.h"

#include "components/App_UiComponents.h"
#include "pages/App_UiPages.h"

/**
 * @brief 视图上下文结构体
 *
 * 保存整个 UI 界面的 LVGL 对象树根节点和当前活动页面信息。
 * nav_bindings 用于导航栏按钮的 Action 绑定。
 */
typedef struct {
    lv_obj_t *screen_root;              /**< 根容器：占满屏幕的 flex column 容器 */
    lv_obj_t *title_label;              /**< 标题栏标签：显示当前页面标题 */
    lv_obj_t *content;                  /**< 内容区容器：放置导航栏和页面控件 */
    lv_obj_t *toast_label;              /**< 底部 Toast 消息标签 */
    const app_ui_page_t *active_page;   /**< 当前活动页面描述符指针 */
    app_ui_action_binding_t nav_bindings[2]; /**< 导航栏按钮的 Action 绑定（[0]=返回, [1]=首页） */
} app_ui_view_t;

/**
 * @brief 初始化视图层，创建主界面布局
 * @param view 视图上下文指针
 */
void App_UiView_Init(app_ui_view_t *view);

/**
 * @brief 显示指定页面
 *
 * 执行流程：
 *   1. 设置标题栏文本
 *   2. 清空内容区
 *   3. 创建导航栏（根据 can_back 和 page->show_back 决定是否显示返回按钮）
 *   4. 调用 page->build() 创建页面自定义内容
 *
 * @param view 视图上下文
 * @param page 要显示的页面描述符
 * @param model 数据模型（用于构建页面内容）
 * @param can_back 是否允许显示返回按钮
 */
void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back);

/**
 * @brief 刷新当前页面的数据展示
 *
 * 调用活动页面的 refresh 回调更新所有数据绑定标签的值。
 *
 * @param view 视图上下文
 * @param model 最新的数据模型
 */
void App_UiView_Refresh(app_ui_view_t *view, const app_ui_model_t *model);

/**
 * @brief 在底部显示一条 Toast 消息
 * @param view 视图上下文
 * @param message 消息文本（传 NULL 或空字符串清除 Toast）
 */
void App_UiView_ShowToast(app_ui_view_t *view, const char *message);

#endif /* APP_UI_VIEW_H */
