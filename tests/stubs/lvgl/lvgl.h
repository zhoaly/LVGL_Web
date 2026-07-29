/**
 * Minimal LVGL 9.3 compile stub for host-side UI source validation.
 *
 * This file intentionally provides declarations only. It does not emulate
 * rendering and must never be included by production targets.
 */

#ifndef TEST_STUB_LVGL_H
#define TEST_STUB_LVGL_H

#include <stdint.h>

typedef struct _lv_obj_t lv_obj_t;
typedef struct _lv_event_t lv_event_t;

typedef struct {
    uint32_t value;
} lv_color_t;

typedef struct _lv_font_t {
    uint32_t line_height;
} lv_font_t;

extern const lv_font_t lv_font_montserrat_14;
extern const lv_font_t lv_font_montserrat_16;

#define LV_FONT_DEFAULT (&lv_font_montserrat_14)

typedef enum {
    LV_EVENT_PRESSED,
    LV_EVENT_PRESS_LOST,
    LV_EVENT_RELEASED,
    LV_EVENT_CLICKED,
} lv_event_code_t;

typedef struct {
    uint32_t magic : 8;
    uint32_t cf : 8;
    uint32_t flags : 16;
    uint32_t w : 16;
    uint32_t h : 16;
    uint32_t stride : 16;
    uint32_t reserved_2 : 16;
} lv_image_header_t;

typedef struct {
    lv_image_header_t header;
    uint32_t data_size;
    const uint8_t *data;
    const void *reserved;
    const void *reserved_2;
} lv_image_dsc_t;

#define LV_IMAGE_HEADER_MAGIC 0x19
#define LV_COLOR_FORMAT_A8 0x0E

#define LV_OPA_TRANSP 0
#define LV_OPA_20 51
#define LV_OPA_40 102
#define LV_OPA_COVER 255

#define LV_STATE_PRESSED 0x0020
#define LV_STATE_FOCUSED 0x0002
#define LV_OBJ_FLAG_CLICKABLE 0x0100
#define LV_OBJ_FLAG_HIDDEN 0x0200

#define LV_FLEX_FLOW_ROW 0
#define LV_FLEX_FLOW_COLUMN 1
#define LV_FLEX_ALIGN_CENTER 0
#define LV_FLEX_ALIGN_SPACE_BETWEEN 1
#define LV_FLEX_ALIGN_START 2

#define LV_SIZE_CONTENT (-1)
#define LV_PCT(value) (value)

typedef void (*lv_event_cb_t)(lv_event_t *event);

lv_color_t lv_color_hex(uint32_t value);
lv_obj_t *lv_screen_active(void);
lv_obj_t *lv_obj_create(lv_obj_t *parent);
lv_obj_t *lv_button_create(lv_obj_t *parent);
lv_obj_t *lv_label_create(lv_obj_t *parent);
lv_obj_t *lv_image_create(lv_obj_t *parent);
void lv_obj_delete(lv_obj_t *object);
void lv_obj_clean(lv_obj_t *object);
void lv_obj_remove_style_all(lv_obj_t *object);
void lv_obj_set_size(lv_obj_t *object, int32_t width, int32_t height);
void lv_obj_set_width(lv_obj_t *object, int32_t width);
void lv_obj_set_height(lv_obj_t *object, int32_t height);
void lv_obj_set_flex_flow(lv_obj_t *object, int32_t flow);
void lv_obj_set_flex_align(lv_obj_t *object,
                           int32_t main_place,
                           int32_t cross_place,
                           int32_t track_place);
void lv_obj_set_flex_grow(lv_obj_t *object, uint8_t grow);
void lv_obj_center(lv_obj_t *object);
void lv_obj_add_flag(lv_obj_t *object, uint32_t flag);
void lv_obj_remove_flag(lv_obj_t *object, uint32_t flag);
void lv_obj_add_event_cb(lv_obj_t *object,
                         lv_event_cb_t callback,
                         lv_event_code_t filter,
                         void *user_data);
void *lv_event_get_user_data(lv_event_t *event);
lv_event_code_t lv_event_get_code(lv_event_t *event);
void lv_label_set_text(lv_obj_t *label, const char *text);
void lv_image_set_src(lv_obj_t *image, const void *source);

void lv_obj_set_style_bg_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_bg_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_border_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_border_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_border_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_image_recolor(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_image_recolor_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_outline_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_outline_pad(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_outline_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_all(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_hor(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_row(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_ver(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_radius(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_shadow_offset_y(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_text_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_text_font(lv_obj_t *object,
                                const lv_font_t *value,
                                int32_t selector);

#endif /* TEST_STUB_LVGL_H */
