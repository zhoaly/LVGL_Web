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

#include "../assets/App_UiAssets.h"
#include "../assets/App_UiTheme.h"
#include "motion/App_UiMotion.h"

void App_UiComponent_ApplyFocusRing(
    lv_obj_t *object,
    app_ui_component_focus_style_t style)
{
    if(object == NULL) {
        return;
    }

    /* 内收底座、细边框与柔和阴影替代外扩的蓝色轮廓。 */
    lv_obj_set_style_outline_width(object, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(object, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(object, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(object, 8, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(
        object,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_SHADOW),
        LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(object, LV_OPA_20, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_offset_y(object, 2, LV_STATE_FOCUSED);

    if(style == APP_UI_COMPONENT_FOCUS_DARK) {
        lv_obj_set_style_bg_color(
            object,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_PRESSED),
            LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(object, LV_OPA_COVER, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(
            object,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_FOREGROUND),
            LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(object, LV_OPA_40, LV_STATE_FOCUSED);
    } else {
        lv_obj_set_style_bg_color(
            object,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
            LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(object, LV_OPA_COVER, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(
            object,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_FOCUS_BORDER),
            LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(object, LV_OPA_40, LV_STATE_FOCUSED);
    }

}

void App_UiComponent_ApplyFocusStyle(
    lv_obj_t *object,
    app_ui_component_focus_style_t style)
{
    if(object == NULL) {
        return;
    }

    App_UiComponent_ApplyFocusRing(object, style);
    App_UiMotion_ApplyButton(object);
}

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

/**
 * @brief 图标按钮按压反馈：白底状态下将图标切换为黑色
 */
static void nav_icon_feedback_cb(lv_event_t *event)
{
    lv_obj_t *icon = lv_event_get_user_data(event);
    lv_event_code_t code = lv_event_get_code(event);

    if(icon == NULL) {
        return;
    }

    if(code == LV_EVENT_PRESSED) {
        lv_obj_set_style_image_recolor(
            icon,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_PRESSED_FOREGROUND),
            0);
    } else {
        lv_obj_set_style_image_recolor(
            icon,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
            0);
    }
}

/**
 * @brief 创建黑白风格的纯图标导航按钮
 */
static lv_obj_t *create_nav_icon_button(lv_obj_t *parent,
                                        const lv_image_dsc_t *source,
                                        app_ui_action_binding_t *binding)
{
    lv_obj_t *button;
    lv_obj_t *icon;

    if(parent == NULL || source == NULL || binding == NULL) {
        return NULL;
    }

    button = lv_button_create(parent);
    lv_obj_remove_style_all(button);
    lv_obj_set_size(button, 48, 36);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_bg_color(
        button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_color(
        button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_border_opa(button, LV_OPA_40, 0);
    App_UiComponent_ApplyFocusStyle(
        button, APP_UI_COMPONENT_FOCUS_DARK);

    icon = lv_image_create(button);
    lv_image_set_src(icon, source);
    lv_obj_set_style_image_recolor(
        icon,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_image_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_center(icon);

    lv_obj_add_event_cb(button, nav_icon_feedback_cb, LV_EVENT_PRESSED, icon);
    lv_obj_add_event_cb(button, nav_icon_feedback_cb, LV_EVENT_RELEASED, icon);
    lv_obj_add_event_cb(button, nav_icon_feedback_cb, LV_EVENT_PRESS_LOST, icon);
    App_UiComponent_BindAction(button, binding);
    return button;
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

    if(parent == NULL || bindings == NULL) {
        return NULL;
    }

    /* 黑色圆角悬浮 Dock，按钮以黑白图标呈现 */
    row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    /*
     * 导航栏是屏幕级浮动 Dock，不参与 screen_root 的 Flex 布局，
     * 避免显示或执行 translate 动画时压缩页面内容区域。
     */
    lv_obj_add_flag(row, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(
        row,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_BACKGROUND),
        0);
    lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(row, 18, 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_border_color(
        row,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_border_opa(row, LV_OPA_20, 0);
    lv_obj_set_style_pad_hor(row, 10, 0);
    lv_obj_set_style_pad_ver(row, 8, 0);
    lv_obj_set_style_shadow_width(row, 12, 0);
    lv_obj_set_style_shadow_color(
        row,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_SHADOW),
        0);
    lv_obj_set_style_shadow_opa(row, LV_OPA_20, 0);
    lv_obj_set_style_shadow_offset_y(row, 3, 0);

    /* 初始化返回和首页按钮的 Action 绑定 */
    App_UiComponent_InitAction(&bindings[0], APP_ACTION_ID_UI_NAV_BACK, 0u);
    App_UiComponent_InitAction(&bindings[1], APP_ACTION_ID_UI_NAV_HOME, 0u);

    /* 根据 can_back 决定是否显示返回图标 */
    if(can_back) {
        (void)create_nav_icon_button(
            row,
            App_UiAssets_GetIcon(APP_UI_ICON_NAV_BACK),
            &bindings[0]);
    } else {
        /* 使用与按钮等宽的占位符，保持 Home 图标固定在右侧 */
        lv_obj_t *spacer = lv_obj_create(row);
        lv_obj_remove_style_all(spacer);
        lv_obj_set_size(spacer, 48, 36);
    }

    /* Home 图标始终显示 */
    (void)create_nav_icon_button(
        row,
        App_UiAssets_GetIcon(APP_UI_ICON_NAV_HOME),
        &bindings[1]);
    lv_obj_update_layout(row);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, 0);
    return row;
}
