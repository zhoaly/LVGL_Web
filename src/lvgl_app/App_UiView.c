#include "App_UiView.h"

#include <string.h>

void App_UiView_Init(app_ui_view_t *view)
{
    lv_obj_t *screen;
    if(view == NULL) {
        return;
    }

    memset(view, 0, sizeof(*view));
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFAFBFD), 0);

    view->screen_root = lv_obj_create(screen);
    lv_obj_remove_style_all(view->screen_root);
    lv_obj_set_size(view->screen_root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(view->screen_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(view->screen_root, 6, 0);
    lv_obj_set_style_pad_row(view->screen_root, 4, 0);

    view->title_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(view->title_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(view->title_label, lv_color_hex(0x2778FF), 0);

    view->content = lv_obj_create(view->screen_root);
    lv_obj_remove_style_all(view->content);
    lv_obj_set_width(view->content, LV_PCT(100));
    lv_obj_set_flex_grow(view->content, 1);
    lv_obj_set_flex_flow(view->content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(view->content, 4, 0);

    view->toast_label = lv_label_create(view->screen_root);
    lv_obj_set_style_text_font(view->toast_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(view->toast_label, lv_color_hex(0x888888), 0);
    lv_label_set_text(view->toast_label, "");
}

void App_UiView_ShowPage(app_ui_view_t *view,
                         const app_ui_page_t *page,
                         const app_ui_model_t *model,
                         bool can_back)
{
    if(view == NULL || page == NULL || model == NULL) {
        return;
    }

    view->active_page = page;
    lv_label_set_text(view->title_label, page->title);
    lv_obj_clean(view->content);
    lv_obj_set_flex_flow(view->content, LV_FLEX_FLOW_COLUMN);
    (void)App_UiComponent_CreateNavBar(view->content,
                                      can_back && page->show_back,
                                      view->nav_bindings);
    page->build(view->content, model);
}

void App_UiView_Refresh(app_ui_view_t *view, const app_ui_model_t *model)
{
    if(view != NULL && view->active_page != NULL && view->active_page->refresh != NULL) {
        view->active_page->refresh(model);
    }
}

void App_UiView_ShowToast(app_ui_view_t *view, const char *message)
{
    if(view != NULL) {
        lv_label_set_text(view->toast_label, message != NULL ? message : "");
    }
}
