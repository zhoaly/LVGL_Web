#include "App_UiNav.h"

#include <string.h>

void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    if(nav == NULL) {
        return;
    }

    memset(nav, 0, sizeof(*nav));
    nav->current = home_page;
}

bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    if(nav == NULL || next_page == APP_UI_PAGE_NONE || nav->depth >= APP_UI_NAV_STACK_DEPTH) {
        return false;
    }

    nav->stack[nav->depth++] = nav->current;
    nav->current = next_page;
    return true;
}

bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    if(nav == NULL || next_page == APP_UI_PAGE_NONE) {
        return false;
    }

    nav->current = next_page;
    return true;
}

bool App_UiNav_Back(app_ui_nav_t *nav)
{
    if(nav == NULL || nav->depth == 0u) {
        return false;
    }

    nav->current = nav->stack[--nav->depth];
    return true;
}

void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    if(nav == NULL) {
        return;
    }

    nav->depth = 0u;
    nav->current = home_page;
}

bool App_UiNav_CanBack(const app_ui_nav_t *nav)
{
    return nav != NULL && nav->depth > 0u;
}

app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav)
{
    return nav != NULL ? nav->current : APP_UI_PAGE_NONE;
}
