/**
 * @file lvgl_app.c
 * @brief 极简 LVGL 演示界面 — 仅显示少量字符，便于验证移植是否成功。
 *
 * 此文件不包含任何 SDL / Emscripten / ESP-IDF 平台代码，
 * 可直接拷贝到 ESP32 工程中使用。
 */

#include "lvgl_app.h"
#include "lvgl/lvgl.h"

/* 控件句柄（静态，仅在文件内使用）*/
static lv_obj_t *label_hello;
static lv_obj_t *label_counter;
static lv_timer_t *app_timer;
static uint32_t counter;

/**
 * @brief 定时器回调：每秒更新计数器
 */
static void on_timer(lv_timer_t *timer)
{
    (void)timer;
    char buf[32];
    counter++;
    lv_snprintf(buf, sizeof(buf), "Uptime: %lu s", (unsigned long)counter);
    lv_label_set_text(label_counter, buf);
}

/**
 * @brief 初始化极简 UI
 *
 * 创建深色背景画布，放置两行文字：
 *   - 标题："LVGL Ready"
 *   - 计数："Uptime: X s"
 */
void lvgl_app_init(void)
{
    lv_obj_t *scr = lv_screen_active();

    /* 深色渐变背景 */
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B1320), 0);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x101B2C), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);

    /* 居中容器 */
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 12, 0);

    /* 大标题 */
    label_hello = lv_label_create(cont);
    lv_label_set_text(label_hello, "LVGL Ready");
    lv_obj_set_style_text_color(label_hello, lv_color_hex(0x5FA8FF), 0);
    lv_obj_set_style_text_font(label_hello, &lv_font_montserrat_28, 0);

    /* 计数标签 */
    label_counter = lv_label_create(cont);
    lv_label_set_text(label_counter, "Uptime: 0 s");
    lv_obj_set_style_text_color(label_counter, lv_color_hex(0xF4F7FB), 0);
    lv_obj_set_style_text_font(label_counter, &lv_font_montserrat_20, 0);

    /* 注释说明 */
    lv_obj_t *note = lv_label_create(cont);
    lv_label_set_text(note, "LVGL 9.3 / Portable Demo");
    lv_obj_set_style_text_color(note, lv_color_hex(0x60758F), 0);
    lv_obj_set_style_text_font(note, &lv_font_montserrat_14, 0);

    /* 1 秒定时器更新计数器 */
    app_timer = lv_timer_create(on_timer, 1000, NULL);
    (void)app_timer;
}
