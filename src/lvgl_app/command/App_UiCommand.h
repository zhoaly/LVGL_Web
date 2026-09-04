/**
 * @file App_UiCommand.h
 * @brief 平台无关的 UI 命令出口。
 */

#ifndef APP_UI_COMMAND_H
#define APP_UI_COMMAND_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    APP_UI_COMMAND_INVALID = 0,
    APP_UI_COMMAND_NAV_BACK,
    APP_UI_COMMAND_NAV_HOME,
    APP_UI_COMMAND_NAV_PUSH,
    APP_UI_COMMAND_MENU_OPEN,
    APP_UI_COMMAND_MENU_CLOSE,
    APP_UI_COMMAND_WIFI_SET_ENABLED,
    APP_UI_COMMAND_WIFI_SCAN,
    APP_UI_COMMAND_WIFI_CONNECT_PROFILE,
    APP_UI_COMMAND_WIFI_DISCONNECT,
    APP_UI_COMMAND_WIFI_FORGET_PROFILE,
    APP_UI_COMMAND_WIFI_SET_AUTO_CONNECT,
    APP_UI_COMMAND_WIFI_SET_PROFILE_AUTO_JOIN,
    APP_UI_COMMAND_COUNT,
} app_ui_command_id_t;

typedef struct {
    app_ui_command_id_t id;
    uint32_t page_id;
    uint32_t operation_token;
    uint32_t profile_id;
    bool enabled;
} app_ui_command_t;

typedef bool (*app_ui_command_submitter_fn)(const app_ui_command_t *command,
                                             void *user_data);

/** 注册平台命令提交后端；重复调用会替换现有后端。 */
bool App_UiCommand_SetSubmitter(app_ui_command_submitter_fn submitter,
                                void *user_data);

/** 提交控件产生的 UI 命令。 */
bool App_UiCommand_Submit(const app_ui_command_t *command);

#endif /* APP_UI_COMMAND_H */
