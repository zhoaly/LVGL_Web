#include "lvgl.h"
#include "lvgl_app/app/App_Ui.h"
#include "lvgl_app/port/App_UiPort.h"
#include "simulator/App_UiWifiMock.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t buffer[240*320];
static uint32_t pixels[240*320];
static lv_indev_t *encoder;
extern int app_ui_mock_wifi_scenario(int scenario);
bool App_UiPort_Init(void) { return true; }
void App_UiPort_Deinit(void) {}
bool App_UiPort_Lock(uint32_t timeout) { (void)timeout; return true; }
void App_UiPort_Unlock(void) {}
bool App_UiPort_Present(void) { return true; }
bool App_UiPort_SetInputGroup(lv_group_t *group) { lv_indev_set_group(encoder,group); return true; }
bool App_UiPort_SetInputAvailable(bool available) { (void)available; return true; }
bool App_UiPort_BindCommandDispatcher(app_ui_command_submitter_fn d,void *data)
{ return App_UiWifiMock_Bind(d,data); }
void App_UiPort_EnterCritical(void) {}
void App_UiPort_ExitCritical(void) {}
static void flush(lv_display_t *display,const lv_area_t *area,uint8_t *data)
{
    uint32_t *source=(uint32_t *)data;
    for(int y=area->y1;y<=area->y2;++y)
        for(int x=area->x1;x<=area->x2;++x) pixels[y*240+x]=*source++;
    lv_display_flush_ready(display);
}
static void advance(unsigned ms)
{
    for(unsigned i=0;i<ms;i+=20) { lv_tick_inc(20); lv_timer_handler(); }
}
static lv_obj_t *find(lv_obj_t *root,const char *text)
{
    if(lv_obj_check_type(root,&lv_label_class) && !strcmp(lv_label_get_text(root),text)) return root;
    for(unsigned i=0;i<lv_obj_get_child_count(root);++i) {
        lv_obj_t *found=find(lv_obj_get_child(root,i),text);
        if(found) return found;
    }
    return NULL;
}
static lv_obj_t *label(const char *text)
{
    lv_obj_t *found=find(lv_layer_top(),text);
    return found ? found : find(lv_screen_active(),text);
}
static void click(const char *text)
{
    lv_obj_t *obj=label(text);
    if(!obj) fprintf(stderr,"Missing label: %s\n",text);
    assert(obj);
    while(obj && !lv_obj_has_flag(obj,LV_OBJ_FLAG_CLICKABLE)) obj=lv_obj_get_parent(obj);
    assert(obj && !lv_obj_has_state(obj,LV_STATE_DISABLED));
    lv_group_focus_obj(obj);
    lv_obj_send_event(obj,LV_EVENT_CLICKED,NULL);
    advance(500);
}
static void navigate(app_ui_command_id_t id,uint32_t page)
{
    app_ui_command_t c={.id=id,.page_id=page};
    assert(App_UiCommand_Submit(&c)); advance(600);
}
static void capture(const char *name)
{
    lv_refr_now(NULL);
    FILE *f=fopen(name,"wb"); assert(f);
    fprintf(f,"P6\n240 320\n255\n");
    for(unsigned i=0;i<240*320;++i) {
        unsigned char rgb[]={(pixels[i]>>16)&255,(pixels[i]>>8)&255,pixels[i]&255};
        fwrite(rgb,1,3,f);
    }
    fclose(f);
}
int main(void)
{
    lv_init();
    lv_display_t *display=lv_display_create(240,320);
    lv_display_set_buffers(display,buffer,NULL,sizeof(buffer),LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display,flush);
    encoder=lv_indev_create(); lv_indev_set_type(encoder,LV_INDEV_TYPE_ENCODER);
    assert(App_UiInit() && App_UiStart());
    advance(300);
    navigate(APP_UI_COMMAND_NAV_PUSH,APP_UI_PAGE_NETWORK);
    advance(1500);
    assert(label("Home Wi-Fi"));
    capture("network.ppm");
    lv_obj_t *network_label=label("Home Wi-Fi");
    lv_obj_t *network_row=lv_obj_get_parent(lv_obj_get_parent(network_label));
    lv_group_focus_obj(network_row); advance(500);
    lv_area_t focused; lv_obj_get_coords(network_row,&focused);
    assert(focused.y1>=48 && focused.y2<254);
    capture("network-list.ppm");
    click("Guest (password)");
    assert(label("Password entry is not available yet"));
    capture("password-toast.ppm");
    advance(3100);
    assert(!label("Password entry is not available yet"));
    click("Home Wi-Fi"); advance(2000);
    assert(label("Connected"));
    click("Saved networks");
    click("Home Wi-Fi");
    assert(label("Disconnect")); capture("details.ppm");
    lv_group_t *previous=lv_indev_get_group(encoder);
    click("Forget network");
    assert(lv_indev_get_group(encoder)!=previous);
    assert(label("Forget network?")); capture("confirm.ppm");
    click("Cancel");
    assert(lv_indev_get_group(encoder)==previous);
    assert(!label("Forget network?"));
    click("Disconnect"); advance(1000);
    assert(app_ui_mock_wifi_scenario(3)); advance(300);
    click("Connect"); advance(2000);
    assert(label("Authentication failed; update saved password"));
    assert(app_ui_mock_wifi_scenario(0)); advance(300);
    click("Forget network"); click("Forget"); advance(1200);
    assert(label("Profile no longer available"));
    navigate(APP_UI_COMMAND_NAV_BACK,0);
    assert(!label("Home Wi-Fi"));
    navigate(APP_UI_COMMAND_NAV_BACK,0);
    assert(label("Home Wi-Fi"));
    assert(app_ui_mock_wifi_scenario(2)); advance(300);
    assert(label("Results limited; some networks omitted"));
    assert(label("Neighbour 19"));
    network_row=lv_obj_get_parent(lv_obj_get_parent(label("Neighbour 19")));
    lv_group_focus_obj(network_row); advance(500);
    lv_obj_get_coords(network_row,&focused);
    assert(focused.y1>=48 && focused.y2<254);
    capture("last-network.ppm");
    assert(app_ui_mock_wifi_scenario(1)); advance(300);
    assert(label("No networks found"));
    assert(app_ui_mock_wifi_scenario(0)); advance(300);
    for(unsigned i=0;i<30;++i) {
        click("Saved networks"); click("Office");
        click("Forget network"); click("Cancel");
        navigate(APP_UI_COMMAND_NAV_HOME,0);
        navigate(APP_UI_COMMAND_NAV_PUSH,APP_UI_PAGE_NETWORK);
    }
    lv_mem_monitor_t memory; lv_mem_monitor(&memory);
    printf("Wi-Fi UI runtime passed; free heap: %u, max used: %u\n",
        (unsigned)memory.free_size,(unsigned)memory.max_used);
    return 0;
}
