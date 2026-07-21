/**
 * @file App_UiPages.c
 * @brief 页面注册中心 —— 维护所有页面的映射表，提供按 ID 查找功能。
 *
 * 【设计说明】
 * 此文件维护一个页面描述符数组，所有页面在此注册。
 * 导航系统通过 App_UiPages_Get() 根据页面 ID 查找对应的页面描述符。
 * 新增页面时在 pages[] 数组中添加即可。
 */

#include "App_UiPages.h"

#include <stddef.h>

#include "../home/App_UiPageHome.h"

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id)
{
    /* 所有已注册页面的描述符数组 */
    const app_ui_page_t *pages[] = {
        App_UiPageHome_Get(),  /* 首页（APP_UI_PAGE_HOME） */
    };
    uint32_t i;

    /* 遍历查找匹配的页面 ID */
    for(i = 0; i < (uint32_t)(sizeof(pages) / sizeof(pages[0])); i++) {
        if(pages[i] != NULL && pages[i]->id == page_id) {
            return pages[i];
        }
    }
    return NULL; /* 未找到对应页面 */
}
