#ifndef APP_UI_COMPONENTS_H
#define APP_UI_COMPONENTS_H

#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "../action/app_action.h"

typedef struct {
    app_action_request_t request;
} app_ui_action_binding_t;

void App_UiComponent_InitAction(app_ui_action_binding_t *binding,
                                app_action_id_t action_id,
                                uint32_t page_id);
void App_UiComponent_BindAction(lv_obj_t *object, app_ui_action_binding_t *binding);
lv_obj_t *App_UiComponent_CreateNavBar(lv_obj_t *parent,
                                      bool can_back,
                                      app_ui_action_binding_t bindings[2]);

#endif /* APP_UI_COMPONENTS_H */
