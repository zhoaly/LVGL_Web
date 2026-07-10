/**
 * @file App_UiNav.c
 * @brief 导航管理器实现。
 *
 * 使用固定数组作为页面历史栈，支持 Push/Back/Replace/Home 四种导航语义。
 * 栈深度限制为 8 层，避免无限级联导航消耗 RAM。
 * 所有函数均不涉及 LVGL 操作，也不分配动态内存。
 */

#include "App_UiNav.h"

void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    nav->depth = 0;
    nav->current = home_page;
}

bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    if(nav->depth >= APP_UI_NAV_STACK_DEPTH) {
        return false;
    }
    /* 将当前页面压栈，然后切换到目标页面 */
    nav->stack[nav->depth++] = nav->current;
    nav->current = next_page;
    return true;
}

bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    /* 直接替换当前页面，不更改历史栈 */
    nav->current = next_page;
    return true;
}

bool App_UiNav_Back(app_ui_nav_t *nav)
{
    if(nav->depth == 0) {
        return false;  /* 栈为空，无法返回 */
    }
    /* 从栈中弹出上一级页面 */
    nav->current = nav->stack[--nav->depth];
    return true;
}

void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    /* 清空整个栈并回到首页 */
    nav->depth = 0;
    nav->current = home_page;
}

bool App_UiNav_CanBack(const app_ui_nav_t *nav)
{
    return nav->depth > 0;
}

app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav)
{
    return nav->current;
}
