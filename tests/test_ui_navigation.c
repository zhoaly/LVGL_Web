#include <assert.h>

#include "App_UiNav.h"

int main(void)
{
    app_ui_nav_t nav;
    app_ui_page_id_t future_page = (app_ui_page_id_t)2;

    App_UiNav_Init(&nav, APP_UI_PAGE_HOME);
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
    assert(!App_UiNav_CanBack(&nav));
    assert(!App_UiNav_Push(&nav, APP_UI_PAGE_NONE));
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);

    assert(App_UiNav_Push(&nav, future_page));
    assert(App_UiNav_Current(&nav) == future_page);
    assert(App_UiNav_CanBack(&nav));
    assert(App_UiNav_Back(&nav));
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
    assert(!App_UiNav_Back(&nav));

    assert(App_UiNav_Replace(&nav, future_page));
    assert(App_UiNav_Current(&nav) == future_page);
    App_UiNav_Home(&nav, APP_UI_PAGE_HOME);
    assert(App_UiNav_Current(&nav) == APP_UI_PAGE_HOME);
    assert(!App_UiNav_CanBack(&nav));
    return 0;
}
