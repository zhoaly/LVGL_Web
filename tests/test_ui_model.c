#include <assert.h>
#include <string.h>

#include "App_UiModel.h"

int main(void)
{
    app_ui_model_t model;
    app_ui_event_t event;

    App_UiModel_Init(&model);
    assert(model.current_page == APP_UI_PAGE_HOME);
    assert(model.dirty_mask == APP_UI_DIRTY_ALL);
    assert(model.message[0] == '\0');

    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_SHOW_MESSAGE;
    strcpy(event.text, "Ready");
    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_ApplyEvent(&model, &event);
    assert(strcmp(model.message, "Ready") == 0);
    assert((model.dirty_mask & APP_UI_DIRTY_SYSTEM) != 0u);

    model.dirty_mask = APP_UI_DIRTY_NONE;
    App_UiModel_SetCurrentPage(&model, APP_UI_PAGE_NONE);
    assert(model.current_page == APP_UI_PAGE_NONE);
    assert((model.dirty_mask & APP_UI_DIRTY_NAV) != 0u);
    return 0;
}
