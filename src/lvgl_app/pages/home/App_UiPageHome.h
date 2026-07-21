/**
 * @file App_UiPageHome.h
 * @brief 首页（Home Page）头文件。
 *
 * 提供首页页面描述符的获取函数。
 */

#ifndef APP_UI_PAGE_HOME_H
#define APP_UI_PAGE_HOME_H

#include "../registry/App_UiPages.h"

/**
 * @brief 获取首页的页面描述符
 * @return 首页页面描述符指针
 */
const app_ui_page_t *App_UiPageHome_Get(void);

#endif /* APP_UI_PAGE_HOME_H */
