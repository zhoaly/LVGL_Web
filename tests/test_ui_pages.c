#include <assert.h>

#include "App_UiPageHidHub.h"
#include "App_UiPageHome.h"
#include "App_UiPageNetwork.h"
#include "App_UiPageSettings.h"
#include "App_UiPageText.h"

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "Home",
    };
    return &page;
}

const app_ui_page_t *App_UiPageText_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_TEXT,
        .title = "Text Page",
    };
    return &page;
}

const app_ui_page_t *App_UiPageNetwork_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_NETWORK,
        .title = "Network",
    };
    return &page;
}

const app_ui_page_t *App_UiPageHidHub_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HID_HUB,
        .title = "HID Hub",
    };
    return &page;
}

const app_ui_page_t *App_UiPageSettings_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_SETTINGS,
        .title = "Settings",
    };
    return &page;
}

int main(void)
{
    assert(App_UiPages_Get(APP_UI_PAGE_HOME) == App_UiPageHome_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_TEXT) == App_UiPageText_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_NETWORK) == App_UiPageNetwork_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_HID_HUB) == App_UiPageHidHub_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_SETTINGS) == App_UiPageSettings_Get());
    assert(App_UiPages_Get(APP_UI_PAGE_NONE) == NULL);
    assert(App_UiPages_Get(APP_UI_PAGE_COUNT) == NULL);
    return 0;
}
