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
} app_ui_page_id_t;

typedef enum {
    APP_UI_DIRTY_NONE = 0,
    APP_UI_DIRTY_SYSTEM = (1u << 0),
    APP_UI_DIRTY_NAV = (1u << 1),
    APP_UI_DIRTY_ALL = 0xFFFFFFFFu,
} app_ui_dirty_mask_t;

typedef enum {
    APP_UI_EVENT_NONE = 0,
    APP_UI_EVENT_SHOW_MESSAGE,
} app_ui_event_type_t;

typedef struct {
    app_ui_event_type_t type;
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

#endif /* APP_UI_H */
