#ifndef APP_UI_H
#define APP_UI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APP_UI_PAGE_NONE = 0,
    APP_UI_PAGE_HOME,
    APP_UI_PAGE_USB_STATUS,
    APP_UI_PAGE_RUNTIME,
} app_ui_page_id_t;

typedef enum {
    APP_UI_DIRTY_NONE = 0,
    APP_UI_DIRTY_SYSTEM = (1u << 0),
    APP_UI_DIRTY_RUNTIME = (1u << 1),
    APP_UI_DIRTY_USB = (1u << 2),
    APP_UI_DIRTY_WIFI = (1u << 3),
    APP_UI_DIRTY_GPIO = (1u << 4),
    APP_UI_DIRTY_NAV = (1u << 5),
    APP_UI_DIRTY_ALL = 0xFFFFFFFFu,
} app_ui_dirty_mask_t;

typedef enum {
    APP_UI_EVENT_NONE = 0,
    APP_UI_EVENT_RUNTIME_TICK,
    APP_UI_EVENT_USB_MODE_CHANGED,
    APP_UI_EVENT_USB_HUB_CHANGED,
    APP_UI_EVENT_WIFI_STATE_CHANGED,
    APP_UI_EVENT_HEAP_UPDATED,
    APP_UI_EVENT_SHOW_MESSAGE,
} app_ui_event_type_t;

typedef struct {
    app_ui_event_type_t type;
    int32_t a;
    int32_t b;
    char text[48];
} app_ui_event_t;

bool App_UiInit(void);
bool App_UiStart(void);
bool App_UiPostEvent(const app_ui_event_t *event);
bool App_UiShowMessage(const char *message);
bool App_UiIsReady(void);

#ifdef __cplusplus
}
#endif

#endif
