/**
 * @file App_UiPages.h
 * @brief 页面管理头文件 —— 定义页面描述符结构和页面查找 API。
 *
 * 【页面描述符模式】
 * 每个页面由一个 app_ui_page_t 结构体描述，包含：
 *   - id：页面唯一标识
 *   - title：页面标题
 *   - dirty_mask：该页面关心的业务域变更标志
 *   - show_back：是否显示返回按钮
 *   - build：构建页面 UI 的回调
 *   - refresh：刷新页面数据的回调
 */

#ifndef APP_UI_PAGES_H
#define APP_UI_PAGES_H

#include <stdbool.h>
#include <stdint.h>

#include "../../model/App_UiModel.h"

/* 前向声明 LVGL 对象类型，避免在头文件中包含 lvgl.h */
typedef struct _lv_obj_t lv_obj_t;

typedef enum {
    APP_UI_PAGE_TRANSITION_INITIAL = 0,
    APP_UI_PAGE_TRANSITION_PUSH,
    APP_UI_PAGE_TRANSITION_BACK,
    APP_UI_PAGE_TRANSITION_HOME,
} app_ui_page_transition_t;

/**
 * @brief 页面描述符结构体
 */
typedef struct {
    app_ui_page_id_t id;                    /**< 页面 ID */
    const char *title;                      /**< 页面标题 */
    uint32_t dirty_mask;                    /**< 页面关心的 dirty 标志位 */
    bool show_back;                         /**< 是否显示返回按钮 */
    void (*build)(lv_obj_t *parent, const app_ui_model_t *model);     /**< 构建 UI 回调 */
    void (*refresh)(const app_ui_model_t *model);                     /**< 刷新数据回调 */
    /** Actual overlap supplied by View; pages must not hard-code Dock dimensions. */
    void (*viewport_changed)(int32_t bottom_inset);
    void (*leave)(void);
    void (*enter)(app_ui_page_transition_t transition);
} app_ui_page_t;

/**
 * @brief 根据页面 ID 获取页面描述符
 * @param page_id 页面 ID
 * @return 页面描述符指针，未找到返回 NULL
 */
const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id);

#endif /* APP_UI_PAGES_H */
