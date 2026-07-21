/**
 * @file App_UiNav.c
 * @brief 导航管理器实现。
 *
 * 【设计说明】
 * 使用固定数组作为页面历史栈，支持四种导航语义：
 *   - Push：进入子页面（当前页压栈，切换到目标页）
 *   - Pop/Back：返回上一级（从栈中弹出历史页）
 *   - Replace：替换当前页（不修改历史栈）
 *   - Home：回到首页（清空整个栈）
 *
 * 【资源约束】
 *   - 栈深度固定为 8 层
 *   - 不涉及 LVGL 操作
 *   - 不分配动态内存
 *   - 所有函数 O(1) 时间复杂度
 */

#include "App_UiNav.h"

#include <string.h>

void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    if(nav == NULL) {
        return;
    }

    /* 清零导航上下文，设置首页 */
    memset(nav, 0, sizeof(*nav));
    nav->current = home_page;
}

bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    /* 参数校验：导航上下文、目标页面有效、栈未满 */
    if(nav == NULL || next_page == APP_UI_PAGE_NONE || nav->depth >= APP_UI_NAV_STACK_DEPTH) {
        return false;
    }

    /* 将当前页面压入历史栈，切换到目标页面 */
    nav->stack[nav->depth++] = nav->current;
    nav->current = next_page;
    return true;
}

bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    /* 直接替换当前页面，不修改历史栈 */
    if(nav == NULL || next_page == APP_UI_PAGE_NONE) {
        return false;
    }

    nav->current = next_page;
    return true;
}

bool App_UiNav_Back(app_ui_nav_t *nav)
{
    /* 栈为空时无法返回 */
    if(nav == NULL || nav->depth == 0u) {
        return false;
    }

    /* 从栈中弹出上一级页面 */
    nav->current = nav->stack[--nav->depth];
    return true;
}

void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    if(nav == NULL) {
        return;
    }

    /* 清空历史栈，回到首页 */
    nav->depth = 0u;
    nav->current = home_page;
}

bool App_UiNav_CanBack(const app_ui_nav_t *nav)
{
    /* 栈非空时允许返回 */
    return nav != NULL && nav->depth > 0u;
}

app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav)
{
    return nav != NULL ? nav->current : APP_UI_PAGE_NONE;
}
