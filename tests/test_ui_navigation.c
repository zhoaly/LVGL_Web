#include <assert.h>
#include <stddef.h>

#include "App_UiNav.h"

int main(void)
{
    static const app_ui_page_id_t menu_pages[] = {
        APP_UI_PAGE_NETWORK,
        APP_UI_PAGE_HID_HUB,
        APP_UI_PAGE_SETTINGS,
    };
    app_ui_nav_t nav;
    size_t index;

    App_UiNav_Init(&nav, APP_UI_PAGE_HOME);
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
    assert(!App_UiNav_CanBack(&nav));
    assert(!App_UiNav_Push(&nav, APP_UI_PAGE_NONE));

    for(index = 0u; index < sizeof(menu_pages) / sizeof(menu_pages[0]); index++) {
        assert(App_UiNav_Push(&nav, menu_pages[index]));
        assert(App_UiNav_Current(&nav) == menu_pages[index]);
        assert(App_UiNav_CanBack(&nav));
        assert(App_UiNav_Back(&nav));
        assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
        assert(!App_UiNav_CanBack(&nav));
    }

    assert(App_UiNav_Push(&nav, APP_UI_PAGE_NETWORK));
    assert(App_UiNav_Push(&nav, APP_UI_PAGE_HID_HUB));
    assert(App_UiNav_Push(&nav, APP_UI_PAGE_SETTINGS));
    App_UiNav_Home(&nav, APP_UI_PAGE_HOME);
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
    assert(!App_UiNav_CanBack(&nav));

    assert(App_UiNav_Replace(&nav, APP_UI_PAGE_SETTINGS));
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_SETTINGS);
    return 0;
}
