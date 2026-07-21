#include "../App_UiPort.h"

bool App_UiPort_Init(void)
{
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    (void)full_refresh;
}

void App_UiPort_EnterCritical(void)
{
}

void App_UiPort_ExitCritical(void)
{
}
