#include <assert.h>

#include "App_UiPageHome.h"

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t home = {
        .id = APP_UI_PAGE_HOME,
        .title = "LVGL App",
    };
    return &home;
}

int main(void)
{
    assert(App_UiPages_Get(APP_UI_PAGE_HOME) == App_UiPageHome_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_NONE) == NULL);
    assert(App_UiPages_Get((app_ui_page_id_t)2) == NULL);
    return 0;
}
