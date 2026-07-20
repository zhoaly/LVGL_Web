/**
 * @file App_Ui.h
 * @brief UI 应用层主头文件 —— 定义公共类型、事件结构和顶层 API。
 *
 * 【架构位置】
 * 此文件位于 MVC 架构的最顶层，所有模块（Model/View/Nav/Port/Pages）
 * 都直接或间接引用此文件中定义的基础类型。
 *
 * 【职责范围】
 *   - 页面 ID 枚举：定义所有支持的页面
 *   - Dirty 标志位：按位标记哪些业务域需要刷新
 *   - 事件类型枚举和结构体：UI 内部通信的事件格式
 *   - 顶层 API 函数：初始化、启动、事件投递
 */

#ifndef APP_UI_H
#define APP_UI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 页面 ID 枚举
 *
 * 定义系统中所有可导航的页面。
 * 新增页面时在此添加枚举值，并在 pages/ 目录下实现对应的 Get 函数。
 */
typedef enum {
    APP_UI_PAGE_NONE = 0,       /**< 无效页面，用于初始化和错误状态 */
    APP_UI_PAGE_HOME,           /**< 首页：显示 USB HUB/WiFi/Runtime 概览卡片 */
    APP_UI_PAGE_USB_STATUS,     /**< USB 状态详情页：显示模式、HUB 使能、设备数 */
    APP_UI_PAGE_RUNTIME,        /**< 运行时信息页：显示运行时间、堆内存、任务数 */
} app_ui_page_id_t;

/**
 * @brief Dirty 标志位枚举
 *
 * 按位标记哪些业务域发生了数据变更，视图层据此决定哪些区域需要刷新。
 * 使用位掩码（bitmask）设计，支持多个域同时标记。
 */
typedef enum {
    APP_UI_DIRTY_NONE = 0,              /**< 无变更 */
    APP_UI_DIRTY_SYSTEM = (1u << 0),    /**< 系统消息/Toast 变更 */
    APP_UI_DIRTY_RUNTIME = (1u << 1),   /**< 运行时数据变更（堆内存/运行时间等） */
    APP_UI_DIRTY_USB = (1u << 2),       /**< USB 状态变更 */
    APP_UI_DIRTY_WIFI = (1u << 3),      /**< WiFi 状态变更 */
    APP_UI_DIRTY_GPIO = (1u << 4),      /**< GPIO 状态变更（预留） */
    APP_UI_DIRTY_NAV = (1u << 5),       /**< 导航变更（页面切换） */
    APP_UI_DIRTY_ALL = 0xFFFFFFFFu,     /**< 全量刷新标志 */
} app_ui_dirty_mask_t;

/**
 * @brief UI 事件类型枚举
 *
 * 定义 Model 层可处理的所有事件类型。
 * 外部业务模块通过 App_UiPostEvent() 投递这些事件。
 */
typedef enum {
    APP_UI_EVENT_NONE = 0,              /**< 空事件 */
    APP_UI_EVENT_RUNTIME_TICK,          /**< 运行时心跳：每秒递增运行时间 */
    APP_UI_EVENT_USB_MODE_CHANGED,      /**< USB 模式变更：a=新模式 */
    APP_UI_EVENT_USB_HUB_CHANGED,       /**< USB HUB 状态变更：a=2 表示切换，0/1 表示设置 */
    APP_UI_EVENT_WIFI_STATE_CHANGED,    /**< WiFi 状态变更：a=状态值，b=RSSI */
    APP_UI_EVENT_HEAP_UPDATED,          /**< 堆内存更新：a=空闲堆, b=最小堆 */
    APP_UI_EVENT_SHOW_MESSAGE,          /**< 显示 Toast 消息：text=消息内容 */
} app_ui_event_type_t;

/**
 * @brief UI 事件结构体
 *
 * 使用联合体风格的设计，通过 type 字段区分事件类型，
 * a/b 存放整型参数，text 存放字符串参数。
 */
typedef struct {
    app_ui_event_type_t type;   /**< 事件类型 */
    int32_t a;                  /**< 参数 a：含义因 type 而异 */
    int32_t b;                  /**< 参数 b：含义因 type 而异 */
    char text[48];              /**< 文本参数：用于消息等场景 */
} app_ui_event_t;

/* ======================== 顶层 API 函数声明 ======================== */

/**
 * @brief 初始化 UI 系统
 *
 * 依次初始化以下模块：
 *   - Model：加载默认初始值
 *   - Nav：设置首页为 APP_UI_PAGE_HOME
 *   - Port：初始化平台相关资源（显示/输入）
 *   - Action：注册 UI 调度器
 *   - View：创建主界面布局
 *
 * @return true  初始化成功
 * @return false 初始化失败（平台初始化或 Action 注册失败）
 */
bool App_UiInit(void);

/**
 * @brief 启动 UI 系统
 *
 * 显示当前页面，并启动平台相关的事件模拟（PC 端）。
 * 必须在 App_UiInit() 成功返回后调用。
 *
 * @return true  启动成功
 * @return false 系统未就绪
 */
bool App_UiStart(void);

/**
 * @brief 投递一个事件到 UI 事件队列
 *
 * 线程安全：内部使用临界区保护队列操作。
 * 事件队列为循环缓冲区，满时返回 false。
 *
 * @param event 要投递的事件指针
 * @return true  投递成功
 * @return false 队列已满或参数无效
 */
bool App_UiPostEvent(const app_ui_event_t *event);

/**
 * @brief 快捷投递一条 Toast 消息事件
 *
 * 内部构造 APP_UI_EVENT_SHOW_MESSAGE 事件并投递。
 *
 * @param message 消息字符串（最长 47 字符）
 * @return true  投递成功
 * @return false 投递失败
 */
bool App_UiShowMessage(const char *message);

/**
 * @brief 检查 UI 系统是否已经初始化完毕
 * @return true  已就绪
 * @return false 未就绪
 */
bool App_UiIsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_UI_H */
