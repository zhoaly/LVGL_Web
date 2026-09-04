#include <assert.h>

#include "App_UiCommand.h"

static unsigned int s_submit_count;
static app_ui_command_t s_last_command;

static bool submitter(const app_ui_command_t *command, void *user_data)
{
    unsigned int *count = user_data;
    assert(command != NULL);
    (*count)++;
    s_last_command = *command;
    return true;
}

int main(void)
{
    app_ui_command_t command = {
        .id = APP_UI_COMMAND_NAV_PUSH,
        .page_id = 3u,
    };

    assert(!App_UiCommand_Submit(&command));
    assert(App_UiCommand_SetSubmitter(submitter, &s_submit_count));
    assert(App_UiCommand_Submit(&command));
    assert(s_submit_count == 1u);
    assert(s_last_command.id == APP_UI_COMMAND_NAV_PUSH);
    assert(s_last_command.page_id == 3u);

    command = (app_ui_command_t){.id = APP_UI_COMMAND_MENU_OPEN};
    assert(App_UiCommand_Submit(&command));
    assert(s_submit_count == 2u);
    assert(s_last_command.id == APP_UI_COMMAND_MENU_OPEN);

    command = (app_ui_command_t){.id = APP_UI_COMMAND_MENU_CLOSE};
    assert(App_UiCommand_Submit(&command));
    assert(s_submit_count == 3u);
    assert(s_last_command.id == APP_UI_COMMAND_MENU_CLOSE);

    command.page_id = 1u;
    assert(!App_UiCommand_Submit(&command));

    command.id = APP_UI_COMMAND_NAV_PUSH;
    command.page_id = 0u;
    assert(!App_UiCommand_Submit(&command));
    command.id = APP_UI_COMMAND_INVALID;
    assert(!App_UiCommand_Submit(&command));
    command = (app_ui_command_t){.id=APP_UI_COMMAND_WIFI_CONNECT_PROFILE};
    assert(!App_UiCommand_Submit(&command));
    command.operation_token = 1;
    assert(!App_UiCommand_Submit(&command));
    command.profile_id = 42;
    assert(App_UiCommand_Submit(&command));
    assert(s_last_command.profile_id == 42 && s_last_command.operation_token == 1);
    assert(!App_UiCommand_SetSubmitter(NULL, NULL));
    return 0;
}
