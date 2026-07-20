/**
 * @file App_UiPageHome.c
 * @brief 首页（Home Page）实现 —— 显示 USB/WiFi/Runtime 状态概览卡片。
 *
 * 【布局】
 * 使用 flex row wrap 布局，三张状态卡片在一行内排列：
 *   ┌──────────┐  ┌──────────┐  ┌──────────┐
 *   │ USB HUB  │  │   WiFi   │  │ Runtime  │
 *   │   ESP    │  │ Connected│  │  312 KB  │
 *   └──────────┘  └──────────┘  └──────────┘
 *
 * 【交互】
 *   - USB HUB 和 Runtime 卡片可点击，跳转到对应的详情页
 *   - WiFi 卡片为只读状态（当前仅为概览展示）
 */

#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

/**
 * @brief 首页上下文结构体（静态全局，保存所有需要动态更新的 LVGL 对象指针）
 */
typedef struct {
    lv_obj_t *usb_value;                    /**< USB 状态数值标签 */
    lv_obj_t *wifi_value;                   /**< WiFi 状态数值标签 */
    lv_obj_t *runtime_value;                /**< Runtime 数值标签 */
    app_ui_action_binding_t usb_action;     /**< USB 卡片点击动作绑定（跳转到 USB 详情页） */
    app_ui_action_binding_t runtime_action; /**< Runtime 卡片点击动作绑定（跳转到 Runtime 详情页） */
} home_page_ctx_t;

/** 首页全局上下文实例 */
static home_page_ctx_t s_home;

/**
 * @brief 辅助函数：从 Model 中取值并设置到标签上
 * @param label LVGL 标签对象
 * @param model 数据模型
 * @param key 要显示的数据 Key
 */
static void set_value(lv_obj_t *label, const app_ui_model_t *model, app_ui_value_key_t key)
{
    char buffer[48];
    if(label != NULL) {
        lv_label_set_text(label, App_UiModel_FormatValue(model, key, buffer, sizeof(buffer)));
    }
}

/**
 * @brief 首页构建回调
 *
 * 创建三张状态卡片：
 *   - USB HUB：显示当前 USB 模式，点击跳转到 USB 状态详情页
 *   - WiFi：显示 WiFi 连接状态（只读，暂不可点击）
 *   - Runtime：显示空闲堆内存，点击跳转到运行时信息页
 *
 * @param parent 父容器（View 层的内容区）
 * @param model 数据模型
 */
static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    /* 设置 flex 行布局，允许换行 */
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    /* 初始化卡片点击动作 */
    App_UiComponent_InitAction(&s_home.usb_action, APP_ACTION_ID_UI_NAV_PUSH, APP_UI_PAGE_USB_STATUS);
    App_UiComponent_InitAction(&s_home.runtime_action, APP_ACTION_ID_UI_NAV_PUSH, APP_UI_PAGE_RUNTIME);

    /* 创建三张状态卡片 */
    s_home.usb_value = App_UiComponent_CreateStatusCard(parent, "USB HUB", &s_home.usb_action);
    s_home.wifi_value = App_UiComponent_CreateStatusCard(parent, "WiFi", NULL);
    s_home.runtime_value = App_UiComponent_CreateStatusCard(parent, "Runtime", &s_home.runtime_action);

    /* 构建完成后立即刷新数据 */
    App_UiPageHome_Get()->refresh(model);
}

/**
 * @brief 首页刷新回调
 *
 * 从 Model 中读取最新的 USB 模式、WiFi 状态和空闲堆内存，
 * 更新到对应的卡片数值标签上。
 *
 * @param model 最新的数据模型
 */
static void refresh(const app_ui_model_t *model)
{
    set_value(s_home.usb_value, model, APP_UI_VALUE_USB_MODE);
    set_value(s_home.wifi_value, model, APP_UI_VALUE_WIFI_STATE);
    set_value(s_home.runtime_value, model, APP_UI_VALUE_FREE_HEAP);
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
    /* 静态常量页面描述符，所有属性在编译时确定 */
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "ESP32HUB",
        .dirty_mask = APP_UI_DIRTY_USB | APP_UI_DIRTY_WIFI | APP_UI_DIRTY_RUNTIME,
        .show_back = false,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
