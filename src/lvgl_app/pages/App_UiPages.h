/**
 * @file App_UiPages.h
 * @brief 页面管理头文件 —— 定义页面描述符结构和页面注册 API。
 *
 * 【页面描述符模式】
 * 每个页面由一个 app_ui_page_t 结构体描述，包含：
 *   - id：页面唯一标识
 *   - title：页面标题
 *   - dirty_mask：该页面关心的业务域变更标志
 *   - show_back：是否显示返回按钮
 *   - build：构建页面 UI 的回调函数
 *   - refresh：刷新页面数据的回调函数
 *
 * 【新增页面步骤】
 *   1. 在 include/App_Ui.h 的 app_ui_page_id_t 中添加新枚举值
 *   2. 创建 App_UiPageXxx.c 实现 build/refresh 回调
 *   3. 在 App_UiPages.c 中注册新页面
 */

#ifndef APP_UI_PAGES_H
#define APP_UI_PAGES_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl/lvgl.h"
#include "../App_UiModel.h"

/**
 * @brief 页面描述符结构体
 *
 * 每个页面使用一个静态常量结构体来描述其属性，
 * 通过 Get 函数返回指针供导航系统使用。
 */
typedef struct {
    app_ui_page_id_t id;                    /**< 页面 ID（唯一标识） */
    const char *title;                      /**< 页面标题（显示在标题栏） */
    uint32_t dirty_mask;                    /**< 页面关心的 dirty 标志位（决定哪些事件触发刷新） */
    bool show_back;                         /**< 是否显示返回按钮 */
    void (*build)(lv_obj_t *parent, const app_ui_model_t *model);     /**< 构建页面 UI 回调 */
    void (*refresh)(const app_ui_model_t *model);                     /**< 刷新页面数据回调 */
} app_ui_page_t;

/**
 * @brief 根据页面 ID 获取页面描述符
 * @param page_id 页面 ID
 * @return 页面描述符指针，未找到返回 NULL
 */
const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id);

/**
 * @brief 获取首页的页面描述符
 * @return 首页页面描述符指针
 */
const app_ui_page_t *App_UiPageHome_Get(void);

/**
 * @brief 获取 USB 状态页的页面描述符
 * @return USB 状态页页面描述符指针
 */
const app_ui_page_t *App_UiPageUsbStatus_Get(void);

/**
 * @brief 获取运行时信息页的页面描述符
 * @return 运行时信息页页面描述符指针
 */
const app_ui_page_t *App_UiPageRuntime_Get(void);

#endif /* APP_UI_PAGES_H */
