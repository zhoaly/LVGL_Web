#include "App_Ui.h"

#include <stdio.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include "../action/app_action.h"
#include "../model/App_UiModel.h"
#include "../navigation/App_UiNav.h"
#include "../pages/registry/App_UiPages.h"
#include "../port/App_UiPort.h"
#include "../view/App_UiView.h"

#define APP_UI_QUEUE_LEN 16u
#define APP_UI_PUMP_BATCH_LEN 8u

typedef enum {
    APP_UI_QUEUE_MODEL_EVENT = 0,
    APP_UI_QUEUE_NAVIGATION,
} app_ui_queue_item_type_t;

typedef struct {
    app_ui_queue_item_type_t type;
    union {
        app_ui_event_t event;
        struct {
            app_action_id_t action_id;
            app_ui_page_id_t page_id;
        } navigation;
    } data;
} app_ui_queue_item_t;

typedef struct {
    bool ready;
    bool started;
    app_ui_model_t model;
    app_ui_nav_t nav;
    app_ui_view_t view;
    app_ui_queue_item_t queue[APP_UI_QUEUE_LEN];
    uint8_t q_head;
    uint8_t q_tail;
    lv_timer_t *pump_timer;
} app_ui_ctx_t;

static app_ui_ctx_t s_ui;

static bool queue_push(const app_ui_queue_item_t *item)
{
    uint8_t next_tail;
    bool pushed = false;

    if(item == NULL) {
        return false;
    }

    App_UiPort_EnterCritical();
    next_tail = (uint8_t)((s_ui.q_tail + 1u) % APP_UI_QUEUE_LEN);
    if(next_tail != s_ui.q_head) {
        s_ui.queue[s_ui.q_tail] = *item;
        s_ui.q_tail = next_tail;
        pushed = true;
    }
    App_UiPort_ExitCritical();
    return pushed;
}

static bool queue_pop(app_ui_queue_item_t *item)
{
    bool popped = false;

    if(item == NULL) {
        return false;
    }

    App_UiPort_EnterCritical();
    if(s_ui.q_head != s_ui.q_tail) {
        *item = s_ui.queue[s_ui.q_head];
        s_ui.q_head = (uint8_t)((s_ui.q_head + 1u) % APP_UI_QUEUE_LEN);
        popped = true;
    }
    App_UiPort_ExitCritical();
    return popped;
}

static bool show_current_page(bool full_refresh)
{
    const app_ui_page_t *page = App_UiPages_Get(App_UiNav_Current(&s_ui.nav));

    if(page == NULL) {
        return false;
    }

    App_UiModel_SetCurrentPage(&s_ui.model, page->id);
    App_UiView_ShowPage(&s_ui.view, page, &s_ui.model, App_UiNav_CanBack(&s_ui.nav));
    s_ui.model.dirty_mask &= ~APP_UI_DIRTY_NAV;
    App_UiPort_RequestFlush(full_refresh);
    return true;
}

static esp_err_t ui_action_dispatcher(const app_action_request_t *request, void *user_data)
{
    app_ui_queue_item_t item;
    app_ui_page_id_t page_id;
    (void)user_data;

    if(request == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if(request->id != APP_ACTION_ID_UI_NAV_BACK &&
       request->id != APP_ACTION_ID_UI_NAV_HOME &&
       request->id != APP_ACTION_ID_UI_NAV_PUSH) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    page_id = (app_ui_page_id_t)request->params.ui_navigation.page_id;
    if(request->id == APP_ACTION_ID_UI_NAV_PUSH && App_UiPages_Get(page_id) == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(&item, 0, sizeof(item));
    item.type = APP_UI_QUEUE_NAVIGATION;
    item.data.navigation.action_id = request->id;
    item.data.navigation.page_id = page_id;
    return queue_push(&item) ? ESP_OK : ESP_ERR_TIMEOUT;
}

static void process_navigation(const app_ui_queue_item_t *item)
{
    bool changed = false;

    if(item == NULL) {
        return;
    }

    switch(item->data.navigation.action_id) {
    case APP_ACTION_ID_UI_NAV_BACK:
        changed = App_UiNav_Back(&s_ui.nav);
        break;
    case APP_ACTION_ID_UI_NAV_HOME:
        changed = App_UiNav_Current(&s_ui.nav) != APP_UI_PAGE_HOME ||
                  App_UiNav_CanBack(&s_ui.nav);
        App_UiNav_Home(&s_ui.nav, APP_UI_PAGE_HOME);
        break;
    case APP_ACTION_ID_UI_NAV_PUSH:
        if(App_UiPages_Get(item->data.navigation.page_id) != NULL) {
            changed = App_UiNav_Push(&s_ui.nav, item->data.navigation.page_id);
        }
        break;
    default:
        break;
    }

    if(changed) {
        (void)show_current_page(true);
    }
}

static void ui_pump_timer_cb(lv_timer_t *timer)
{
    app_ui_queue_item_t item;
    const app_ui_page_t *page;
    uint8_t processed = 0u;
    uint32_t refresh_mask;
    (void)timer;

    while(processed < APP_UI_PUMP_BATCH_LEN && queue_pop(&item)) {
        if(item.type == APP_UI_QUEUE_MODEL_EVENT) {
            App_UiModel_ApplyEvent(&s_ui.model, &item.data.event);
        } else if(item.type == APP_UI_QUEUE_NAVIGATION) {
            process_navigation(&item);
        }
        processed++;
    }

    page = App_UiPages_Get(App_UiNav_Current(&s_ui.nav));
    if(page == NULL) {
        return;
    }

    refresh_mask = s_ui.model.dirty_mask & page->dirty_mask;
    if(refresh_mask != 0u) {
        App_UiView_Refresh(&s_ui.view, &s_ui.model);
        s_ui.model.dirty_mask &= ~refresh_mask;
        App_UiPort_RequestFlush(false);
    }

    if((s_ui.model.dirty_mask & APP_UI_DIRTY_SYSTEM) != 0u) {
        App_UiView_ShowToast(&s_ui.view, s_ui.model.message);
        s_ui.model.dirty_mask &= ~APP_UI_DIRTY_SYSTEM;
        App_UiPort_RequestFlush(false);
    }
}

bool App_UiInit(void)
{
    memset(&s_ui, 0, sizeof(s_ui));
    App_UiModel_Init(&s_ui.model);
    App_UiNav_Init(&s_ui.nav, APP_UI_PAGE_HOME);

    if(!App_UiPort_Init() || app_action_init() != ESP_OK) {
        return false;
    }

    App_UiView_Init(&s_ui.view);
    if(app_action_register_ui_dispatcher(ui_action_dispatcher, NULL) != ESP_OK) {
        return false;
    }

    s_ui.pump_timer = lv_timer_create(ui_pump_timer_cb, 50, NULL);
    if(s_ui.pump_timer == NULL) {
        return false;
    }

    s_ui.ready = true;
    return true;
}

bool App_UiStart(void)
{
    if(!s_ui.ready) {
        return false;
    }
    if(s_ui.started) {
        return true;
    }

    if(!show_current_page(true)) {
        return false;
    }

    s_ui.started = true;
    return true;
}

bool App_UiPostEvent(const app_ui_event_t *event)
{
    app_ui_queue_item_t item;

    if(!s_ui.ready || event == NULL || event->type <= APP_UI_EVENT_NONE ||
       event->type > APP_UI_EVENT_SHOW_MESSAGE) {
        return false;
    }

    memset(&item, 0, sizeof(item));
    item.type = APP_UI_QUEUE_MODEL_EVENT;
    item.data.event = *event;
    return queue_push(&item);
}

bool App_UiShowMessage(const char *message)
{
    app_ui_event_t event;

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_SHOW_MESSAGE;
    if(message != NULL) {
        snprintf(event.text, sizeof(event.text), "%s", message);
    }
    return App_UiPostEvent(&event);
}

bool App_UiIsReady(void)
{
    return s_ui.ready;
}
