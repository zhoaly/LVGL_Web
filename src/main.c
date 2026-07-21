/**
 * @file main.c
 * @brief 应用程序入口 —— 初始化 LVGL、SDL 显示和 UI 框架。
 *
 * 【启动流程】
 *   1. lv_init() —— 初始化 LVGL 核心库
 *   2. lv_sdl_window_create() —— 创建 SDL 窗口（尺寸由 CMake 变量决定）
 *   3. 创建鼠标、滚轮、键盘输入设备
 *   4. App_UiInit() + App_UiStart() —— 初始化并启动 UI 框架
 *   5. emscripten_set_main_loop_arg() —— 将主循环交给浏览器驱动
 *
 * 【运行环境】
 * 此文件通过 Emscripten 编译为 WebAssembly，在浏览器中运行。
 * LVGL 使用 SDL 后端进行渲染和输入处理。
 */

#include "lvgl/lvgl.h"

#include <emscripten.h>
#include <stddef.h>

#include "lvgl_app/app/App_Ui.h"

/**
 * @brief LVGL 主循环回调
 *
 * 由浏览器每帧调用一次，驱动 LVGL 的定时器和事件处理。
 * 所有 LVGL 相关的更新都在此回调中完成。
 *
 * @param user_data 用户数据（未使用）
 */
static void lvgl_loop(void *user_data)
{
    (void)user_data;
    lv_timer_handler();
}

int main(void)
{
    /* ---- 步骤1：初始化 LVGL 核心库 ---- */
    lv_init();

    /* ---- 步骤2：创建 SDL 显示窗口 ---- */
    lv_display_t *display = lv_sdl_window_create(LVGL_SIM_WIDTH, LVGL_SIM_HEIGHT);
    if(display == NULL) {
        return 1;
    }
    lv_display_set_default(display);

    /* ---- 步骤3：创建输入设备（鼠标、滚轮、键盘） ---- */
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_set_display(mouse, display);

    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, display);

    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, display);

    /* ---- 步骤4：初始化并启动 UI 应用框架 ---- */
    if(!App_UiInit() || !App_UiStart()) {
        return 1;
    }

    /*
     * 步骤5：将主循环交给浏览器控制。
     * 浏览器每帧调用 lvgl_loop，LVGL 在其中处理定时器和事件。
     */
    emscripten_set_main_loop_arg(lvgl_loop, NULL, 0, 1);
    return 0;
}
