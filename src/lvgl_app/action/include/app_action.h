#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef ESP_PLATFORM
#include "esp_err.h"
#else
typedef int32_t esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_NO_MEM 0x101
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#define ESP_ERR_NOT_FOUND 0x105
#define ESP_ERR_NOT_SUPPORTED 0x106
#define ESP_ERR_TIMEOUT 0x107
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define APP_ACTION_MESSAGE_MAX_LEN 96

typedef uint32_t app_action_job_id_t;

/* Keep the ESP Action IDs in their original order. UI IDs are appended so
 * existing numeric values remain stable. */
typedef enum {
    APP_ACTION_ID_INVALID = 0,
    APP_ACTION_ID_EPD_INFO,
    APP_ACTION_ID_EPD_INIT,
    APP_ACTION_ID_EPD_CLEAR,
    APP_ACTION_ID_EPD_BLACK,
    APP_ACTION_ID_EPD_BW,
    APP_ACTION_ID_EPD_SLEEP,
    APP_ACTION_ID_EPD_WAKE,
    APP_ACTION_ID_WS2812_INIT,
    APP_ACTION_ID_WS2812_DEINIT,
    APP_ACTION_ID_WS2812_STATUS,
    APP_ACTION_ID_WS2812_ON,
    APP_ACTION_ID_WS2812_OFF,
    APP_ACTION_ID_WS2812_RGB,
    APP_ACTION_ID_WS2812_PIXEL,
    APP_ACTION_ID_WS2812_CLEAR,
    APP_ACTION_ID_WS2812_FRAME,
    APP_ACTION_ID_WS2812_DEMO,
    APP_ACTION_ID_ENCODER_STATUS,
    APP_ACTION_ID_ENCODER_START,
    APP_ACTION_ID_ENCODER_STOP,
    APP_ACTION_ID_ENCODER_RESET,
    APP_ACTION_ID_ENCODER_REVERSE,
    APP_ACTION_ID_KEY_STATUS,
    APP_ACTION_ID_KEY_RAW,
    APP_ACTION_ID_KEY_START,
    APP_ACTION_ID_KEY_STOP,
    APP_ACTION_ID_KEY_RESET_STAT,
    APP_ACTION_ID_WIFI_INIT,
    APP_ACTION_ID_WIFI_DEINIT,
    APP_ACTION_ID_WIFI_STATUS,
    APP_ACTION_ID_WIFI_STA_START,
    APP_ACTION_ID_WIFI_STA_STOP,
    APP_ACTION_ID_WIFI_STA_CONNECT,
    APP_ACTION_ID_WIFI_STA_DISCONNECT,
    APP_ACTION_ID_WIFI_STA_RECONNECT,
    APP_ACTION_ID_WIFI_STA_CONNECT_SAVED,
    APP_ACTION_ID_WIFI_SCAN_START,
    APP_ACTION_ID_WIFI_SCAN_WAIT,
    APP_ACTION_ID_WIFI_SCAN_LIST,
    APP_ACTION_ID_WIFI_NVS_SAVE,
    APP_ACTION_ID_WIFI_NVS_LOAD,
    APP_ACTION_ID_WIFI_NVS_CLEAR,
    APP_ACTION_ID_WIFI_NVS_HAS,
    APP_ACTION_ID_HTTP_INIT,
    APP_ACTION_ID_HTTP_GET,
    APP_ACTION_ID_HTTP_POST_JSON,
    APP_ACTION_ID_OTA_VERSION,
    APP_ACTION_ID_OTA_STATUS,
    APP_ACTION_ID_OTA_CHECK,
    APP_ACTION_ID_OTA_UPGRADE_MANIFEST,
    APP_ACTION_ID_OTA_UPGRADE,
    APP_ACTION_ID_OTA_UPGRADE_CHECKED,
    APP_ACTION_ID_OTA_CONFIRM,
    APP_ACTION_ID_OTA_ROLLBACK,
    APP_ACTION_ID_TIME_INIT,
    APP_ACTION_ID_TIME_START,
    APP_ACTION_ID_TIME_SYNC,
    APP_ACTION_ID_TIME_NOW,
    APP_ACTION_ID_TIME_STATUS,
    APP_ACTION_ID_TIME_DEINIT,
    APP_ACTION_ID_GPIO_INFO,
    APP_ACTION_ID_GPIO_GET,
    APP_ACTION_ID_GPIO_SET,
    APP_ACTION_ID_GPIO_INPUT,
    APP_ACTION_ID_GPIO_OUTPUT,
    APP_ACTION_ID_GPIO_PULLUP,
    APP_ACTION_ID_GPIO_PULLDOWN,
    APP_ACTION_ID_GPIO_TOGGLE,
    APP_ACTION_ID_GPIO_WATCH,
    APP_ACTION_ID_SYS_INFO,
    APP_ACTION_ID_SYS_RESTART,
    APP_ACTION_ID_SYS_HEAP,
    APP_ACTION_ID_RUNTIME,
    APP_ACTION_ID_TASKLIST,
    APP_ACTION_ID_UI_NAV_BACK,
    APP_ACTION_ID_UI_NAV_HOME,
    APP_ACTION_ID_UI_NAV_PUSH,
    APP_ACTION_ID_MAX,
} app_action_id_t;

typedef enum {
    APP_ACTION_JOB_STATE_INVALID = 0,
    APP_ACTION_JOB_STATE_PENDING,
    APP_ACTION_JOB_STATE_RUNNING,
    APP_ACTION_JOB_STATE_DONE,
    APP_ACTION_JOB_STATE_FAILED,
} app_action_job_state_t;

typedef struct {
    app_action_id_t id;
    union {
        struct {
            uint32_t page_id;
        } ui_navigation;
        uint8_t reserved[768];
    } params;
} app_action_request_t;

typedef struct {
    app_action_job_id_t job_id;
    app_action_id_t action_id;
    app_action_job_state_t state;
    esp_err_t result;
    int64_t submitted_at_us;
    int64_t started_at_us;
    int64_t finished_at_us;
    char message[APP_ACTION_MESSAGE_MAX_LEN];
} app_action_job_snapshot_t;

typedef esp_err_t (*app_action_ui_dispatcher_fn)(const app_action_request_t *request, void *user_data);

esp_err_t app_action_init(void);
esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher, void *user_data);
esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);
esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);
esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot);
const char *app_action_id_to_name(app_action_id_t id);

#ifdef __cplusplus
}
#endif
