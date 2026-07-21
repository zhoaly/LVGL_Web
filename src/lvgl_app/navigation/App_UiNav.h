/**
 * @file App_UiNav.h
 * @brief 导航管理层头文件 —— 维护页面栈和页面跳转逻辑。
 *
 * 【导航语义】
 *   - Push（进入）：将当前页面压入历史栈，跳转到子页面
 *   - Back（返回）：从历史栈中弹出上一级页面
 *   - Replace（替换）：直接切换页面，不修改历史栈
 *   - Home（首页）：清空整个历史栈回到首页
 *
 * 【资源约束】
 * 页面栈深度限制为 8 层，超出时 Push 返回 false。
 */

#ifndef APP_UI_NAV_H
#define APP_UI_NAV_H

#include <stdbool.h>
#include <stdint.h>

#include "../app/App_Ui.h"

/** 页面历史栈最大深度 */
#define APP_UI_NAV_STACK_DEPTH 8u

/**
 * @brief 导航上下文结构体
 *
 * 维护一个页面 ID 栈（stack）和当前页面 ID（current）。
 * depth 表示栈中元素个数，即可返回的级数。
 */
typedef struct {
    app_ui_page_id_t stack[APP_UI_NAV_STACK_DEPTH]; /**< 页面历史栈 */
    uint8_t depth;      /**< 当前栈深度 */
    app_ui_page_id_t current; /**< 当前页面 ID */
} app_ui_nav_t;

/**
 * @brief 初始化导航管理器，设置首页
 * @param nav 导航上下文
 * @param home_page 首页页面 ID
 */
void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page);

/**
 * @brief 进入子页面：将当前页面压栈，跳转到目标页面
 * @param nav 导航上下文
 * @param next_page 目标页面 ID
 * @return true 成功，false 栈已满或参数无效
 */
bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page);

/**
 * @brief 替换当前页面：不保存历史，直接切换到目标页面
 * @param nav 导航上下文
 * @param next_page 目标页面 ID
 * @return true 成功
 */
bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page);

/**
 * @brief 返回上一级页面：从栈中弹出历史页面
 * @param nav 导航上下文
 * @return true 成功，false 已经是首页或栈为空
 */
bool App_UiNav_Back(app_ui_nav_t *nav);

/**
 * @brief 清空导航栈并回到首页
 * @param nav 导航上下文
 * @param home_page 首页页面 ID
 */
void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page);

/**
 * @brief 检查当前是否可以返回上一级
 * @param nav 导航上下文
 * @return true 有上级页面可返回
 */
bool App_UiNav_CanBack(const app_ui_nav_t *nav);

/**
 * @brief 获取当前页面 ID
 * @param nav 导航上下文
 * @return 当前页面 ID，参数为 NULL 时返回 APP_UI_PAGE_NONE
 */
app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav);

#endif /* APP_UI_NAV_H */
