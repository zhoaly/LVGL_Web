#ifndef APP_UI_PORT_H
#define APP_UI_PORT_H

#include <stdbool.h>

bool App_UiPort_Init(void);
void App_UiPort_RequestFlush(bool full_refresh);
void App_UiPort_EnterCritical(void);
void App_UiPort_ExitCritical(void);

#endif /* APP_UI_PORT_H */
