#ifndef APP_UI_NAV_H
#define APP_UI_NAV_H

#include <stdbool.h>
#include <stdint.h>

#include "../app/App_Ui.h"

#define APP_UI_NAV_STACK_DEPTH 8u

typedef struct {
    app_ui_page_id_t stack[APP_UI_NAV_STACK_DEPTH];
    uint8_t depth;
    app_ui_page_id_t current;
} app_ui_nav_t;

void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page);
bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page);
bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page);
bool App_UiNav_Back(app_ui_nav_t *nav);
void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page);
bool App_UiNav_CanBack(const app_ui_nav_t *nav);
app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav);

#endif /* APP_UI_NAV_H */
