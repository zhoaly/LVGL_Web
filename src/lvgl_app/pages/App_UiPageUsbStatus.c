/**
 * @file App_UiPageUsbStatus.c
 * @brief USB 状态详情页（USB Status Page）实现 —— 显示 USB 相关信息。
 *
 * 【显示内容】
 *   - Current Mode：当前 USB 模式（OFF/PC/ESP）
 *   - HUB Enable：USB HUB 电源状态（ON/OFF）
 *   - Device Count：下游连接的 USB 设备数量
 *
 * 【布局】
 * 使用 flex column 布局，每行显示一个键值对。
 */

#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

/** USB 状态页的数值标签指针数组 */
static lv_obj_t *s_values[3];

/**
 * @brief USB 状态页刷新回调
 *
 * 从 Model 中读取 USB 模式、HUB 使能状态和设备数量，
 * 更新到对应的信息行数值标签上。
 *
 * @param model 最新的数据模型
 */
static void refresh(const app_ui_model_t *model)
{
    /* 要显示的数据 Key 列表 */
    static const app_ui_value_key_t keys[] = {
        APP_UI_VALUE_USB_MODE,
        APP_UI_VALUE_USB_HUB_ENABLED,
        APP_UI_VALUE_USB_DEVICE_COUNT,
    };
    char buffer[48];
    uint32_t i;

    /* 遍历所有信息行，更新数值 */
    for(i = 0; i < 3u; i++) {
        lv_label_set_text(s_values[i], App_UiModel_FormatValue(model, keys[i], buffer, sizeof(buffer)));
    }
}

/**
 * @brief USB 状态页构建回调
 *
 * 创建 3 行键值信息：
 *   Current Mode | HUB Enable | Device Count
 *
 * @param parent 父容器
 * @param model 数据模型
 */
static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    /* 创建信息行并保存数值标签指针 */
    s_values[0] = App_UiComponent_CreateInfoRow(parent, "Current Mode");
    s_values[1] = App_UiComponent_CreateInfoRow(parent, "HUB Enable");
    s_values[2] = App_UiComponent_CreateInfoRow(parent, "Device Count");

    /* 构建完成后立即刷新数据 */
    refresh(model);
}

const app_ui_page_t *App_UiPageUsbStatus_Get(void)
{
    /* 静态常量页面描述符 */
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_USB_STATUS,
        .title = "USB Status",
        .dirty_mask = APP_UI_DIRTY_USB,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
