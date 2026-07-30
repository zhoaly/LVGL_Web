/**
 * @file main.c
 * @brief 应用程序入口 —— 初始化 LVGL、SDL 显示和 UI 框架。
 *
 * 【启动流程】
 *   1. lv_init() —— 初始化 LVGL 核心库
 *   2. lv_sdl_window_create() —— 创建 SDL 窗口（尺寸由 CMake 变量决定）
 *   3. 创建鼠标、键盘和网页编码器输入设备
 *   4. App_UiInit() + App_UiStart() —— 初始化并启动 UI 框架
 *   5. emscripten_set_main_loop_arg() —— 将主循环交给浏览器驱动
 *
 * 【运行环境】
 * 此文件通过 Emscripten 编译为 WebAssembly，在浏览器中运行。
 * LVGL 使用 SDL 后端进行渲染和输入处理。
 */

#include "lvgl/lvgl.h"

#include <emscripten.h>
#include <stdint.h>
#include <stddef.h>

#include "lvgl_app/app/App_Ui.h"

static lv_indev_t *encoder_indev;
static int32_t encoder_pending_steps;
static lv_indev_state_t encoder_button_state = LV_INDEV_STATE_RELEASED;

static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;
    data->enc_diff = encoder_pending_steps;
    data->state = encoder_button_state;
    encoder_pending_steps = 0;
}

EMSCRIPTEN_KEEPALIVE
void lvgl_encoder_rotate(int32_t steps)
{
    if(encoder_indev == NULL || steps == 0) {
        return;
    }

    if(steps > 32) {
        steps = 32;
    }
    else if(steps < -32) {
        steps = -32;
    }

    encoder_pending_steps += steps;
    lv_indev_read(encoder_indev);
}

EMSCRIPTEN_KEEPALIVE
void lvgl_encoder_set_pressed(int pressed)
{
    lv_indev_state_t next_state =
        pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    if(encoder_indev == NULL || next_state == encoder_button_state) {
        return;
    }

    encoder_button_state = next_state;
    lv_indev_read(encoder_indev);
}

EM_JS(void, install_encoder_bridge, (), {
    const channel = "zlyhub.lvgl.encoder.v1";
    const rotate = (steps) => Module._lvgl_encoder_rotate(steps | 0);
    const setPressed = (pressed) =>
        Module._lvgl_encoder_set_pressed(pressed ? 1 : 0);
    const sendReady = () => {
        if(window.parent !== window) {
            window.parent.postMessage(
                { channel, action: "ready" },
                window.location.origin
            );
        }
    };
    const isPressKey = (event) =>
        event.key === "Enter" || event.key === " ";
    const heldPressKeys = new Set();

    window.addEventListener("message", (event) => {
        if(event.origin !== window.location.origin ||
           event.source !== window.parent) {
            return;
        }

        const message = event.data;
        if(!message || message.channel !== channel) {
            return;
        }

        if(message.action === "probe") {
            sendReady();
        }
        else if(message.action === "rotate" &&
                Number.isInteger(message.steps) &&
                message.steps !== 0) {
            rotate(Math.max(-32, Math.min(32, message.steps)));
        }
        else if(message.action === "press" &&
                typeof message.pressed === "boolean") {
            setPressed(message.pressed);
        }
    });

    window.addEventListener("wheel", (event) => {
        if(event.deltaY === 0) {
            return;
        }

        event.preventDefault();
        rotate(event.deltaY > 0 ? 1 : -1);
    }, { passive: false });

    window.addEventListener("keydown", (event) => {
        if(event.key === "ArrowRight") {
            event.preventDefault();
            rotate(1);
        }
        else if(event.key === "ArrowLeft") {
            event.preventDefault();
            rotate(-1);
        }
        else if(isPressKey(event)) {
            event.preventDefault();
            if(!heldPressKeys.has(event.code)) {
                heldPressKeys.add(event.code);
                setPressed(true);
            }
        }
    });

    window.addEventListener("keyup", (event) => {
        if(isPressKey(event) && heldPressKeys.delete(event.code)) {
            event.preventDefault();
            setPressed(false);
        }
    });

    window.addEventListener("blur", () => {
        heldPressKeys.clear();
        setPressed(false);
    });

    sendReady();
});

EM_JS(void, notify_preview_ready, (int width, int height), {
    if(window.lvglPreviewBridge &&
       typeof window.lvglPreviewBridge.markReady === "function") {
        window.lvglPreviewBridge.markReady({
            width,
            height,
            mockData: true,
            encoder: true
        });
    }
});

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

    /* ---- 步骤3：创建输入设备（鼠标、键盘、网页编码器） ---- */
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_set_display(mouse, display);

    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, display);

    lv_group_t *encoder_group = lv_group_create();
    if(encoder_group == NULL) {
        return 1;
    }
    lv_group_set_default(encoder_group);

    encoder_indev = lv_indev_create();
    if(encoder_indev == NULL) {
        return 1;
    }
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, encoder_read);
    lv_indev_set_display(encoder_indev, display);
    lv_indev_set_group(encoder_indev, encoder_group);

    /* ---- 步骤4：初始化并启动 UI 应用框架 ---- */
    if(!App_UiInit() || !App_UiStart()) {
        return 1;
    }

    install_encoder_bridge();
    notify_preview_ready(LVGL_SIM_WIDTH, LVGL_SIM_HEIGHT);

    /*
     * 步骤5：将主循环交给浏览器控制。
     * 浏览器每帧调用 lvgl_loop，LVGL 在其中处理定时器和事件。
     */
    emscripten_set_main_loop_arg(lvgl_loop, NULL, 0, 1);
    return 0;
}
