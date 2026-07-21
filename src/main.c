#include "lvgl/lvgl.h"

#include <emscripten.h>
#include <stddef.h>

#include "lvgl_app/app/App_Ui.h"

static void lvgl_loop(void *user_data)
{
    (void)user_data;
    lv_timer_handler();
}

int main(void)
{
    lv_init();

    lv_display_t *display = lv_sdl_window_create(LVGL_SIM_WIDTH, LVGL_SIM_HEIGHT);
    if(display == NULL) {
        return 1;
    }
    lv_display_set_default(display);

    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_set_display(mouse, display);

    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, display);

    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, display);

    if(!App_UiInit() || !App_UiStart()) {
        return 1;
    }

    /* Let the browser own the event loop; LVGL is serviced once per frame. */
    emscripten_set_main_loop_arg(lvgl_loop, NULL, 0, 1);
    return 0;
}
