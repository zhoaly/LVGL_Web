#ifndef APP_UI_CONFIRM_DIALOG_H
#define APP_UI_CONFIRM_DIALOG_H
#include "lvgl.h"
#include <stdbool.h>

typedef struct {
    const char *title, *message, *confirm_text, *cancel_text;
    bool enabled;
} app_ui_confirm_dialog_state_t;
typedef void (*app_ui_confirm_dialog_callback_t)(bool confirmed, void *user_data);
/* Owner keeps this context alive until Destroy, including parent deletion. */
typedef struct {
    lv_obj_t *root, *title, *message, *confirm, *cancel;
    lv_obj_t *confirm_label, *cancel_label, *previous_focus;
    lv_group_t *group, *previous_group;
    app_ui_confirm_dialog_callback_t callback;
    void *user_data;
} app_ui_confirm_dialog_t;
lv_obj_t *App_UiConfirmDialog_Create(app_ui_confirm_dialog_t *dialog,
    const app_ui_confirm_dialog_state_t *state,
    app_ui_confirm_dialog_callback_t callback, void *user_data);
bool App_UiConfirmDialog_Update(app_ui_confirm_dialog_t *dialog,
    const app_ui_confirm_dialog_state_t *state);
void App_UiConfirmDialog_Destroy(app_ui_confirm_dialog_t *dialog);
#endif
