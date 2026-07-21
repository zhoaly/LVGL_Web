#include "App_UiModel.h"

#include <stdio.h>
#include <string.h>

void App_UiModel_Init(app_ui_model_t *model)
{
    if(model == NULL) {
        return;
    }

    memset(model, 0, sizeof(*model));
    model->current_page = APP_UI_PAGE_HOME;
    model->dirty_mask = APP_UI_DIRTY_ALL;
}

void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event)
{
    if(model == NULL || event == NULL) {
        return;
    }

    if(event->type == APP_UI_EVENT_SHOW_MESSAGE) {
        snprintf(model->message, sizeof(model->message), "%s", event->text);
        model->dirty_mask |= APP_UI_DIRTY_SYSTEM;
    }
}

void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id)
{
    if(model == NULL) {
        return;
    }

    model->current_page = page_id;
    model->dirty_mask |= APP_UI_DIRTY_NAV;
}
