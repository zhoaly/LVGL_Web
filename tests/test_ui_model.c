#include <assert.h>
#include <string.h>

#include "App_UiModel.h"

static void clear_event(app_ui_event_t *event, app_ui_event_type_t type)
{
    memset(event, 0, sizeof(*event));
    event->type = type;
}

int main(void)
{
    app_ui_model_t model;
    app_ui_status_state_t previous_status;
    app_ui_event_t event;

    App_UiModel_Init(&model);
    assert(model.current_page == APP_UI_PAGE_HOME);
    assert(model.dirty_mask == APP_UI_DIRTY_ALL);
    assert(model.message[0] == '\0');
    assert(model.status.time.hour == 0u);
    assert(model.status.time.minute == 0u);
    assert(!model.status.time.synced);
    assert(model.status.weather.temperature_c == 0);
    assert(!model.status.weather.available);
    assert(model.status.wifi == APP_UI_WIFI_DISCONNECTED);
    assert(model.status.bluetooth == APP_UI_BLUETOOTH_OFF);

    clear_event(&event, APP_UI_EVENT_SHOW_MESSAGE);
    strcpy(event.text, "Ready");
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(strcmp(model.message, "Ready") == 0);
    assert((model.dirty_mask & APP_UI_DIRTY_SYSTEM) != 0u);

    clear_event(&event, APP_UI_EVENT_TIME_UPDATED);
    event.data.time.hour = 9u;
    event.data.time.minute = 41u;
    event.data.time.synced = true;
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(model.status.time.hour == 9u);
    assert(model.status.time.minute == 41u);
    assert(model.status.time.synced);
    assert(model.dirty_mask == APP_UI_DIRTY_STATUS);

    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(model.dirty_mask == APP_UI_DIRTY_NONE);

    clear_event(&event, APP_UI_EVENT_WEATHER_UPDATED);
    event.data.weather.temperature_c = -12;
    event.data.weather.available = true;
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(model.status.weather.temperature_c == -12);
    assert(model.status.weather.available);
    assert(model.dirty_mask == APP_UI_DIRTY_STATUS);

    clear_event(&event, APP_UI_EVENT_WIFI_STATE_CHANGED);
    event.data.wifi = APP_UI_WIFI_CONNECTING;
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(model.status.wifi == APP_UI_WIFI_CONNECTING);
    assert(model.dirty_mask == APP_UI_DIRTY_STATUS);

    clear_event(&event, APP_UI_EVENT_BLUETOOTH_STATE_CHANGED);
    event.data.bluetooth = APP_UI_BLUETOOTH_ADVERTISING;
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(model.status.bluetooth == APP_UI_BLUETOOTH_ADVERTISING);
    assert(model.dirty_mask == APP_UI_DIRTY_STATUS);

    previous_status = model.status;
    clear_event(&event, APP_UI_EVENT_TIME_UPDATED);
    event.data.time.hour = 24u;
    event.data.time.minute = 0u;
    event.data.time.synced = true;
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(memcmp(&model.status, &previous_status, sizeof(previous_status)) == 0);
    assert(model.dirty_mask == APP_UI_DIRTY_NONE);

    clear_event(&event, APP_UI_EVENT_WEATHER_UPDATED);
    event.data.weather.temperature_c = 100;
    event.data.weather.available = true;
    App_UiModel_ApplyEvent(&model, &event);
    assert(memcmp(&model.status, &previous_status, sizeof(previous_status)) == 0);
    assert(model.dirty_mask == APP_UI_DIRTY_NONE);

    clear_event(&event, APP_UI_EVENT_WIFI_STATE_CHANGED);
    event.data.wifi = APP_UI_WIFI_STATE_COUNT;
    App_UiModel_ApplyEvent(&model, &event);
    assert(memcmp(&model.status, &previous_status, sizeof(previous_status)) == 0);
    assert(model.dirty_mask == APP_UI_DIRTY_NONE);

    clear_event(&event, APP_UI_EVENT_BLUETOOTH_STATE_CHANGED);
    event.data.bluetooth = APP_UI_BLUETOOTH_STATE_COUNT;
    App_UiModel_ApplyEvent(&model, &event);
    assert(memcmp(&model.status, &previous_status, sizeof(previous_status)) == 0);
    assert(model.dirty_mask == APP_UI_DIRTY_NONE);

    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_SetCurrentPage(&model, APP_UI_PAGE_NONE);
    assert(model.current_page == APP_UI_PAGE_NONE);
    assert((model.dirty_mask & APP_UI_DIRTY_NAV) != 0u);
    return 0;
}
