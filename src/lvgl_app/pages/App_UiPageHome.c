#include "App_UiPages.h"

#include "../components/App_UiComponents.h"

typedef struct {
    lv_obj_t *usb_value;
    lv_obj_t *wifi_value;
    lv_obj_t *runtime_value;
    app_ui_action_binding_t usb_action;
    app_ui_action_binding_t runtime_action;
} home_page_ctx_t;

static home_page_ctx_t s_home;

static void set_value(lv_obj_t *label, const app_ui_model_t *model, app_ui_value_key_t key)
{
    char buffer[48];
    if(label != NULL) {
        lv_label_set_text(label, App_UiModel_FormatValue(model, key, buffer, sizeof(buffer)));
    }
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    App_UiComponent_InitAction(&s_home.usb_action, APP_ACTION_ID_UI_NAV_PUSH, APP_UI_PAGE_USB_STATUS);
    App_UiComponent_InitAction(&s_home.runtime_action, APP_ACTION_ID_UI_NAV_PUSH, APP_UI_PAGE_RUNTIME);
    s_home.usb_value = App_UiComponent_CreateStatusCard(parent, "USB HUB", &s_home.usb_action);
    s_home.wifi_value = App_UiComponent_CreateStatusCard(parent, "WiFi", NULL);
    s_home.runtime_value = App_UiComponent_CreateStatusCard(parent, "Runtime", &s_home.runtime_action);
    App_UiPageHome_Get()->refresh(model);
}

static void refresh(const app_ui_model_t *model)
{
    set_value(s_home.usb_value, model, APP_UI_VALUE_USB_MODE);
    set_value(s_home.wifi_value, model, APP_UI_VALUE_WIFI_STATE);
    set_value(s_home.runtime_value, model, APP_UI_VALUE_FREE_HEAP);
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
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
