/**
 * @file App_Ui.c
 * @brief UI 应用主控制器 —— 实现事件队列、导航调度和 UI 生命周期管理。
 *
 * 【架构位置】
 * 此文件是 MVC 架构中的 Controller 部分，负责：
 *   1. 初始化所有模块（Model/View/Nav/Port/Action）
 *   2. 维护事件队列和泵送（pump）循环
 *   3. 调度导航请求和 Model 事件
 *   4. 协调视图刷新
 *
 * 【事件处理流程】
 * 外部事件 -> App_UiPostEvent() -> queue_push() -> ui_pump_timer_cb()
 *   -> queue_pop() -> 类型判断
 *      - MODEL_EVENT: App_UiModel_ApplyEvent() 更新数据
 *      - NAVIGATION: process_navigation() 页面跳转
 *   -> 检查 dirty_mask -> App_UiView_Refresh() 刷新视图
 *   -> 检查系统消息 -> App_UiView_ShowToast() 显示 Toast
 */

#include "App_Ui.h"

#include <stdio.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include "../action/app_action.h"
#include "../model/App_UiModel.h"
#include "../navigation/App_UiNav.h"
#include "../pages/registry/App_UiPages.h"
#include "../port/App_UiPort.h"
#include "../view/App_UiView.h"

/** 内部事件队列长度（循环缓冲区） */
#define APP_UI_QUEUE_LEN 16u

/** 每次泵送最多处理的事件数 */
#define APP_UI_PUMP_BATCH_LEN 8u

/**
 * @brief 内部事件队列项类型
 */
typedef enum {
    APP_UI_QUEUE_MODEL_EVENT = 0,  /**< Model 数据更新事件 */
    APP_UI_QUEUE_NAVIGATION,       /**< 页面导航请求 */
} app_ui_queue_item_type_t;

/**
 * @brief 内部事件队列项结构体（tagged union）
 */
typedef struct {
    app_ui_queue_item_type_t type;
    union {
        app_ui_event_t event;                   /**< Model 事件 */
        struct {
            app_action_id_t action_id;          /**< 导航动作 ID */
            app_ui_page_id_t page_id;           /**< 目标页面 ID */
        } navigation;                           /**< 导航数据 */
    } data;
} app_ui_queue_item_t;

/**
 * @brief UI 全局上下文结构体（单例）
 */
typedef struct {
    bool ready;                             /**< 初始化完成标志 */
    bool started;                           /**< 启动完成标志 */
    app_ui_model_t model;                   /**< 数据模型 */
    app_ui_nav_t nav;                       /**< 导航管理器 */
    app_ui_view_t view;                     /**< 视图层 */
    app_ui_queue_item_t queue[APP_UI_QUEUE_LEN]; /**< 事件循环缓冲区 */
    uint8_t q_head;                         /**< 队列头索引（读取位置） */
    uint8_t q_tail;                         /**< 队列尾索引（写入位置） */
    lv_timer_t *pump_timer;                 /**< LVGL 定时器：周期性泵送事件 */
} app_ui_ctx_t;

/** 全局单例上下文实例 */
static app_ui_ctx_t s_ui;

/**
 * @brief 向事件循环队列压入一个项目
 *
 * 使用循环缓冲区，通过预留一个空位区分满和空。
 * 线程安全：操作在临界区保护下执行。
 *
 * @param item 要入队的项
 * @return true 成功，false 队列已满或参数无效
 */
static bool queue_push(const app_ui_queue_item_t *item)
{
    uint8_t next_tail;
    bool pushed = false;

    if(item == NULL) {
        return false;
    }

    App_UiPort_EnterCritical();
    next_tail = (uint8_t)((s_ui.q_tail + 1u) % APP_UI_QUEUE_LEN);
    if(next_tail != s_ui.q_head) {
        s_ui.queue[s_ui.q_tail] = *item;
        s_ui.q_tail = next_tail;
        pushed = true;
    }
    App_UiPort_ExitCritical();
    return pushed;
}

/**
 * @brief 从事件循环队列弹出一个项目
 * @param item [out] 接收弹出的项
 * @return true 成功，false 队列为空或参数无效
 */
static bool queue_pop(app_ui_queue_item_t *item)
{
    bool popped = false;

    if(item == NULL) {
        return false;
    }

    App_UiPort_EnterCritical();
    if(s_ui.q_head != s_ui.q_tail) {
        *item = s_ui.queue[s_ui.q_head];
        s_ui.q_head = (uint8_t)((s_ui.q_head + 1u) % APP_UI_QUEUE_LEN);
        popped = true;
    }
    App_UiPort_ExitCritical();
    return popped;
}

/**
 * @brief 显示当前导航指向的页面
 *
 * @param full_refresh 是否执行全屏刷新
 * @return true 成功
 */
static bool show_current_page(bool full_refresh,
                              app_ui_page_transition_t transition)
{
    const app_ui_page_t *page = App_UiPages_Get(App_UiNav_Current(&s_ui.nav));

    if(page == NULL) {
        return false;
    }

    App_UiModel_SetCurrentPage(&s_ui.model, page->id);
    App_UiView_ShowPage(&s_ui.view,
                        page,
                        &s_ui.model,
                        App_UiNav_CanBack(&s_ui.nav),
                        transition);
    s_ui.model.dirty_mask &= ~APP_UI_DIRTY_NAV;
    App_UiPort_RequestFlush(full_refresh);
    return true;
}

/**
 * @brief UI 导航动作调度器：将 Action 层请求转换为内部队列消息
 *
 * 通过 app_action_register_ui_dispatcher() 注册为回调。
 * 外部调用 app_action_submit() 提交 UI 导航请求时由此函数处理。
 */
static esp_err_t ui_action_dispatcher(const app_action_request_t *request, void *user_data)
{
    app_ui_queue_item_t item;
    app_ui_page_id_t page_id;
    (void)user_data;

    if(request == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if(request->id != APP_ACTION_ID_UI_NAV_BACK &&
       request->id != APP_ACTION_ID_UI_NAV_HOME &&
       request->id != APP_ACTION_ID_UI_NAV_PUSH) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    page_id = (app_ui_page_id_t)request->params.ui_navigation.page_id;
    if(request->id == APP_ACTION_ID_UI_NAV_PUSH && App_UiPages_Get(page_id) == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(&item, 0, sizeof(item));
    item.type = APP_UI_QUEUE_NAVIGATION;
    item.data.navigation.action_id = request->id;
    item.data.navigation.page_id = page_id;
    return queue_push(&item) ? ESP_OK : ESP_ERR_TIMEOUT;
}

/**
 * @brief 处理导航队列项，执行实际的页面跳转
 * @param item 队列中的导航项
 */
static void process_navigation(const app_ui_queue_item_t *item)
{
    bool changed = false;
    app_ui_page_transition_t transition = APP_UI_PAGE_TRANSITION_INITIAL;

    if(item == NULL) {
        return;
    }

    switch(item->data.navigation.action_id) {
    case APP_ACTION_ID_UI_NAV_BACK:
        changed = App_UiNav_Back(&s_ui.nav);
        transition = APP_UI_PAGE_TRANSITION_BACK;
        break;
    case APP_ACTION_ID_UI_NAV_HOME:
        changed = App_UiNav_Current(&s_ui.nav) != APP_UI_PAGE_HOME ||
                  App_UiNav_CanBack(&s_ui.nav);
        App_UiNav_Home(&s_ui.nav, APP_UI_PAGE_HOME);
        transition = APP_UI_PAGE_TRANSITION_HOME;
        break;
    case APP_ACTION_ID_UI_NAV_PUSH:
        if(App_UiPages_Get(item->data.navigation.page_id) != NULL &&
           item->data.navigation.page_id != App_UiNav_Current(&s_ui.nav)) {
            changed = App_UiNav_Push(&s_ui.nav, item->data.navigation.page_id);
            transition = APP_UI_PAGE_TRANSITION_PUSH;
        }
        break;
    default:
        break;
    }

    if(changed) {
        (void)show_current_page(true, transition);
    }
}

/**
 * @brief LVGL 定时器回调：事件泵送循环的核心
 *
 * 每 50ms 触发一次，执行：
 *   1. 批量处理事件队列（最多 8 个）
 *   2. 检查 dirty_mask 并刷新视图
 *   3. 检查系统消息并显示 Toast
 */
static void ui_pump_timer_cb(lv_timer_t *timer)
{
    app_ui_queue_item_t item;
    const app_ui_page_t *page;
    uint8_t processed = 0u;
    uint32_t refresh_mask;
    (void)timer;

    /* ---- 步骤1：批量处理事件队列 ---- */
    while(processed < APP_UI_PUMP_BATCH_LEN && queue_pop(&item)) {
        if(item.type == APP_UI_QUEUE_MODEL_EVENT) {
            App_UiModel_ApplyEvent(&s_ui.model, &item.data.event);
        } else if(item.type == APP_UI_QUEUE_NAVIGATION) {
            process_navigation(&item);
        }
        processed++;
    }

    /* ---- 步骤2：选择性刷新视图 ---- */
    page = App_UiPages_Get(App_UiNav_Current(&s_ui.nav));
    if(page == NULL) {
        return;
    }

    refresh_mask = s_ui.model.dirty_mask &
                   (page->dirty_mask | APP_UI_DIRTY_STATUS);
    if(refresh_mask != 0u) {
        App_UiView_Refresh(&s_ui.view, &s_ui.model, refresh_mask);
        s_ui.model.dirty_mask &= ~refresh_mask;
        App_UiPort_RequestFlush(false);
    }

    /* ---- 步骤3：处理系统 Toast 消息 ---- */
    if((s_ui.model.dirty_mask & APP_UI_DIRTY_SYSTEM) != 0u) {
        App_UiView_ShowToast(&s_ui.view, s_ui.model.message);
        s_ui.model.dirty_mask &= ~APP_UI_DIRTY_SYSTEM;
        App_UiPort_RequestFlush(false);
    }
}

bool App_UiInit(void)
{
    /* 清零全局上下文 */
    memset(&s_ui, 0, sizeof(s_ui));

    /* 初始化 MVC 核心模块 */
    App_UiModel_Init(&s_ui.model);
    App_UiNav_Init(&s_ui.nav, APP_UI_PAGE_HOME);

    /* 初始化平台层和 Action 系统 */
    if(!App_UiPort_Init() || app_action_init() != ESP_OK) {
        return false;
    }

    /* 初始化视图层，注册 UI 调度器 */
    App_UiView_Init(&s_ui.view);
    if(app_action_register_ui_dispatcher(ui_action_dispatcher, NULL) != ESP_OK) {
        return false;
    }

    /* 创建事件泵送定时器（50ms 周期） */
    s_ui.pump_timer = lv_timer_create(ui_pump_timer_cb, 50, NULL);
    if(s_ui.pump_timer == NULL) {
        return false;
    }

    s_ui.ready = true;
    return true;
}

bool App_UiStart(void)
{
    if(!s_ui.ready) {
        return false;
    }
    if(s_ui.started) {
        return true;
    }

    if(!show_current_page(true, APP_UI_PAGE_TRANSITION_INITIAL)) {
        return false;
    }

    s_ui.started = true;
    return true;
}

bool App_UiPostEvent(const app_ui_event_t *event)
{
    app_ui_queue_item_t item;

    /* 参数校验 */
    if(!s_ui.ready || event == NULL || event->type <= APP_UI_EVENT_NONE ||
       event->type >= APP_UI_EVENT_COUNT) {
        return false;
    }

    /* 包装为内部队列项并入队 */
    memset(&item, 0, sizeof(item));
    item.type = APP_UI_QUEUE_MODEL_EVENT;
    item.data.event = *event;
    return queue_push(&item);
}

bool App_UiShowMessage(const char *message)
{
    app_ui_event_t event;

    /* 构造 SHOW_MESSAGE 事件并投递 */
    memset(&event, 0, sizeof(event));
    event.type = APP_UI_EVENT_SHOW_MESSAGE;
    if(message != NULL) {
        snprintf(event.text, sizeof(event.text), "%s", message);
    }
    return App_UiPostEvent(&event);
}

bool App_UiIsReady(void)
{
    return s_ui.ready;
}
