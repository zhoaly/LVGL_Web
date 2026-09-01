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

    command.page_id = 0u;
    assert(!App_UiCommand_Submit(&command));
    command.id = APP_UI_COMMAND_INVALID;
    assert(!App_UiCommand_Submit(&command));
    assert(!App_UiCommand_SetSubmitter(NULL, NULL));
    return 0;
}
