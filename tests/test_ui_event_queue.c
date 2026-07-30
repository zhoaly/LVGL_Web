#include <assert.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include "lvgl_app/action/app_action.h"
#include "lvgl_app/app/App_Ui.h"
#include "lvgl_app/model/App_UiModel.h"
#include "lvgl_app/pages/registry/App_UiPages.h"
#include "lvgl_app/port/App_UiPort.h"
#include "lvgl_app/view/App_UiView.h"

struct _lv_timer_t {
    lv_timer_cb_t callback;
};

static struct _lv_timer_t s_timer;
static app_ui_model_t s_refreshed_model;
static unsigned int s_refresh_count;

lv_timer_t *lv_timer_create(
    lv_timer_cb_t callback,
    uint32_t period,
    void *user_data)
{
    (void)period;
    (void)user_data;
    s_timer.callback = callback;
    return &s_timer;
}

bool App_UiPort_Init(void)
{
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    (void)full_refresh;
}

void App_UiPort_EnterCritical(void)
{
}

void App_UiPort_ExitCritical(void)
{
}

esp_err_t app_action_init(void)
{
    return ESP_OK;
}

esp_err_t app_action_register_ui_dispatcher(
    app_action_ui_dispatcher_fn dispatcher,
    void *user_data)
{
    (void)dispatcher;
    (void)user_data;
    return ESP_OK;
}

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id)
{
    static const app_ui_page_t home_page = {
        .id = APP_UI_PAGE_HOME,
        .title = "",
        .dirty_mask = APP_UI_DIRTY_STATUS,
        .show_back = false,
        .build = NULL,
        .refresh = NULL,
    };

    return page_id == APP_UI_PAGE_HOME ? &home_page : NULL;
}

void App_UiView_Init(app_ui_view_t *view)
{
    memset(view, 0, sizeof(*view));
}

void App_UiView_ShowPage(
    app_ui_view_t *view,
    const app_ui_page_t *page,
    const app_ui_model_t *model,
    bool can_back)
{
    (void)view;
    (void)page;
    (void)model;
    (void)can_back;
}

void App_UiView_Refresh(
    app_ui_view_t *view,
    const app_ui_model_t *model)
{
    (void)view;
    s_refreshed_model = *model;
    s_refresh_count++;
}

void App_UiView_ShowToast(app_ui_view_t *view, const char *message)
{
    (void)view;
    (void)message;
}

int main(void)
{
    app_ui_event_t event;
    unsigned int index;

    assert(App_UiInit());
    assert(App_UiStart());
    assert(s_timer.callback != NULL);

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_TIME_UPDATED;
    event.data.time.hour = 9u;
    event.data.time.synced = true;

    for(index = 0u; index < 15u; index++) {
        event.data.time.minute = (uint8_t)index;
        assert(App_UiPostEvent(&event));
    }

    event.data.time.minute = 59u;
    assert(!App_UiPostEvent(&event));

    s_timer.callback(&s_timer);
    s_timer.callback(&s_timer);

    assert(s_refresh_count == 2u);
    assert(s_refreshed_model.status.time.hour == 9u);
    assert(s_refreshed_model.status.time.minute == 14u);
    assert(s_refreshed_model.status.time.synced);
    return 0;
}
