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
 *   - 栈深度固定为 8 层（APP_UI_NAV_STACK_DEPTH）
 *   - 不涉及 LVGL 操作
 *   - 不分配动态内存
 *   - 所有函数 O(1) 时间复杂度
 */

#include "App_UiNav.h"

void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    /* 清空页面栈，设置首页 */
    nav->depth = 0;
    nav->current = home_page;
}

bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    /* 检查栈是否已满 */
    if(nav->depth >= APP_UI_NAV_STACK_DEPTH) {
        return false;
    }
    /* 将当前页面压入历史栈，然后切换到目标页面 */
    nav->stack[nav->depth++] = nav->current;
    nav->current = next_page;
    return true;
}

bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page)
{
    /*
     * 直接替换当前页面，不修改历史栈。
     * 适用于 Tab 切换等不需要记录返回路径的场景。
     */
    nav->current = next_page;
    return true;
}

bool App_UiNav_Back(app_ui_nav_t *nav)
{
    /* 栈为空时无法返回 */
    if(nav->depth == 0) {
        return false;
    }
    /* 从栈中弹出上一级页面作为当前页面 */
    nav->current = nav->stack[--nav->depth];
    return true;
}

void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page)
{
    /* 清空整个页面历史栈并回到首页 */
    nav->depth = 0;
    nav->current = home_page;
}

bool App_UiNav_CanBack(const app_ui_nav_t *nav)
{
    /* 栈非空时允许返回 */
    return nav->depth > 0;
}

app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav)
{
    return nav->current;
}
