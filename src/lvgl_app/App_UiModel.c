/**
 * @file App_UiModel.c
 * @brief Model 层实现：状态初始化、事件处理、值格式化。
 *
 * 【设计原则】
 * Model 是纯数据层，遵循以下约束：
 *   - 不包含任何 LVGL 调用或图形界面相关代码
 *   - 不包含业务逻辑（如 USB 协议、WiFi 扫描等）
 *   - 不分配动态内存
 *   - 所有字符串操作使用 snprintf 保证缓冲区安全
 *
 * 【职责范围】
 *   1. 接收事件（ApplyEvent）：根据事件类型更新对应字段
 *   2. 维护 dirty 标志：标记哪些业务域发生了变更
 *   3. 格式化输出（FormatValue）：将数据值转换为可显示的字符串
 */

#include "App_UiModel.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief 安全字符串复制辅助函数
 *
 * 使用 snprintf 确保：
 *   - 不超过目标缓冲区大小
 *   - 目标字符串始终以 NULL 结尾
 *   - 源指针为 NULL 时目标置为空字符串
 *
 * @param dst 目标缓冲区
 * @param dst_size 目标缓冲区大小
 * @param src 源字符串（可为 NULL）
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
    /* 清零整个 Model 结构体 */
    memset(model, 0, sizeof(*model));

    /* 设置默认初始值（模拟真实硬件启动后的典型状态） */
    model->current_page = APP_UI_PAGE_HOME;     /* 默认显示首页 */
    model->free_heap_kb = 312;                  /* 模拟空闲堆 312KB */
    model->min_heap_kb = 296;                   /* 模拟最小堆 296KB */
    model->task_count = 9;                      /* 模拟系统任务数 */
    model->usb_mode = 2;                        /* 默认 USB 模式：ESP */
    model->usb_hub_enabled = 1;                 /* HUB 电源默认开启 */
    model->usb_device_count = 3;                /* 模拟已连接 3 个设备 */
    model->wifi_state = 2;                      /* WiFi 状态：已连接 */
    model->wifi_rssi = -48;                     /* 信号强度 -48dBm */
    copy_text(model->wifi_ssid, sizeof(model->wifi_ssid), "ESP32HUB_AP");
    copy_text(model->wifi_ip, sizeof(model->wifi_ip), "192.168.4.1");
    copy_text(model->reset_reason, sizeof(model->reset_reason), "PowerOn");
}

void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id)
{
    /* 更新当前页面 ID 并标记导航 dirty */
    model->current_page = page_id;
    model->dirty_mask |= APP_UI_DIRTY_NAV;
}

void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event)
{
    /* 根据事件类型更新对应字段 */
    switch(event->type) {
    case APP_UI_EVENT_RUNTIME_TICK:
        /* 心跳事件：运行时间 +1 秒 */
        model->uptime_sec++;
        model->dirty_mask |= APP_UI_DIRTY_RUNTIME;
        break;

    case APP_UI_EVENT_USB_MODE_CHANGED:
        /* USB 模式变更：直接设置新模式 */
        model->usb_mode = (uint8_t)event->a;
        model->dirty_mask |= APP_UI_DIRTY_USB;
        break;

    case APP_UI_EVENT_USB_HUB_CHANGED:
        /*
         * HUB 状态变更：
         *   - event->a == 2 时：切换（toggle）当前状态
         *   - 其他值：直接设置（0=关闭，1=开启）
         */
        if(event->a == 2) {
            model->usb_hub_enabled = (uint8_t)(!model->usb_hub_enabled);
        } else {
            model->usb_hub_enabled = (uint8_t)(event->a ? 1 : 0);
        }
        model->dirty_mask |= APP_UI_DIRTY_USB;
        break;

    case APP_UI_EVENT_WIFI_STATE_CHANGED:
        /*
         * WiFi 状态变更：
         *   - event->a == 2 时：切换连接/断开
         *   - 其他值：直接设置状态
         *   - event->b 更新 RSSI 值
         */
        if(event->a == 2) {
            model->wifi_state = (uint8_t)((model->wifi_state == 2) ? 0 : 2);
        } else {
            model->wifi_state = (uint8_t)event->a;
        }
        model->wifi_rssi = (int16_t)event->b;
        model->dirty_mask |= APP_UI_DIRTY_WIFI;
        break;

    case APP_UI_EVENT_HEAP_UPDATED:
        /* 堆内存更新：同步空闲堆和最小堆 */
        model->free_heap_kb = (uint32_t)event->a;
        model->min_heap_kb = (uint32_t)event->b;
        model->dirty_mask |= APP_UI_DIRTY_RUNTIME;
        break;

    case APP_UI_EVENT_SHOW_MESSAGE:
        /* 显示消息：复制消息文本到 Model */
        copy_text(model->message, sizeof(model->message), event->text);
        model->dirty_mask |= APP_UI_DIRTY_SYSTEM;
        break;

    default:
        /* 未知事件类型，忽略 */
        break;
    }
}

const char *App_UiModel_FormatValue(const app_ui_model_t *model, app_ui_value_key_t key, char *buf, size_t buf_size)
{
    const char *wifi_state = "Offline";

    /* 将 WiFi 状态枚举值转换为可读字符串 */
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

    /* 根据 Key 类型格式化对应的数据值 */
    switch(key) {
    case APP_UI_VALUE_USB_MODE:
        /* USB 模式：1=PC, 2=ESP, 其他=OFF */
        if(model->usb_mode == 1) {
            return "PC";
        }
        if(model->usb_mode == 2) {
            return "ESP";
        }
        return "OFF";

    case APP_UI_VALUE_USB_HUB_ENABLED:
        /* HUB 电源状态：非0=ON, 0=OFF */
        return model->usb_hub_enabled ? "ON" : "OFF";

    case APP_UI_VALUE_USB_DEVICE_COUNT:
        /* 设备数量：格式化为无符号整数 */
        snprintf(buf, buf_size, "%u", (unsigned int)model->usb_device_count);
        return buf;

    case APP_UI_VALUE_WIFI_STATE:
        /* WiFi 连接状态：返回对应的可读字符串 */
        return wifi_state;

    case APP_UI_VALUE_WIFI_SSID:
        /* WiFi SSID：直接返回字符串 */
        return model->wifi_ssid;

    case APP_UI_VALUE_WIFI_IP:
        /* WiFi IP 地址：直接返回字符串 */
        return model->wifi_ip;

    case APP_UI_VALUE_WIFI_RSSI:
        /* WiFi 信号强度：格式化为 "X dBm" */
        snprintf(buf, buf_size, "%d dBm", (int)model->wifi_rssi);
        return buf;

    case APP_UI_VALUE_UPTIME:
        /* 运行时间：格式化为 "X s" */
        snprintf(buf, buf_size, "%lu s", (unsigned long)model->uptime_sec);
        return buf;

    case APP_UI_VALUE_FREE_HEAP:
        /* 空闲堆内存：格式化为 "X KB" */
        snprintf(buf, buf_size, "%lu KB", (unsigned long)model->free_heap_kb);
        return buf;

    case APP_UI_VALUE_MIN_HEAP:
        /* 最小空闲堆内存：格式化为 "X KB" */
        snprintf(buf, buf_size, "%lu KB", (unsigned long)model->min_heap_kb);
        return buf;

    case APP_UI_VALUE_TASK_COUNT:
        /* 系统任务数：格式化为无符号整数 */
        snprintf(buf, buf_size, "%u", (unsigned int)model->task_count);
        return buf;

    case APP_UI_VALUE_RESET_REASON:
        /* 重启原因：直接返回字符串 */
        return model->reset_reason;

    default:
        /* 未知 Key：返回占位符 */
        return "-";
    }
}
