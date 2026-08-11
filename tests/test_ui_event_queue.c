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
static uint32_t s_refreshed_mask;
static unsigned int s_refresh_count;
static unsigned int s_show_count;
static app_ui_page_id_t s_shown_page;
static app_ui_page_transition_t s_shown_transition;
static app_action_ui_dispatcher_fn s_dispatcher;
static void *s_dispatcher_user_data;

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
    s_dispatcher = dispatcher;
    s_dispatcher_user_data = user_data;
    return ESP_OK;
}

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id)
{
    static const app_ui_page_t home_page = {
        .id = APP_UI_PAGE_HOME,
        .title = "",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = false,
        .build = NULL,
        .refresh = NULL,
    };
    static const app_ui_page_t text_page = {
        .id = APP_UI_PAGE_TEXT,
        .title = "Text",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = true,
        .build = NULL,
        .refresh = NULL,
    };

    if(page_id == APP_UI_PAGE_HOME) {
        return &home_page;
    }
    return page_id == APP_UI_PAGE_TEXT ? &text_page : NULL;
}

void App_UiView_Init(app_ui_view_t *view)
{
    memset(view, 0, sizeof(*view));
}

void App_UiView_ShowPage(
    app_ui_view_t *view,
    const app_ui_page_t *page,
    const app_ui_model_t *model,
    bool can_back,
    app_ui_page_transition_t transition)
{
    (void)view;
    (void)model;
    (void)can_back;
    s_show_count++;
    s_shown_page = page->id;
    s_shown_transition = transition;
}

void App_UiView_Refresh(
    app_ui_view_t *view,
    const app_ui_model_t *model,
    uint32_t dirty_mask)
{
    (void)view;
    s_refreshed_model = *model;
    s_refreshed_mask = dirty_mask;
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
    app_action_request_t request;
    unsigned int show_count_before;
    unsigned int index;

    assert(App_UiInit());
    assert(App_UiStart());
    assert(s_timer.callback != NULL);
    assert(s_dispatcher != NULL);
    assert(s_show_count == 1u);
    assert(s_shown_page == APP_UI_PAGE_HOME);
    assert(s_shown_transition == APP_UI_PAGE_TRANSITION_INITIAL);

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
    assert((s_refreshed_mask & APP_UI_DIRTY_STATUS) != 0u);

    memset(&request, 0, sizeof(request));
    request.id = APP_ACTION_ID_UI_NAV_PUSH;
    request.params.ui_navigation.page_id = APP_UI_PAGE_TEXT;
    assert(s_dispatcher(&request, s_dispatcher_user_data) == ESP_OK);
    s_timer.callback(&s_timer);
    assert(s_shown_page == APP_UI_PAGE_TEXT);
    assert(s_shown_transition == APP_UI_PAGE_TRANSITION_PUSH);

    /* 选择当前菜单页只关闭抽屉，不重复 Push 或增加历史栈。 */
    show_count_before = s_show_count;
    assert(s_dispatcher(&request, s_dispatcher_user_data) == ESP_OK);
    s_timer.callback(&s_timer);
    assert(s_show_count == show_count_before);

    /* 子页面不声明 STATUS dirty，全部状态事件仍刷新屏幕级状态栏。 */
    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_TIME_UPDATED;
    event.data.time.hour = 10u;
    event.data.time.minute = 30u;
    event.data.time.synced = true;
    assert(App_UiPostEvent(&event));

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_WEATHER_UPDATED;
    event.data.weather.temperature_c = 28;
    event.data.weather.available = true;
    assert(App_UiPostEvent(&event));

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_WIFI_STATE_CHANGED;
    event.data.wifi = APP_UI_WIFI_CONNECTED;
    assert(App_UiPostEvent(&event));

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_BLUETOOTH_STATE_CHANGED;
    event.data.bluetooth = APP_UI_BLUETOOTH_CONNECTED;
    assert(App_UiPostEvent(&event));

    s_timer.callback(&s_timer);
    assert(s_refreshed_model.status.time.hour == 10u);
    assert(s_refreshed_model.status.time.minute == 30u);
    assert(s_refreshed_model.status.weather.temperature_c == 28);
    assert(s_refreshed_model.status.weather.available);
    assert(s_refreshed_model.status.wifi == APP_UI_WIFI_CONNECTED);
    assert(s_refreshed_model.status.bluetooth == APP_UI_BLUETOOTH_CONNECTED);
    assert((s_refreshed_mask & APP_UI_DIRTY_STATUS) != 0u);

    request.id = APP_ACTION_ID_UI_NAV_BACK;
    assert(s_dispatcher(&request, s_dispatcher_user_data) == ESP_OK);
    s_timer.callback(&s_timer);
    assert(s_shown_page == APP_UI_PAGE_HOME);
    assert(s_shown_transition == APP_UI_PAGE_TRANSITION_BACK);

    request.id = APP_ACTION_ID_UI_NAV_PUSH;
    request.params.ui_navigation.page_id = APP_UI_PAGE_TEXT;
    assert(s_dispatcher(&request, s_dispatcher_user_data) == ESP_OK);
    s_timer.callback(&s_timer);
    request.id = APP_ACTION_ID_UI_NAV_HOME;
    assert(s_dispatcher(&request, s_dispatcher_user_data) == ESP_OK);
    s_timer.callback(&s_timer);
    assert(s_shown_page == APP_UI_PAGE_HOME);
    assert(s_shown_transition == APP_UI_PAGE_TRANSITION_HOME);
    return 0;
}
