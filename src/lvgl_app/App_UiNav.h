/**
 * @file App_UiNav.h
 * @brief 导航管理层：维护页面栈和页面跳转逻辑的头文件。
 *
 * 【导航语义】
 *   - Push（进入）：将当前页面压入历史栈，跳转到子页面
 *   - Back（返回）：从历史栈中弹出上一级页面
 *   - Replace（替换）：直接切换页面，不修改历史栈
 *   - Home（首页）：清空整个历史栈回到首页
 *
 * 【资源约束】
 *   页面栈深度限制为 8 层（APP_UI_NAV_STACK_DEPTH），
 *   超出时 Push 返回 false。
 *   页面离开时 View 层会销毁其 LVGL 对象，返回时重新创建，
 *   以降低 RAM 占用。
 */

#ifndef APP_UI_NAV_H
#define APP_UI_NAV_H

#include <stdbool.h>
#include <stdint.h>

#include "include/App_Ui.h"

/** 页面历史栈最大深度（防止无限级联导航消耗 RAM） */
#define APP_UI_NAV_STACK_DEPTH 8u

/**
 * @brief 导航上下文结构体
 *
 * 维护一个页面 ID 栈（stack）和当前页面 ID（current）。
 * depth 表示栈中元素个数，同时也是可返回的级数。
 * 当前页面始终由 current 字段直接表示。
 */
typedef struct {
    app_ui_page_id_t stack[APP_UI_NAV_STACK_DEPTH]; /**< 页面历史栈：存储 Push 之前的页面 ID */
    uint8_t depth;      /**< 当前栈深度（同时也是可返回的级数） */
    app_ui_page_id_t current; /**< 当前正在显示的页面 ID */
} app_ui_nav_t;

/**
 * @brief 初始化导航管理器，设置首页。
 * @param nav 导航上下文
 * @param home_page 首页页面 ID
 */
void App_UiNav_Init(app_ui_nav_t *nav, app_ui_page_id_t home_page);

/**
 * @brief 进入子页面：将当前页面压栈，跳转到目标页面。
 * @param nav 导航上下文
 * @param next_page 目标页面 ID
 * @return true 成功，false 栈已满
 */
bool App_UiNav_Push(app_ui_nav_t *nav, app_ui_page_id_t next_page);

/**
 * @brief 替换当前页面：不保存历史，直接切换到目标页面。
 * 适用于 Tab 切换等不需要返回的场景。
 * @param nav 导航上下文
 * @param next_page 目标页面 ID
 * @return true 成功
 */
bool App_UiNav_Replace(app_ui_nav_t *nav, app_ui_page_id_t next_page);

/**
 * @brief 返回上一级页面：从栈中弹出历史页面。
 * @param nav 导航上下文
 * @return true 成功，false 已经是首页或栈为空
 */
bool App_UiNav_Back(app_ui_nav_t *nav);

/**
 * @brief 清空导航栈并回到首页。
 * @param nav 导航上下文
 * @param home_page 首页页面 ID
 */
void App_UiNav_Home(app_ui_nav_t *nav, app_ui_page_id_t home_page);

/**
 * @brief 检查当前是否可以返回上一级。
 * @param nav 导航上下文
 * @return true 有上级页面可返回
 */
bool App_UiNav_CanBack(const app_ui_nav_t *nav);

/**
 * @brief 获取当前页面 ID。
 * @param nav 导航上下文
 * @return 当前页面 ID
 */
app_ui_page_id_t App_UiNav_Current(const app_ui_nav_t *nav);

#endif /* APP_UI_NAV_H */
