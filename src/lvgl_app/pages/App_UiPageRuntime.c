/**
 * @file App_UiPageRuntime.c
 * @brief 运行时信息页（Runtime Page）实现 —— 显示系统运行状态详情。
 *
 * 【显示内容】
 *   - Uptime：系统已运行时间（秒）
 *   - Free Heap：当前空闲堆内存（KB）
 *   - Min Heap：历史最小空闲堆内存（KB）
 *   - Tasks：系统当前任务数
 *   - Reset Reason：上次重启原因
 *
 * 【布局】
 * 使用 flex column 布局，每行显示一个键值对（标签: 值）。
 */

#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

/** 运行时信息页的数值标签指针数组（保存所有需要动态更新的标签） */
static lv_obj_t *s_values[5];

/**
 * @brief 运行时信息页刷新回调
 *
 * 从 Model 中读取运行时间、堆内存、任务数和重启原因，
 * 更新到对应的信息行数值标签上。
 *
 * @param model 最新的数据模型
 */
static void refresh(const app_ui_model_t *model)
{
    /* 要显示的数据 Key 列表，与 s_values 数组一一对应 */
    static const app_ui_value_key_t keys[] = {
        APP_UI_VALUE_UPTIME,
        APP_UI_VALUE_FREE_HEAP,
        APP_UI_VALUE_MIN_HEAP,
        APP_UI_VALUE_TASK_COUNT,
        APP_UI_VALUE_RESET_REASON,
    };
    char buffer[48];
    uint32_t i;

    /* 遍历所有信息行，更新数值 */
    for(i = 0; i < 5u; i++) {
        lv_label_set_text(s_values[i], App_UiModel_FormatValue(model, keys[i], buffer, sizeof(buffer)));
    }
}

/**
 * @brief 运行时信息页构建回调
 *
 * 创建 5 行键值信息：
 *   Uptime | Free Heap | Min Heap | Tasks | Reset Reason
 *
 * @param parent 父容器
 * @param model 数据模型
 */
static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    /* 创建信息行并保存数值标签指针 */
    s_values[0] = App_UiComponent_CreateInfoRow(parent, "Uptime");
    s_values[1] = App_UiComponent_CreateInfoRow(parent, "Free Heap");
    s_values[2] = App_UiComponent_CreateInfoRow(parent, "Min Heap");
    s_values[3] = App_UiComponent_CreateInfoRow(parent, "Tasks");
    s_values[4] = App_UiComponent_CreateInfoRow(parent, "Reset Reason");

    /* 构建完成后立即刷新数据 */
    refresh(model);
}

const app_ui_page_t *App_UiPageRuntime_Get(void)
{
    /* 静态常量页面描述符 */
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_RUNTIME,
        .title = "Runtime",
        .dirty_mask = APP_UI_DIRTY_RUNTIME,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
