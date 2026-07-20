/**
 * @file App_UiComponents.c
 * @brief UI 可复用组件实现 —— 导航栏、状态卡片、信息行、Action 绑定。
 *
 * 【设计说明】
 * 此文件提供一组可复用的 LVGL 组件构建函数，
 * 各页面通过这些组件快速搭建 UI，避免重复代码。
 *
 * 【组件列表】
 *   - NavBar：顶部导航栏（返回/首页按钮）
 *   - StatusCard：状态概览卡片（带点击跳转）
 *   - InfoRow：信息行（标签: 值）
 *   - Action 绑定：将 LVGL 点击事件关联到 Action 系统
 */

#include "App_UiComponents.h"

#include <string.h>

/**
 * @brief LVGL 点击事件回调：提交 Action 请求
 *
 * 当用户点击绑定了 Action 的控件时，
 * 从事件用户数据中获取 Action 绑定信息并提交。
 *
 * @param event LVGL 事件指针
 */
static void action_click_cb(lv_event_t *event)
{
    app_ui_action_binding_t *binding = lv_event_get_user_data(event);
    app_action_job_id_t job_id;
    if(binding != NULL) {
        (void)app_action_submit(&binding->request, &job_id);
    }
}

void App_UiComponent_InitAction(app_ui_action_binding_t *binding,
                                app_action_id_t action_id,
                                uint32_t page_id)
{
    if(binding == NULL) {
        return;
    }
    /* 初始化 Action 绑定结构体 */
    memset(binding, 0, sizeof(*binding));
    binding->request.id = action_id;
    binding->request.params.ui_navigation.page_id = page_id;
}

void App_UiComponent_BindAction(lv_obj_t *object, app_ui_action_binding_t *binding)
{
    if(object == NULL || binding == NULL) {
        return;
    }
    /* 使控件可点击，并注册点击事件回调 */
    lv_obj_add_flag(object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(object, action_click_cb, LV_EVENT_CLICKED, binding);
}

lv_obj_t *App_UiComponent_CreateNavBar(lv_obj_t *parent,
                                      bool can_back,
                                      app_ui_action_binding_t bindings[2])
{
    /* 创建导航栏容器行（flex row，左右分散对齐） */
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* 初始化返回按钮和首页按钮的 Action 绑定 */
    App_UiComponent_InitAction(&bindings[0], APP_ACTION_ID_UI_NAV_BACK, 0u);
    App_UiComponent_InitAction(&bindings[1], APP_ACTION_ID_UI_NAV_HOME, 0u);

    /* 根据 can_back 决定是否显示返回按钮 */
    if(can_back) {
        lv_obj_t *button = lv_button_create(row);
        lv_obj_t *label = lv_label_create(button);
        lv_label_set_text(label, "< Back");
        App_UiComponent_BindAction(button, &bindings[0]);
    } else {
        /* 不可返回时放置占位符以保持布局 */
        lv_obj_t *spacer = lv_obj_create(row);
        lv_obj_remove_style_all(spacer);
        lv_obj_set_size(spacer, 1, 1);
    }

    /* 首页按钮（始终显示） */
    lv_obj_t *home = lv_button_create(row);
    lv_obj_t *home_label = lv_label_create(home);
    lv_label_set_text(home_label, "Home");
    App_UiComponent_BindAction(home, &bindings[1]);
    return row;
}

lv_obj_t *App_UiComponent_CreateStatusCard(lv_obj_t *parent,
                                          const char *title,
                                          app_ui_action_binding_t *binding)
{
    /*
     * 创建状态卡片：
     *   - 如果提供了 binding，使用按钮（可点击跳转）
     *   - 否则使用普通容器（只读显示）
     */
    lv_obj_t *card = binding != NULL ? lv_button_create(parent) : lv_obj_create(parent);
    lv_obj_set_size(card, 130, 72);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* 标题标签 */
    lv_obj_t *title_label = lv_label_create(card);
    lv_label_set_text(title_label, title != NULL ? title : "");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);

    /* 数值标签（初始显示 "-"） */
    lv_obj_t *value_label = lv_label_create(card);
    lv_label_set_text(value_label, "-");
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_14, 0);

    /* 绑定点击动作（如果有） */
    if(binding != NULL) {
        App_UiComponent_BindAction(card, binding);
    }
    return value_label;
}

lv_obj_t *App_UiComponent_CreateInfoRow(lv_obj_t *parent, const char *label)
{
    /*
     * 创建信息行（flex row，左右分散对齐）：
     *   - 左侧：标签文本（如 "Uptime"）
     *   - 右侧：数值文本（初始 "-"，由页面 refresh 更新）
     */
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(row, 4, 0);

    /* 左侧标签 */
    lv_obj_t *left = lv_label_create(row);
    lv_label_set_text(left, label != NULL ? label : "");

    /* 右侧数值标签 */
    lv_obj_t *right = lv_label_create(row);
    lv_label_set_text(right, "-");
    return right;
}
