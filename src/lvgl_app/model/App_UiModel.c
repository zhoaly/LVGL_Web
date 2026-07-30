/**
 * @file App_UiModel.c
 * @brief Model 层实现 —— 状态初始化、事件处理、页面设置。
 *
 * 【设计原则】
 * Model 是纯数据层，遵循以下约束：
 *   - 不包含任何 LVGL 调用或图形界面相关代码
 *   - 不分配动态内存
 *   - 所有操作均为简单的数据赋值
 */

#include "App_UiModel.h"

#include <stdio.h>
#include <string.h>

void App_UiModel_Init(app_ui_model_t *model)
{
    if(model == NULL) {
        return;
    }

    /* 清零 Model，设置首页并标记全量刷新 */
    memset(model, 0, sizeof(*model));
    model->current_page = APP_UI_PAGE_HOME;
    model->dirty_mask = APP_UI_DIRTY_ALL;
}

void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event)
{
    if(model == NULL || event == NULL) {
        return;
    }

    switch(event->type) {
    case APP_UI_EVENT_SHOW_MESSAGE:
        snprintf(model->message, sizeof(model->message), "%s", event->text);
        model->dirty_mask |= APP_UI_DIRTY_SYSTEM;
        break;

    case APP_UI_EVENT_TIME_UPDATED:
        if(event->data.time.hour > 23u ||
           event->data.time.minute > 59u) {
            break;
        }
        if(model->status.time.hour != event->data.time.hour ||
           model->status.time.minute != event->data.time.minute ||
           model->status.time.synced != event->data.time.synced) {
            model->status.time = event->data.time;
            model->dirty_mask |= APP_UI_DIRTY_STATUS;
        }
        break;

    case APP_UI_EVENT_WEATHER_UPDATED:
        if(event->data.weather.temperature_c < -99 ||
           event->data.weather.temperature_c > 99) {
            break;
        }
        if(model->status.weather.temperature_c !=
               event->data.weather.temperature_c ||
           model->status.weather.available !=
               event->data.weather.available) {
            model->status.weather = event->data.weather;
            model->dirty_mask |= APP_UI_DIRTY_STATUS;
        }
        break;

    case APP_UI_EVENT_WIFI_STATE_CHANGED:
        if(event->data.wifi < APP_UI_WIFI_DISCONNECTED ||
           event->data.wifi >= APP_UI_WIFI_STATE_COUNT) {
            break;
        }
        if(model->status.wifi != event->data.wifi) {
            model->status.wifi = event->data.wifi;
            model->dirty_mask |= APP_UI_DIRTY_STATUS;
        }
        break;

    case APP_UI_EVENT_BLUETOOTH_STATE_CHANGED:
        if(event->data.bluetooth < APP_UI_BLUETOOTH_OFF ||
           event->data.bluetooth >= APP_UI_BLUETOOTH_STATE_COUNT) {
            break;
        }
        if(model->status.bluetooth != event->data.bluetooth) {
            model->status.bluetooth = event->data.bluetooth;
            model->dirty_mask |= APP_UI_DIRTY_STATUS;
        }
        break;

    default:
        break;
    }
}

void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id)
{
    if(model == NULL) {
        return;
    }

    /* 更新当前页面并标记导航 dirty */
    model->current_page = page_id;
    model->dirty_mask |= APP_UI_DIRTY_NAV;
}
