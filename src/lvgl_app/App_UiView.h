#ifndef APP_UI_VIEW_H
#define APP_UI_VIEW_H

#include "lvgl/lvgl.h"

#include "components/App_UiComponents.h"
#include "pages/App_UiPages.h"

typedef struct {
    lv_obj_t *screen_root;
    lv_obj_t *title_label;
    lv_obj_t *content;
    lv_obj_t *toast_label;
    const app_ui_page_t *active_page;
    app_ui_action_binding_t nav_bindings[2];
} app_ui_view_t;

void App_UiView_Init(app_ui_view_t *view);
void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back);
void App_UiView_Refresh(app_ui_view_t *view, const app_ui_model_t *model);
void App_UiView_ShowToast(app_ui_view_t *view, const char *message);

#endif
