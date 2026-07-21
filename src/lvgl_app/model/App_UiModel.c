/**
 * @file App_UiModel.c
 * @brief Model 层实现 —— 状态初始化、事件处理、页面设置。
 *
 * 【设计原则】
 * Model 是纯数据层，遵循以下约束：
 *   - 不包含任何 LVGL 调用或图形界面相关代码
 *   - 不分配动态内存
 *   - 所有操作均为简单的数据赋值
 */

#include "App_UiModel.h"

#include <stdio.h>
#include <string.h>

void App_UiModel_Init(app_ui_model_t *model)
{
    if(model == NULL) {
        return;
    }

    /* 清零 Model，设置首页并标记全量刷新 */
    memset(model, 0, sizeof(*model));
    model->current_page = APP_UI_PAGE_HOME;
    model->dirty_mask = APP_UI_DIRTY_ALL;
}

void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event)
{
    if(model == NULL || event == NULL) {
        return;
    }

    /* 根据事件类型更新 Model 字段 */
    if(event->type == APP_UI_EVENT_SHOW_MESSAGE) {
        /* 复制消息文本并标记系统 dirty */
        snprintf(model->message, sizeof(model->message), "%s", event->text);
        model->dirty_mask |= APP_UI_DIRTY_SYSTEM;
    }
}

void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id)
{
    if(model == NULL) {
        return;
    }

    /* 更新当前页面并标记导航 dirty */
    model->current_page = page_id;
    model->dirty_mask |= APP_UI_DIRTY_NAV;
}
