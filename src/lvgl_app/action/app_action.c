/**
 * @file app_action.c
 * @brief Action 系统门面层实现 —— 参数校验并转发到底层 task_action。
 *
 * 【设计模式】门面模式（Facade Pattern）
 * 此文件作为 Action 系统的对外接口层，位于调用方和底层实现之间：
 *
 *   调用方（如 UI 组件、按钮回调）
 *       │
 *       ▼
 *   app_action_submit()  ←── 此文件（门面层）
 *       │                    - 参数合法性校验
 *       │                    - 动作 ID 范围检查
 *       ▼
 *   task_action_submit()  ←── 底层实现（task_action.c）
 *                            - 分配任务 ID
 *                            - 调用 UI 调度器执行
 *                            - 记录快照到环形缓冲区
 *
 * 【当前限制】
 * 当前仅支持 UI 导航动作（Back/Home/Push），
 * 若传入其他动作 ID，参数校验阶段会被拦截。
 */

#include "app_action.h"

#include <stddef.h>

#include "task_action.h"

/**
 * @brief 初始化 Action 系统
 *
 * 委托给底层 task_action_init() 执行初始化操作：
 *   - 清零历史任务环形缓冲区
 *   - 重置任务 ID 计数器为 1
 *   - 清除 UI 调度器指针
 *
 * 必须在调用 app_action_submit() 之前调用此函数。
 *
 * @return ESP_OK 初始化成功
 */
esp_err_t app_action_init(void)
{
    return task_action_init();
}

/**
 * @brief 注册 UI 调度器回调函数
 *
 * 注册的回调将在每次 app_action_submit() 调用时被执行，
 * 用于将动作请求转发到 UI 控制器（App_Ui.c 中的 ui_action_dispatcher）。
 *
 * 通常在 App_UiInit() 中调用，系统初始化期间注册一次即可。
 *
 * @param dispatcher  调度器函数指针（不可为 NULL）
 * @param user_data   用户自定义数据，原样传递给调度器回调
 * @return ESP_OK                注册成功
 * @return ESP_ERR_INVALID_ARG   dispatcher 为 NULL
 */
esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                             void *user_data)
{
    return task_action_register_ui_dispatcher(dispatcher, user_data);
}

/**
 * @brief 提交一个动作请求到 Action 系统执行
 *
 * 执行流程：
 *   1. 【第一层校验】检查指针是否为 NULL，动作 ID 是否在合法范围内
 *   2. 【第二层校验】Push 动作必须指定有效的目标页面 ID（不能为 0）
 *   3. 【执行】校验通过后委托给 task_action_submit() 处理：
 *      - 分配自增任务 ID（job_id）
 *      - 在环形缓冲区中创建任务快照
 *      - 同步调用 UI 调度器执行动作
 *      - 更新执行结果和状态到快照中
 *
 * @param request  动作请求指针（含动作 ID 和参数）
 * @param job_id   [out] 返回分配的唯一任务 ID
 * @return ESP_OK              执行成功
 * @return ESP_ERR_INVALID_ARG     参数无效（NULL / ID 越界 / Push 缺 page_id）
 * @return ESP_ERR_NOT_SUPPORTED   不支持的导航动作类型
 * @return ESP_ERR_TIMEOUT         事件队列已满
 */
esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id)
{
    /* ---- 第一层：检查指针和动作 ID 范围 ----
     * request->id 必须在 (APP_ACTION_ID_INVALID, APP_ACTION_ID_MAX) 开区间内，
     * 即不能等于 APP_ACTION_ID_INVALID(0) 或 APP_ACTION_ID_MAX
     */
    if(request == NULL || job_id == NULL ||
       request->id <= APP_ACTION_ID_INVALID || request->id >= APP_ACTION_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    /* ---- 第二层：Push 动作必须指定有效的目标页面 ID ----
     * page_id == 0 对应 APP_UI_PAGE_NONE，是无效页面，不允许导航到该页面
     */
    if(request->id == APP_ACTION_ID_UI_NAV_PUSH &&
       request->params.ui_navigation.page_id == 0u) {
        return ESP_ERR_INVALID_ARG;
    }

    /* ---- 第三层：校验通过，委托底层执行 ---- */
    return task_action_submit(request, job_id);
}

/**
 * @brief 根据任务 ID 查询历史任务快照
 *
 * 在底层 task_action 的环形缓冲区中查找指定 job_id 的任务记录。
 * 历史缓冲区最多保留最近 16 条记录，超过的记录会被覆盖。
 *
 * @param job_id    要查询的任务 ID（不能为 0）
 * @param snapshot  [out] 接收任务快照的缓冲区
 * @return ESP_OK                查询成功
 * @return ESP_ERR_INVALID_ARG   参数无效（job_id == 0 或 snapshot == NULL）
 * @return ESP_ERR_NOT_FOUND     未找到指定 job_id 的任务记录
 */
esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot)
{
    return task_action_get_job(job_id, snapshot);
}

/**
 * @brief 获取最近一次提交的任务快照
 *
 * 查询最近一次通过 app_action_submit() 提交的任务记录。
 * 如果自系统初始化以来从未提交过任何任务，返回错误。
 *
 * @param snapshot  [out] 接收最新任务快照的缓冲区
 * @return ESP_OK                查询成功
 * @return ESP_ERR_INVALID_ARG   参数无效或尚未提交过任务
 */
esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot)
{
    return task_action_get_latest(snapshot);
}

/**
 * @brief 将动作 ID 枚举值转换为人类可读的字符串名称
 *
 * 用于日志输出和调试场景。返回的字符串为静态常量，
 * 调用者无需也不应尝试释放该内存。
 *
 * @param id  动作 ID 枚举值
 * @return    动作名称字符串（如 "ui_nav_back"）
 *             未知或无效 ID 返回 "invalid"
 */
const char *app_action_id_to_name(app_action_id_t id)
{
    switch(id) {
    case APP_ACTION_ID_UI_NAV_BACK: return "ui_nav_back";
    case APP_ACTION_ID_UI_NAV_HOME: return "ui_nav_home";
    case APP_ACTION_ID_UI_NAV_PUSH: return "ui_nav_push";
    default: return "invalid";
    }
}
