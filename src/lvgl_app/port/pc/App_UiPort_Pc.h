#ifndef APP_UI_PORT_PC_H
#define APP_UI_PORT_PC_H

#include "lvgl/lvgl.h"

/** 将 Web 模拟器创建的编码器 indev 交给 UI Port 管理 group。 */
void App_UiPort_PcSetEncoderIndev(lv_indev_t *indev);

#endif /* APP_UI_PORT_PC_H */
