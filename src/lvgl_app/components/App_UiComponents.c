/**
 * @file App_UiComponents.c
 * @brief UI 可复用组件实现 —— Action 绑定和导航栏构建。
 *
 * 【组件列表】
 *   - Action 绑定：将 LVGL 点击事件关联到 Action 系统
 *   - NavBar：导航栏（返回/首页按钮），flex row 左右分散对齐布局
 */

#include "App_UiComponents.h"

#include <string.h>

/**
 * @brief LVGL 点击事件回调：提交绑定的 Action 请求
 *
 * 当用户点击控件时，从事件用户数据中获取 binding，
 * 然后通过 app_action_submit() 提交请求。
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

    /* 初始化绑定结构体：清零后设置动作 ID 和目标页面 */
    memset(binding, 0, sizeof(*binding));
    binding->request.id = action_id;
    binding->request.params.ui_navigation.page_id = page_id;
}

void App_UiComponent_BindAction(lv_obj_t *object, app_ui_action_binding_t *binding)
{
    if(object == NULL || binding == NULL) {
        return;
    }

    /* 使控件可点击，注册点击事件回调 */
    lv_obj_add_flag(object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(object, action_click_cb, LV_EVENT_CLICKED, binding);
}

lv_obj_t *App_UiComponent_CreateNavBar(lv_obj_t *parent,
                                      bool can_back,
                                      app_ui_action_binding_t bindings[2])
{
    lv_obj_t *row;
    lv_obj_t *home;
    lv_obj_t *home_label;

    if(parent == NULL || bindings == NULL) {
        return NULL;
    }

    /* 创建导航栏容器行（flex row，左右分散对齐） */
    row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* 初始化返回和首页按钮的 Action 绑定 */
    App_UiComponent_InitAction(&bindings[0], APP_ACTION_ID_UI_NAV_BACK, 0u);
    App_UiComponent_InitAction(&bindings[1], APP_ACTION_ID_UI_NAV_HOME, 0u);

    /* 根据 can_back 决定是否显示返回按钮 */
    if(can_back) {
        lv_obj_t *back = lv_button_create(row);
        lv_obj_t *back_label = lv_label_create(back);
        lv_label_set_text(back_label, "< Back");
        App_UiComponent_BindAction(back, &bindings[0]);
    } else {
        /* 不可返回时放置占位符以保持右对齐 */
        lv_obj_t *spacer = lv_obj_create(row);
        lv_obj_remove_style_all(spacer);
        lv_obj_set_size(spacer, 1, 1);
    }

    /* 首页按钮（始终显示） */
    home = lv_button_create(row);
    home_label = lv_label_create(home);
    lv_label_set_text(home_label, "Home");
    App_UiComponent_BindAction(home, &bindings[1]);
    return row;
}
