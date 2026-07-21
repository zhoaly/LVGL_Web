#include "../App_UiPort.h"

#include "freertos/FreeRTOS.h"

static portMUX_TYPE s_ui_queue_mux = portMUX_INITIALIZER_UNLOCKED;

bool App_UiPort_Init(void)
{
    /* Connect the target display and input drivers here. */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /* Implement target-specific partial/full display refresh here. */
    (void)full_refresh;
}

void App_UiPort_EnterCritical(void)
{
    portENTER_CRITICAL(&s_ui_queue_mux);
}

void App_UiPort_ExitCritical(void)
{
    portEXIT_CRITICAL(&s_ui_queue_mux);
}
