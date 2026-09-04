/**
 * @file App_UiMockBridge.c
 * @brief Emscripten-only typed bridge from browser mock controls to UI events.
 */

#include <string.h>
#include "App_UiWifiMock.h"

#include <emscripten/emscripten.h>

#include "../lvgl_app/app/App_Ui.h"

EMSCRIPTEN_KEEPALIVE
int app_ui_mock_set_time(int hour, int minute, int synced)
{
    app_ui_event_t event;

    if(hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_TIME_UPDATED;
    event.data.time.hour = (uint8_t)hour;
    event.data.time.minute = (uint8_t)minute;
    event.data.time.synced = synced != 0;
    return App_UiPostEvent(&event) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int app_ui_mock_set_weather(int temperature_c, int available)
{
    app_ui_event_t event;

    if(temperature_c < -99 || temperature_c > 99) {
        return 0;
    }

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_WEATHER_UPDATED;
    event.data.weather.temperature_c = (int16_t)temperature_c;
    event.data.weather.available = available != 0;
    return App_UiPostEvent(&event) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int app_ui_mock_set_wifi(int state)
{
    return App_UiWifiMock_SetStatus(state);
}

EMSCRIPTEN_KEEPALIVE
int app_ui_mock_set_bluetooth(int state)
{
    app_ui_event_t event;

    if(state < APP_UI_BLUETOOTH_OFF ||
       state >= APP_UI_BLUETOOTH_STATE_COUNT) {
        return 0;
    }

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_BLUETOOTH_STATE_CHANGED;
    event.data.bluetooth = (app_ui_bluetooth_state_t)state;
    return App_UiPostEvent(&event) ? 1 : 0;
}
