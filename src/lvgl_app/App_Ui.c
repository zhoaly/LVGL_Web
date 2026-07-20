/**
 * @file App_Ui.c
 * @brief UI 应用主控制器 —— 实现事件循环、导航调度和 UI 生命周期管理。
 *
 * 【架构位置】
 * 此文件是 MVC 架构中的 Controller 部分，负责：
 *   1. 初始化所有模块（Model/View/Nav/Port）
 *   2. 维护事件队列和泵送（pump）循环
 *   3. 调度导航请求和 Model 事件
 *   4. 协调视图刷新
 *
 * 【事件处理流程】
 * 外部事件 -> App_UiPostEvent() -> 队列 push -> ui_pump_timer_cb()
 *   -> queue_pop() -> 类型判断
 *      - MODEL_EVENT: 调用 App_UiModel_ApplyEvent() 更新数据
 *      - NAVIGATION: 调用 process_navigation() 处理页面跳转
 *   -> 检查 dirty_mask -> 调用 App_UiView_Refresh() 刷新视图
 *   -> 检查系统消息 -> 调用 App_UiView_ShowToast() 显示 Toast
 */

#include "include/App_Ui.h"

#include <stdio.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "app_action.h"

#include "App_UiModel.h"
#include "App_UiNav.h"
#include "App_UiPort.h"
#include "App_UiView.h"
#include "pages/App_UiPages.h"

/**
 * @brief 内部事件队列长度
 * 循环缓冲区大小，必须为 2 的幂或预留一个空位用于判满。
 * 当前使用预留空位法（tail+1 == head 表示满）。
 */
#define APP_UI_QUEUE_LEN 24u

/**
 * @brief 内部事件队列项类型枚举
 */
typedef enum {
    APP_UI_QUEUE_MODEL_EVENT = 0,   /**< Model 数据更新事件 */
    APP_UI_QUEUE_NAVIGATION,        /**< 页面导航请求 */
} app_ui_queue_item_type_t;

/**
 * @brief 内部事件队列项结构体
 * 使用 tagged union（标签联合体）区分不同数据类型。
 */
typedef struct {
    app_ui_queue_item_type_t type;  /**< 队列项类型 */
    union {
        app_ui_event_t event;                   /**< Model 事件 */
        struct {
            app_action_id_t action_id;          /**< 导航动作 ID */
            app_ui_page_id_t page_id;           /**< 目标页面 ID */
        } navigation;                           /**< 导航数据 */
    } data;
} app_ui_queue_item_t;

/**
 * @brief UI 全局上下文结构体
 *
 * 单例模式，所有状态集中管理。
 * 包含就绪标志、启动标志、Model/Nav/View 三大模块、
 * 事件循环队列和 LVGL 定时器。
 */
typedef struct {
    bool ready;                             /**< 初始化完成标志 */
    bool started;                           /**< 启动完成标志 */
    app_ui_model_t model;                   /**< 数据模型（状态缓存） */
    app_ui_nav_t nav;                       /**< 导航管理器（页面栈） */
    app_ui_view_t view;                     /**< 视图层（LVGL 对象树） */
    app_ui_queue_item_t queue[APP_UI_QUEUE_LEN]; /**< 事件循环缓冲区 */
    uint8_t q_head;                         /**< 队列头索引（读取位置） */
    uint8_t q_tail;                         /**< 队列尾索引（写入位置） */
    lv_timer_t *pump_timer;                 /**< LVGL 定时器：周期性泵送事件 */
} app_ui_ctx_t;

/** 全局单例上下文实例 */
static app_ui_ctx_t s_ui;

/**
 * @brief 向事件循环队列中压入一个项目
 *
 * 使用循环缓冲区(circular buffer)实现，通过预留一个空位区分满和空：
 *   - 队空条件：q_head == q_tail
 *   - 队满条件：(q_tail + 1) % QUEUE_LEN == q_head
 *
 * 线程安全：操作在临界区保护下执行。
 *
 * @param item 要入队的队列项指针
 * @return true  入队成功
 * @return false 队列已满
 */
static bool queue_push(const app_ui_queue_item_t *item)
{
    uint8_t next_tail;
    bool pushed = false;

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
 * @brief 从事件循环队列中弹出一个项目
 *
 * @param item [out] 接收弹出的队列项
 * @return true  弹出成功
 * @return false 队列为空
 */
static bool queue_pop(app_ui_queue_item_t *item)
{
    bool popped = false;

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
 * @brief 模拟事件适配器：将 Port 层的事件发射函数转换为 App_UiPostEvent 调用
 *
 * 此函数作为回调传递给 App_UiPort_StartSimulation()，
 * 使得 PC 端的模拟事件发生器可以投递事件到 UI 队列。
 *
 * @param event 要投递的事件
 * @param user_data 用户数据（未使用）
 * @return true 投递成功
 */
static bool emit_adapter(const app_ui_event_t *event, void *user_data)
{
    (void)user_data;
    return App_UiPostEvent(event);
}

/**
 * @brief 显示当前导航指向的页面
 *
 * 执行流程：
 *   1. 根据 Nav 中的 current 页面 ID 获取页面描述符
 *   2. 更新 Model 中的当前页面记录并标记 dirty
 *   3. 调用 View 层构建页面 UI
 *   4. 清除导航 dirty 标志
 *   5. 请求平台层刷新显示
 *
 * @param full_refresh 是否执行全屏刷新（墨水屏场景下清除残影）
 */
static void show_current_page(bool full_refresh)
{
    const app_ui_page_t *page = App_UiPages_Get(App_UiNav_Current(&s_ui.nav));
    if(page == NULL) {
        return;
    }

    App_UiModel_SetCurrentPage(&s_ui.model, page->id);
    App_UiView_ShowPage(&s_ui.view, page, &s_ui.model, App_UiNav_CanBack(&s_ui.nav));
    s_ui.model.dirty_mask &= ~APP_UI_DIRTY_NAV;
    App_UiPort_RequestFlush(full_refresh);
}

/**
 * @brief UI 导航动作调度器：将 Action 层的导航请求转换为内部队列消息
 *
 * 此函数通过 app_action_register_ui_dispatcher() 注册为 Action 层的回调。
 * 当外部模块调用 app_action_submit() 提交 UI 导航请求时，由此函数处理。
 *
 * 支持的导航动作：
 *   - APP_ACTION_ID_UI_NAV_BACK：返回上一页
 *   - APP_ACTION_ID_UI_NAV_HOME：回到首页
 *   - APP_ACTION_ID_UI_NAV_PUSH：推入新页面
 *
 * @param request Action 请求指针
 * @param user_data 用户数据（未使用）
 * @return ESP_OK 成功入队
 * @return ESP_ERR_INVALID_ARG 参数无效
 * @return ESP_ERR_NOT_SUPPORTED 不支持的导航动作
 * @return ESP_ERR_TIMEOUT 队列已满
 */
static esp_err_t ui_action_dispatcher(const app_action_request_t *request, void *user_data)
{
    app_ui_queue_item_t item;
    (void)user_data;

    if(request == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if(request->id != APP_ACTION_ID_UI_NAV_BACK &&
       request->id != APP_ACTION_ID_UI_NAV_HOME &&
       request->id != APP_ACTION_ID_UI_NAV_PUSH) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    memset(&item, 0, sizeof(item));
    item.type = APP_UI_QUEUE_NAVIGATION;
    item.data.navigation.action_id = request->id;
    item.data.navigation.page_id = (app_ui_page_id_t)request->params.ui_navigation.page_id;

    if(request->id == APP_ACTION_ID_UI_NAV_PUSH &&
       App_UiPages_Get(item.data.navigation.page_id) == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return queue_push(&item) ? ESP_OK : ESP_ERR_TIMEOUT;
}

/**
 * @brief 处理导航队列项，执行实际的页面跳转
 *
 * 根据导航动作类型调用 Nav 层对应的 API：
 *   - Back：返回到栈中上一级页面
 *   - Home：清空导航栈回到首页
 *   - Push：将新页面压栈并跳转
 *
 * 执行成功后调用 show_current_page() 刷新视图。
 *
 * @param item 队列中的导航项
 */
static void process_navigation(const app_ui_queue_item_t *item)
{
    bool changed = false;
    switch(item->data.navigation.action_id) {
    case APP_ACTION_ID_UI_NAV_BACK:
        changed = App_UiNav_Back(&s_ui.nav);
        break;
    case APP_ACTION_ID_UI_NAV_HOME:
        changed = App_UiNav_Current(&s_ui.nav) != APP_UI_PAGE_HOME || App_UiNav_CanBack(&s_ui.nav);
        App_UiNav_Home(&s_ui.nav, APP_UI_PAGE_HOME);
        break;
    case APP_ACTION_ID_UI_NAV_PUSH:
        if(App_UiPages_Get(item->data.navigation.page_id) != NULL) {
            changed = App_UiNav_Push(&s_ui.nav, item->data.navigation.page_id);
        }
        break;
    default:
        break;
    }
    if(changed) {
        show_current_page(true);
    }
}

/**
 * @brief LVGL 定时器回调：事件泵送循环的核心函数
 *
 * 每 50ms 由 LVGL 定时器触发一次，执行以下步骤：
 *
 * 1. 【事件处理】从队列中批量弹出事件（最多 8 个/周期）
 *    - MODEL_EVENT：调用 App_UiModel_ApplyEvent() 更新数据
 *    - NAVIGATION：调用 process_navigation() 处理页面跳转
 *
 * 2. 【视图刷新】检查当前页面的 dirty_mask 是否与 Model 的 dirty_mask 有交集
 *    - 有交集：调用 App_UiView_Refresh() 更新页面内的 Label 值
 *    - 清除已处理的 dirty 标志
 *
 * 3. 【Toast 显示】如果系统消息非空，调用 App_UiView_ShowToast()
 *
 * @param timer 触发此回调的 LVGL 定时器指针
 */
static void ui_pump_timer_cb(lv_timer_t *timer)
{
    app_ui_queue_item_t item;
    const app_ui_page_t *page;
    uint8_t processed = 0;
    uint32_t refresh_mask;
    (void)timer;

    /* ---- 步骤1：批量处理事件队列 ---- */
    while(processed < 8u && queue_pop(&item)) {
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
    refresh_mask = s_ui.model.dirty_mask & page->dirty_mask;
    if(refresh_mask != 0u) {
        App_UiView_Refresh(&s_ui.view, &s_ui.model);
        s_ui.model.dirty_mask &= ~refresh_mask;
        App_UiPort_RequestFlush(false);
    }

    /* ---- 步骤3：处理系统 Toast 消息 ---- */
    if(s_ui.model.message[0] != '\0' &&
       (s_ui.model.dirty_mask & APP_UI_DIRTY_SYSTEM) != 0u) {
        App_UiView_ShowToast(&s_ui.view, s_ui.model.message);
        s_ui.model.dirty_mask &= ~APP_UI_DIRTY_SYSTEM;
    }
}

bool App_UiInit(void)
{
    /* 清零全局上下文 */
    memset(&s_ui, 0, sizeof(s_ui));

    /* 初始化 MVC 三大核心模块 */
    App_UiModel_Init(&s_ui.model);     /* 数据模型：加载默认值 */
    App_UiNav_Init(&s_ui.nav, APP_UI_PAGE_HOME); /* 导航：首页 */

    /* 初始化平台适配层和 Action 系统 */
    if(!App_UiPort_Init() || app_action_init() != ESP_OK) {
        return false;
    }

    /* 初始化视图层（创建 LVGL 对象树） */
    App_UiView_Init(&s_ui.view);

    /* 创建事件泵送定时器（50ms 周期） */
    s_ui.pump_timer = lv_timer_create(ui_pump_timer_cb, 50, NULL);
    if(s_ui.pump_timer == NULL ||
       app_action_register_ui_dispatcher(ui_action_dispatcher, NULL) != ESP_OK) {
        return false;
    }

    s_ui.ready = true;
    return true;
}

bool App_UiStart(void)
{
    /* 检查系统就绪状态 */
    if(!s_ui.ready) {
        return false;
    }
    /* 防止重复启动 */
    if(s_ui.started) {
        return true;
    }

    /* 显示首页（全屏刷新） */
    show_current_page(true);

    /*
     * 启动平台模拟事件发生器（仅 PC 端有效）。
     * PC 端会定期注入模拟的心跳和堆变化事件，
     * 以便在没有真实硬件的情况下测试 UI。
     * ESP32 端此函数为空操作。
     */
    App_UiPort_StartSimulation(emit_adapter, NULL);

    s_ui.started = true;
    return true;
}

bool App_UiPostEvent(const app_ui_event_t *event)
{
    app_ui_queue_item_t item;

    /* 参数校验 */
    if(!s_ui.ready || event == NULL) {
        return false;
    }

    /* 将外部事件包装为内部队列项并入队 */
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
    /* 返回系统是否已完成初始化 */
    return s_ui.ready;
}
