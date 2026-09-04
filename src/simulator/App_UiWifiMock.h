#pragma once
#include "../lvgl_app/command/App_UiCommand.h"
bool App_UiWifiMock_Bind(app_ui_command_submitter_fn dispatcher, void *user_data);
int App_UiWifiMock_SetStatus(int state);
