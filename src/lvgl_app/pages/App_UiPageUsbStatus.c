#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

static lv_obj_t *s_values[3];

static void refresh(const app_ui_model_t *model)
{
    static const app_ui_value_key_t keys[] = {
        APP_UI_VALUE_USB_MODE,
        APP_UI_VALUE_USB_HUB_ENABLED,
        APP_UI_VALUE_USB_DEVICE_COUNT,
    };
    char buffer[48];
    uint32_t i;
    for(i = 0; i < 3u; i++) {
        lv_label_set_text(s_values[i], App_UiModel_FormatValue(model, keys[i], buffer, sizeof(buffer)));
    }
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    s_values[0] = App_UiComponent_CreateInfoRow(parent, "Current Mode");
    s_values[1] = App_UiComponent_CreateInfoRow(parent, "HUB Enable");
    s_values[2] = App_UiComponent_CreateInfoRow(parent, "Device Count");
    refresh(model);
}

const app_ui_page_t *App_UiPageUsbStatus_Get(void)
{
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
