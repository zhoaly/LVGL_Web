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
typedef struct _lv_timer_t lv_timer_t;
typedef struct _lv_group_t lv_group_t;
typedef struct _lv_indev_t lv_indev_t;
typedef uint8_t lv_opa_t;
typedef int32_t lv_style_prop_t;
typedef int32_t lv_anim_enable_t;

typedef struct {
    uint32_t placeholder;
} lv_style_transition_dsc_t;

typedef struct _lv_anim_t {
    uint32_t placeholder;
} lv_anim_t;

typedef struct {
    uint32_t value;
} lv_color_t;

typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} lv_area_t;

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
    LV_EVENT_VALUE_CHANGED,
    LV_EVENT_FOCUSED,
    LV_EVENT_CHILD_CREATED,
    LV_EVENT_DELETE,
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
#define LV_OPA_50 128
#define LV_OPA_COVER 255

#define LV_STATE_DEFAULT 0x0000
#define LV_STATE_CHECKED 0x0001
#define LV_STATE_PRESSED 0x0020
#define LV_STATE_FOCUSED 0x0002
#define LV_STATE_DISABLED 0x0080
#define LV_OBJ_FLAG_CLICKABLE 0x0100
#define LV_OBJ_FLAG_HIDDEN 0x0200
#define LV_OBJ_FLAG_SCROLLABLE 0x0400
#define LV_OBJ_FLAG_FLOATING 0x0800
#define LV_OBJ_FLAG_EVENT_BUBBLE 0x1000

#define LV_PART_INDICATOR 0x010000
#define LV_PART_KNOB 0x020000
#define LV_PART_SCROLLBAR 0x040000

#define LV_STYLE_TRANSFORM_WIDTH 1
#define LV_STYLE_TRANSFORM_HEIGHT 2
#define LV_STYLE_TRANSLATE_Y 3
#define LV_STYLE_BG_COLOR 4
#define LV_STYLE_BG_OPA 5
#define LV_STYLE_BORDER_COLOR 6
#define LV_STYLE_BORDER_OPA 7
#define LV_STYLE_SHADOW_OPA 8
#define LV_STYLE_SHADOW_OFFSET_Y 9

#define LV_ANIM_REPEAT_INFINITE UINT32_MAX
#define LV_ANIM_OFF 0
#define LV_ANIM_ON 1

#define LV_DIR_VER 0x03
#define LV_SCROLLBAR_MODE_AUTO 3
#define LV_LABEL_LONG_MODE_DOTS 3
#define LV_TEXT_ALIGN_RIGHT 2
#define LV_RADIUS_CIRCLE 0x7FFF

#define LV_FLEX_FLOW_ROW 0
#define LV_FLEX_FLOW_COLUMN 1
#define LV_FLEX_ALIGN_CENTER 0
#define LV_FLEX_ALIGN_SPACE_BETWEEN 1
#define LV_FLEX_ALIGN_START 2
#define LV_ALIGN_BOTTOM_MID 0

#define LV_SIZE_CONTENT (-1)
#define LV_PCT(value) (value)

typedef void (*lv_event_cb_t)(lv_event_t *event);
typedef void (*lv_timer_cb_t)(lv_timer_t *timer);
typedef void (*lv_anim_exec_xcb_t)(void *variable, int32_t value);
typedef int32_t (*lv_anim_path_cb_t)(const lv_anim_t *animation);
typedef void (*lv_anim_completed_cb_t)(lv_anim_t *animation);

lv_color_t lv_color_hex(uint32_t value);
lv_obj_t *lv_screen_active(void);
lv_obj_t *lv_layer_top(void);
void lv_refr_now(void *display);
lv_obj_t *lv_obj_create(lv_obj_t *parent);
lv_obj_t *lv_button_create(lv_obj_t *parent);
lv_obj_t *lv_label_create(lv_obj_t *parent);
lv_obj_t *lv_image_create(lv_obj_t *parent);
lv_obj_t *lv_switch_create(lv_obj_t *parent);
lv_obj_t *lv_slider_create(lv_obj_t *parent);
void lv_obj_delete(lv_obj_t *object);
void lv_obj_clean(lv_obj_t *object);
void lv_obj_remove_style_all(lv_obj_t *object);
void lv_obj_set_size(lv_obj_t *object, int32_t width, int32_t height);
void lv_obj_set_width(lv_obj_t *object, int32_t width);
int32_t lv_obj_get_width(const lv_obj_t *object);
int32_t lv_obj_get_height(const lv_obj_t *object);
lv_obj_t *lv_obj_get_parent(const lv_obj_t *object);
void lv_obj_get_coords(const lv_obj_t *object, lv_area_t *area);
int32_t lv_obj_get_scroll_y(const lv_obj_t *object);
void lv_obj_update_layout(const lv_obj_t *object);
void lv_obj_set_height(lv_obj_t *object, int32_t height);
void lv_obj_set_x(lv_obj_t *object, int32_t x);
void lv_obj_align(lv_obj_t *object,
                  int32_t alignment,
                  int32_t x_offset,
                  int32_t y_offset);
void lv_obj_set_flex_flow(lv_obj_t *object, int32_t flow);
void lv_obj_set_flex_align(lv_obj_t *object,
                           int32_t main_place,
                           int32_t cross_place,
                           int32_t track_place);
void lv_obj_set_flex_grow(lv_obj_t *object, uint8_t grow);
void lv_obj_center(lv_obj_t *object);
void lv_obj_add_flag(lv_obj_t *object, uint32_t flag);
void lv_obj_remove_flag(lv_obj_t *object, uint32_t flag);
void lv_obj_add_state(lv_obj_t *object, uint32_t state);
void lv_obj_remove_state(lv_obj_t *object, uint32_t state);
void lv_obj_add_event_cb(lv_obj_t *object,
                         lv_event_cb_t callback,
                         lv_event_code_t filter,
                         void *user_data);
void *lv_event_get_user_data(lv_event_t *event);
void *lv_event_get_param(lv_event_t *event);
lv_obj_t *lv_event_get_target(lv_event_t *event);
lv_event_code_t lv_event_get_code(lv_event_t *event);
void lv_label_set_text(lv_obj_t *label, const char *text);
void lv_label_set_long_mode(lv_obj_t *label, int32_t mode);
void lv_image_set_src(lv_obj_t *image, const void *source);
void lv_group_remove_obj(lv_obj_t *object);
lv_group_t *lv_group_create(void);
lv_group_t *lv_group_get_default(void);
void lv_group_set_default(lv_group_t *group);
uint32_t lv_group_get_obj_count(const lv_group_t *group);
lv_obj_t *lv_group_get_obj_by_index(const lv_group_t *group, uint32_t index);
void lv_group_focus_obj(lv_obj_t *object);
void lv_indev_set_group(lv_indev_t *indev, lv_group_t *group);
void lv_slider_set_range(lv_obj_t *slider, int32_t minimum, int32_t maximum);
void lv_slider_set_value(
    lv_obj_t *slider,
    int32_t value,
    lv_anim_enable_t animation);
int32_t lv_slider_get_value(const lv_obj_t *slider);

void lv_obj_set_style_bg_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_bg_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_border_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_border_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_border_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_image_recolor(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_image_recolor_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_outline_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_outline_pad(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_outline_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_all(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_bottom(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_column(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_hor(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_row(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_pad_ver(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_radius(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_shadow_offset_x(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_offset_y(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_opa(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_shadow_width(lv_obj_t *object, int32_t value, int32_t selector);
void lv_obj_set_style_text_color(lv_obj_t *object, lv_color_t value, int32_t selector);
void lv_obj_set_style_text_font(lv_obj_t *object,
                                const lv_font_t *value,
                                int32_t selector);
void lv_obj_set_style_text_align(lv_obj_t *object,
                                 int32_t value,
                                 int32_t selector);
void lv_obj_set_style_transform_width(lv_obj_t *object,
                                      int32_t value,
                                      int32_t selector);
void lv_obj_set_style_transform_height(lv_obj_t *object,
                                       int32_t value,
                                       int32_t selector);
void lv_obj_set_style_translate_y(lv_obj_t *object,
                                  int32_t value,
                                  int32_t selector);
void lv_obj_set_style_text_opa(lv_obj_t *object,
                               int32_t value,
                               int32_t selector);
void lv_obj_set_style_image_opa(lv_obj_t *object,
                                int32_t value,
                                int32_t selector);
void lv_obj_set_style_transition(
    lv_obj_t *object,
    const lv_style_transition_dsc_t *transition,
    int32_t selector);
void lv_style_transition_dsc_init(
    lv_style_transition_dsc_t *transition,
    const lv_style_prop_t properties[],
    lv_anim_path_cb_t path,
    uint32_t duration,
    uint32_t delay,
    void *user_data);

void lv_obj_set_scroll_dir(lv_obj_t *object, int32_t direction);
void lv_obj_set_scrollbar_mode(lv_obj_t *object, int32_t mode);
void lv_obj_scroll_to_view(lv_obj_t *object, lv_anim_enable_t animation);
void lv_obj_scroll_to_y(
    lv_obj_t *object,
    int32_t y,
    lv_anim_enable_t animation);

void lv_anim_init(lv_anim_t *animation);
void lv_anim_set_var(lv_anim_t *animation, void *variable);
void lv_anim_set_exec_cb(lv_anim_t *animation, lv_anim_exec_xcb_t callback);
void lv_anim_set_values(lv_anim_t *animation, int32_t start, int32_t end);
void lv_anim_set_duration(lv_anim_t *animation, uint32_t duration);
void lv_anim_set_delay(lv_anim_t *animation, uint32_t delay);
void lv_anim_set_reverse_duration(lv_anim_t *animation, uint32_t duration);
void lv_anim_set_repeat_count(lv_anim_t *animation, uint32_t count);
void lv_anim_set_path_cb(lv_anim_t *animation, lv_anim_path_cb_t callback);
void lv_anim_set_user_data(lv_anim_t *animation, void *user_data);
void lv_anim_set_completed_cb(lv_anim_t *animation,
                              lv_anim_completed_cb_t callback);
lv_anim_t *lv_anim_start(lv_anim_t *animation);
uint32_t lv_anim_delete(void *variable, lv_anim_exec_xcb_t callback);
void *lv_anim_get_user_data(lv_anim_t *animation);
int32_t lv_anim_path_ease_out(const lv_anim_t *animation);
lv_timer_t *lv_timer_create(
    lv_timer_cb_t callback,
    uint32_t period,
    void *user_data);

#endif /* TEST_STUB_LVGL_H */
