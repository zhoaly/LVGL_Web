/**
 * @file App_UiPageControlsGallery.c
 * @brief Local interactive examples for reusable setting controls.
 */

#include "App_UiPageControlsGallery.h"

#include <stdio.h>

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"
#include "../../components/widgets/action_row/App_UiActionRow.h"
#include "../../components/widgets/scroll_stack/App_UiScrollStack.h"
#include "../../components/widgets/slider_row/App_UiSliderRow.h"
#include "../../components/widgets/toggle_row/App_UiToggleRow.h"

enum {
    GALLERY_BOTTOM_SCROLL_SPACE = 60,
    GALLERY_ACTION_DISPLAY_MODE = 1,
    GALLERY_ACTION_DISABLED = 2,
    GALLERY_TOGGLE_WIFI = 10,
    GALLERY_TOGGLE_HAPTICS = 11,
    GALLERY_TOGGLE_DISABLED = 12,
    GALLERY_SLIDER_BRIGHTNESS = 20,
    GALLERY_SLIDER_TIMEOUT = 21,
};

static const char *const s_display_modes[] = {
    "Auto",
    "Light",
    "Dark",
};

static app_ui_scroll_stack_t s_stack;
static app_ui_action_row_t s_display_mode_row;
static app_ui_action_row_t s_disabled_action_row;
static app_ui_toggle_row_t s_wifi_row;
static app_ui_toggle_row_t s_haptics_row;
static app_ui_toggle_row_t s_disabled_toggle_row;
static app_ui_slider_row_t s_brightness_row;
static app_ui_slider_row_t s_timeout_row;
static app_ui_action_row_state_t s_display_mode_state;
static lv_obj_t *s_last_event_label;
static size_t s_display_mode_index;

static void set_last_event(const char *text)
{
    if(s_last_event_label != NULL && text != NULL) {
        lv_label_set_text(s_last_event_label, text);
    }
}

static void gallery_deleted_cb(lv_event_t *event)
{
    (void)event;
    s_last_event_label = NULL;
}

static lv_obj_t *create_section_label(lv_obj_t *parent, const char *text)
{
    const lv_font_t *font = App_UiTheme_GetFont(
        APP_UI_THEME_FONT_EMPHASIS);
    lv_obj_t *label = lv_label_create(parent);

    if(label == NULL) {
        return NULL;
    }
    lv_label_set_text(label, text);
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_set_height(label, (int32_t)font->line_height);
    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(
        label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
        0);
    lv_obj_set_style_pad_hor(label, 4, 0);
    return label;
}

static void action_activated(uint32_t item_id, void *user_data)
{
    (void)user_data;

    if(item_id != GALLERY_ACTION_DISPLAY_MODE) {
        return;
    }

    s_display_mode_index =
        (s_display_mode_index + 1u) %
        (sizeof(s_display_modes) / sizeof(s_display_modes[0]));
    s_display_mode_state.value = s_display_modes[s_display_mode_index];
    (void)App_UiActionRow_Update(
        &s_display_mode_row, &s_display_mode_state);

    {
        char message[48];
        (void)snprintf(
            message,
            sizeof(message),
            "Action: mode = %s",
            s_display_modes[s_display_mode_index]);
        set_last_event(message);
    }
}

static void toggle_changed(
    uint32_t item_id,
    bool checked,
    void *user_data)
{
    char message[48];
    (void)user_data;

    (void)snprintf(
        message,
        sizeof(message),
        "Toggle %lu: %s",
        (unsigned long)item_id,
        checked ? "on" : "off");
    set_last_event(message);
}

static void slider_changed(
    uint32_t item_id,
    int32_t value,
    void *user_data)
{
    char message[48];
    (void)user_data;

    (void)snprintf(
        message,
        sizeof(message),
        "Slider %lu: %ld",
        (unsigned long)item_id,
        (long)value);
    set_last_event(message);
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    static const app_ui_action_row_callbacks_t action_callbacks = {
        .on_activated = action_activated,
        .user_data = NULL,
    };
    static const app_ui_toggle_row_callbacks_t toggle_callbacks = {
        .on_changed = toggle_changed,
        .user_data = NULL,
    };
    static const app_ui_slider_row_callbacks_t slider_callbacks = {
        .on_value_changed = slider_changed,
        .user_data = NULL,
    };
    static const app_ui_action_row_state_t disabled_action_state = {
        .id = GALLERY_ACTION_DISABLED,
        .title = "Reset settings",
        .subtitle = "Disabled example",
        .value = NULL,
        .enabled = false,
    };
    static const app_ui_toggle_row_state_t wifi_state = {
        .id = GALLERY_TOGGLE_WIFI,
        .title = "Wi-Fi",
        .subtitle = "Interactive local state",
        .checked = true,
        .enabled = true,
    };
    static const app_ui_toggle_row_state_t haptics_state = {
        .id = GALLERY_TOGGLE_HAPTICS,
        .title = "Haptic feedback",
        .subtitle = NULL,
        .checked = false,
        .enabled = true,
    };
    static const app_ui_toggle_row_state_t disabled_toggle_state = {
        .id = GALLERY_TOGGLE_DISABLED,
        .title = "Unavailable option",
        .subtitle = "Disabled example",
        .checked = false,
        .enabled = false,
    };
    static const app_ui_slider_row_state_t brightness_state = {
        .id = GALLERY_SLIDER_BRIGHTNESS,
        .title = "Brightness",
        .subtitle = NULL,
        .min = 0,
        .max = 100,
        .step = 5,
        .value = 70,
        .unit = "%",
        .enabled = true,
    };
    static const app_ui_slider_row_state_t timeout_state = {
        .id = GALLERY_SLIDER_TIMEOUT,
        .title = "Screen timeout",
        .subtitle = "Five-second steps",
        .min = 5,
        .max = 60,
        .step = 5,
        .value = 30,
        .unit = "s",
        .enabled = true,
    };
    lv_obj_t *stack_root;
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    stack_root = App_UiScrollStack_Create(parent, &s_stack);
    if(stack_root == NULL) {
        return;
    }
    (void)App_UiScrollStack_SetBottomSpace(
        &s_stack, GALLERY_BOTTOM_SCROLL_SPACE);
    lv_obj_add_event_cb(
        stack_root, gallery_deleted_cb, LV_EVENT_DELETE, NULL);

    s_display_mode_index = 0u;
    s_display_mode_state.id = GALLERY_ACTION_DISPLAY_MODE;
    s_display_mode_state.title = "Display mode";
    s_display_mode_state.subtitle = "Tap to cycle value";
    s_display_mode_state.value = s_display_modes[s_display_mode_index];
    s_display_mode_state.enabled = true;

    (void)create_section_label(stack_root, "Controls Gallery");
    s_last_event_label = lv_label_create(stack_root);
    if(s_last_event_label != NULL) {
        const lv_font_t *font = App_UiTheme_GetFont(
            APP_UI_THEME_FONT_BODY);

        lv_label_set_text(s_last_event_label, "Last event: none");
        lv_obj_set_width(s_last_event_label, LV_PCT(100));
        lv_obj_set_height(
            s_last_event_label, (int32_t)font->line_height);
        lv_label_set_long_mode(
            s_last_event_label, LV_LABEL_LONG_MODE_DOTS);
        lv_obj_set_style_text_font(s_last_event_label, font, 0);
        lv_obj_set_style_text_color(
            s_last_event_label,
            App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
            0);
        lv_obj_set_style_pad_hor(s_last_event_label, 4, 0);
    }

    (void)create_section_label(stack_root, "Actions");
    (void)App_UiActionRow_Create(
        stack_root,
        &s_display_mode_row,
        &s_display_mode_state,
        &action_callbacks);
    (void)App_UiActionRow_Create(
        stack_root,
        &s_disabled_action_row,
        &disabled_action_state,
        &action_callbacks);

    (void)create_section_label(stack_root, "Toggles");
    (void)App_UiToggleRow_Create(
        stack_root, &s_wifi_row, &wifi_state, &toggle_callbacks);
    (void)App_UiToggleRow_Create(
        stack_root, &s_haptics_row, &haptics_state, &toggle_callbacks);
    (void)App_UiToggleRow_Create(
        stack_root,
        &s_disabled_toggle_row,
        &disabled_toggle_state,
        &toggle_callbacks);

    (void)create_section_label(stack_root, "Sliders");
    (void)App_UiSliderRow_Create(
        stack_root,
        &s_brightness_row,
        &brightness_state,
        &slider_callbacks);
    (void)App_UiSliderRow_Create(
        stack_root, &s_timeout_row, &timeout_state, &slider_callbacks);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageControlsGallery_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_CONTROLS_GALLERY,
        .title = "Controls Gallery",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
