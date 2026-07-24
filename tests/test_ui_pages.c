#include <assert.h>

#include "App_UiPageHome.h"
#include "App_UiPageText.h"

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t home = {
        .id = APP_UI_PAGE_HOME,
        .title = "LVGL App",
    };
    return &home;
}

const app_ui_page_t *App_UiPageText_Get(void)
{
    static const app_ui_page_t text = {
        .id = APP_UI_PAGE_TEXT,
        .title = "Text Page",
    };
    return &text;
}

int main(void)
{
    assert(App_UiPages_Get(APP_UI_PAGE_HOME) == App_UiPageHome_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_TEXT) == App_UiPageText_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_NONE) == NULL);
    assert(App_UiPages_Get((app_ui_page_id_t)3) == NULL);
    return 0;
}
