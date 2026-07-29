/**
 * @file App_UiPageHome.c
 * @brief 首页（Home Page）实现 —— 显示 "Ready" 欢迎界面。
 *
 * 【布局】
 * 使用 flex column 居中对齐，在屏幕中央显示 "Ready" 标签。
 * 首页没有返回按钮，不可导航返回。
 */

#include "App_UiPageHome.h"

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"
#include "../../components/App_UiComponents.h"

static app_ui_action_binding_t s_text_page_binding;

/**
 * @brief 首页构建回调：创建一个居中的 "Ready" 标签
 * @param parent 父容器
 * @param model 数据模型（未使用）
 */
static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *open_button;
    lv_obj_t *open_button_label;
    lv_obj_t *ready_label;
    (void)model;

    /* 设置 flex 列居中布局 */
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* 创建 "Ready" 标签 */
    ready_label = lv_label_create(parent);
    lv_label_set_text(ready_label, "Ready");
    lv_obj_set_style_text_font(
        ready_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);
    lv_obj_set_style_text_color(
        ready_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
        0);

    /* 创建跳转到纯文字页面的按钮 */
    open_button = lv_button_create(parent);
    open_button_label = lv_label_create(open_button);
    lv_label_set_text(open_button_label, "Open Page");
    lv_obj_center(open_button_label);

    App_UiComponent_InitAction(&s_text_page_binding,
                               APP_ACTION_ID_UI_NAV_PUSH,
                               APP_UI_PAGE_TEXT);
    App_UiComponent_BindAction(open_button, &s_text_page_binding);
}

/**
 * @brief 首页刷新回调（当前为空操作）
 * @param model 数据模型（未使用）
 */
static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
    /* 静态常量页面描述符 */
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "LVGL App",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = false,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
