#ifndef APP_UI_PAGES_H
#define APP_UI_PAGES_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl/lvgl.h"
#include "../App_UiModel.h"

typedef struct {
    app_ui_page_id_t id;
    const char *title;
    uint32_t dirty_mask;
    bool show_back;
    void (*build)(lv_obj_t *parent, const app_ui_model_t *model);
    void (*refresh)(const app_ui_model_t *model);
} app_ui_page_t;

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id);
const app_ui_page_t *App_UiPageHome_Get(void);
const app_ui_page_t *App_UiPageUsbStatus_Get(void);
const app_ui_page_t *App_UiPageRuntime_Get(void);

#endif
