/**
 * @file App_UiCommand.c
 * @brief UI 命令提交门面。
 */

#include "App_UiCommand.h"

#include <stddef.h>

static app_ui_command_submitter_fn s_submitter;
static void *s_submitter_user_data;

bool App_UiCommand_SetSubmitter(app_ui_command_submitter_fn submitter,
                                void *user_data)
{
    s_submitter = submitter;
    s_submitter_user_data = submitter != NULL ? user_data : NULL;
    return submitter != NULL;
}

bool App_UiCommand_Submit(const app_ui_command_t *command)
{
    if(command == NULL || command->id <= APP_UI_COMMAND_INVALID ||
       command->id >= APP_UI_COMMAND_COUNT || s_submitter == NULL) {
        return false;
    }
    if(command->id == APP_UI_COMMAND_NAV_PUSH && command->page_id == 0u) {
        return false;
    }
    return s_submitter(command, s_submitter_user_data);
}
