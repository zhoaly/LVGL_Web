#ifndef APP_UI_MODEL_H
#define APP_UI_MODEL_H

#include <stdint.h>

#include "../app/App_Ui.h"

typedef struct {
    app_ui_page_id_t current_page;
    uint32_t dirty_mask;
    char message[48];
} app_ui_model_t;

void App_UiModel_Init(app_ui_model_t *model);
void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event);
void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id);

#endif /* APP_UI_MODEL_H */
