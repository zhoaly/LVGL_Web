#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

static lv_obj_t *s_values[5];

static void refresh(const app_ui_model_t *model)
{
    static const app_ui_value_key_t keys[] = {
        APP_UI_VALUE_UPTIME,
        APP_UI_VALUE_FREE_HEAP,
        APP_UI_VALUE_MIN_HEAP,
        APP_UI_VALUE_TASK_COUNT,
        APP_UI_VALUE_RESET_REASON,
    };
    char buffer[48];
    uint32_t i;
    for(i = 0; i < 5u; i++) {
        lv_label_set_text(s_values[i], App_UiModel_FormatValue(model, keys[i], buffer, sizeof(buffer)));
    }
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    s_values[0] = App_UiComponent_CreateInfoRow(parent, "Uptime");
    s_values[1] = App_UiComponent_CreateInfoRow(parent, "Free Heap");
    s_values[2] = App_UiComponent_CreateInfoRow(parent, "Min Heap");
    s_values[3] = App_UiComponent_CreateInfoRow(parent, "Tasks");
    s_values[4] = App_UiComponent_CreateInfoRow(parent, "Reset Reason");
    refresh(model);
}

const app_ui_page_t *App_UiPageRuntime_Get(void)
{
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
