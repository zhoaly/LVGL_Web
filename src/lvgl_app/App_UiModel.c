/**
 * @file App_UiModel.c
 * @brief Model 层实现：状态初始化、事件处理、值格式化。
 *
 * Model 是纯数据层，不包含任何 LVGL 调用或业务逻辑。
 * 它的职责是接收事件、更新字段、标记 dirty，以及将数据值格式化为显示字符串。
 */

#include "App_UiModel.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 安全字符串复制辅助函数。
 * 使用 snprintf 确保不超过缓冲区大小，并且始终以 NULL 结尾。
 */
static void copy_text(char *dst, size_t dst_size, const char *src)
{
    if(dst_size == 0) {
        return;
    }
    if(src == NULL) {
        dst[0] = '\0';
        return;
    }
    snprintf(dst, dst_size, "%s", src);
}

void App_UiModel_Init(app_ui_model_t *model)
{
    memset(model, 0, sizeof(*model));
    model->current_page = APP_UI_PAGE_HOME;
    model->free_heap_kb = 312;
    model->min_heap_kb = 296;
    model->task_count = 9;
    model->usb_mode = 2;
    model->usb_hub_enabled = 1;
    model->usb_device_count = 3;
    model->wifi_state = 2;
    model->wifi_rssi = -48;
    copy_text(model->wifi_ssid, sizeof(model->wifi_ssid), "ESP32HUB_AP");
    copy_text(model->wifi_ip, sizeof(model->wifi_ip), "192.168.4.1");
    copy_text(model->reset_reason, sizeof(model->reset_reason), "PowerOn");
}

void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id)
{
    model->current_page = page_id;
    model->dirty_mask |= APP_UI_DIRTY_NAV;
}

void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event)
{
    switch(event->type) {
    case APP_UI_EVENT_RUNTIME_TICK:
        model->uptime_sec++;
        model->dirty_mask |= APP_UI_DIRTY_RUNTIME;
        break;
    case APP_UI_EVENT_USB_MODE_CHANGED:
        model->usb_mode = (uint8_t)event->a;
        model->dirty_mask |= APP_UI_DIRTY_USB;
        break;
    case APP_UI_EVENT_USB_HUB_CHANGED:
        if(event->a == 2) {
            model->usb_hub_enabled = (uint8_t)(!model->usb_hub_enabled);
        } else {
            model->usb_hub_enabled = (uint8_t)(event->a ? 1 : 0);
        }
        model->dirty_mask |= APP_UI_DIRTY_USB;
        break;
    case APP_UI_EVENT_WIFI_STATE_CHANGED:
        if(event->a == 2) {
            model->wifi_state = (uint8_t)((model->wifi_state == 2) ? 0 : 2);
        } else {
            model->wifi_state = (uint8_t)event->a;
        }
        model->wifi_rssi = (int16_t)event->b;
        model->dirty_mask |= APP_UI_DIRTY_WIFI;
        break;
    case APP_UI_EVENT_HEAP_UPDATED:
        model->free_heap_kb = (uint32_t)event->a;
        model->min_heap_kb = (uint32_t)event->b;
        model->dirty_mask |= APP_UI_DIRTY_RUNTIME;
        break;
    case APP_UI_EVENT_SHOW_MESSAGE:
        copy_text(model->message, sizeof(model->message), event->text);
        model->dirty_mask |= APP_UI_DIRTY_SYSTEM;
        break;
    default:
        break;
    }
}

const char *App_UiModel_FormatValue(const app_ui_model_t *model, app_ui_value_key_t key, char *buf, size_t buf_size)
{
    const char *wifi_state = "Offline";

    switch(model->wifi_state) {
    case 1:
        wifi_state = "Connecting";
        break;
    case 2:
        wifi_state = "Connected";
        break;
    case 3:
        wifi_state = "Error";
        break;
    default:
        break;
    }

    switch(key) {
    case APP_UI_VALUE_USB_MODE:
        if(model->usb_mode == 1) {
            return "PC";
        }
        if(model->usb_mode == 2) {
            return "ESP";
        }
        return "OFF";
    case APP_UI_VALUE_USB_HUB_ENABLED:
        return model->usb_hub_enabled ? "ON" : "OFF";
    case APP_UI_VALUE_USB_DEVICE_COUNT:
        snprintf(buf, buf_size, "%u", (unsigned int)model->usb_device_count);
        return buf;
    case APP_UI_VALUE_WIFI_STATE:
        return wifi_state;
    case APP_UI_VALUE_WIFI_SSID:
        return model->wifi_ssid;
    case APP_UI_VALUE_WIFI_IP:
        return model->wifi_ip;
    case APP_UI_VALUE_WIFI_RSSI:
        snprintf(buf, buf_size, "%d dBm", (int)model->wifi_rssi);
        return buf;
    case APP_UI_VALUE_UPTIME:
        snprintf(buf, buf_size, "%lu s", (unsigned long)model->uptime_sec);
        return buf;
    case APP_UI_VALUE_FREE_HEAP:
        snprintf(buf, buf_size, "%lu KB", (unsigned long)model->free_heap_kb);
        return buf;
    case APP_UI_VALUE_MIN_HEAP:
        snprintf(buf, buf_size, "%lu KB", (unsigned long)model->min_heap_kb);
        return buf;
    case APP_UI_VALUE_TASK_COUNT:
        snprintf(buf, buf_size, "%u", (unsigned int)model->task_count);
        return buf;
    case APP_UI_VALUE_RESET_REASON:
        return model->reset_reason;
    default:
        return "-";
    }
}
