#ifndef APP_ACTION_H
#define APP_ACTION_H

#include <stdint.h>

#ifdef ESP_PLATFORM
#include "esp_err.h"
#else
typedef int32_t esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#define ESP_ERR_NOT_FOUND 0x105
#define ESP_ERR_NOT_SUPPORTED 0x106
#define ESP_ERR_TIMEOUT 0x107
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define APP_ACTION_MESSAGE_MAX_LEN 48

typedef uint32_t app_action_job_id_t;

typedef enum {
    APP_ACTION_ID_INVALID = 0,
    APP_ACTION_ID_UI_NAV_BACK,
    APP_ACTION_ID_UI_NAV_HOME,
    APP_ACTION_ID_UI_NAV_PUSH,
    APP_ACTION_ID_MAX,
} app_action_id_t;

typedef enum {
    APP_ACTION_JOB_STATE_INVALID = 0,
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
        uint8_t reserved[16];
    } params;
} app_action_request_t;

typedef struct {
    app_action_job_id_t job_id;
    app_action_id_t action_id;
    app_action_job_state_t state;
    esp_err_t result;
    char message[APP_ACTION_MESSAGE_MAX_LEN];
} app_action_job_snapshot_t;

typedef esp_err_t (*app_action_ui_dispatcher_fn)(const app_action_request_t *request,
                                                  void *user_data);

esp_err_t app_action_init(void);
esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                             void *user_data);
esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);
esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);
esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot);
const char *app_action_id_to_name(app_action_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* APP_ACTION_H */
