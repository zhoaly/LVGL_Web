/** Network composition. No ESP-IDF types or service calls belong here. */
#include "App_UiPageNetwork.h"
#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "../../assets/App_UiTheme.h"
#include "../../command/App_UiCommand.h"
#include "../../components/widgets/action_row/App_UiActionRow.h"
#include "../../components/widgets/toggle_row/App_UiToggleRow.h"
#include "../../components/widgets/scroll_stack/App_UiScrollStack.h"
#include "../../components/widgets/confirm_dialog/App_UiConfirmDialog.h"

enum { ROW_CURRENT = 1, ROW_SCAN, ROW_SAVED, ROW_CONNECT, ROW_FORGET,
       ROW_POWER, ROW_AUTO, ROW_JOIN, ROW_NETWORK_BASE = 100 };
typedef struct {
    app_ui_scroll_stack_t stack;
    app_ui_action_row_t current, scan, saved;
    app_ui_toggle_row_t power, automatic;
    app_ui_action_row_t rows[APP_UI_WIFI_NETWORK_CAPACITY];
    app_ui_wifi_network_t keys[APP_UI_WIFI_NETWORK_CAPACITY];
    bool used[APP_UI_WIFI_NETWORK_CAPACITY];
    lv_obj_t *list, *hint;
    int32_t scroll;
} network_page_t;
typedef struct {
    app_ui_scroll_stack_t stack;
    app_ui_action_row_t rows[APP_UI_WIFI_PROFILE_CAPACITY];
    uint32_t ids[APP_UI_WIFI_PROFILE_CAPACITY];
    lv_obj_t *hint;
} saved_page_t;
typedef struct {
    app_ui_scroll_stack_t stack;
    app_ui_action_row_t connect, forget;
    app_ui_toggle_row_t join;
    app_ui_confirm_dialog_t dialog;
    lv_obj_t *description;
} detail_page_t;
static network_page_t s_network;
static saved_page_t s_saved;
static detail_page_t s_detail;
static const app_ui_model_t *s_model;
static uint32_t s_selected, s_token, s_pending, s_scan_generation, s_scan_tick;
static bool s_scan_on_enter;
static app_ui_command_id_t s_pending_command;
static void network_refresh(const app_ui_model_t *model);
static void saved_refresh(const app_ui_model_t *model);
static void detail_refresh(const app_ui_model_t *model);

static const char *auth_name(app_ui_wifi_auth_t auth)
{
    static const char *const names[] = {"Open", "WPA", "WPA2", "WPA3",
        "WPA2/3", "WEP", "Enterprise", "WAPI", "OWE", "Unknown"};
    return (unsigned)auth < sizeof(names)/sizeof(names[0]) ? names[auth] : "Unknown";
}
static bool pending(void)
{
    if(!s_pending) return false;
    const app_ui_wifi_operation_t *op =
        &s_model->operations[s_pending % APP_UI_WIFI_OPERATION_CAPACITY];
    if(op->token == s_pending && op->state != APP_UI_WIFI_OPERATION_ACCEPTED)
        s_pending = 0;
    return s_pending != 0;
}
static bool busy(void)
{
    const app_ui_wifi_runtime_t *w = &s_model->wifi;
    return pending() || w->transitioning || w->scan == APP_UI_WIFI_SCAN_RUNNING ||
        w->link == APP_UI_WIFI_LINK_SELECTING || w->link == APP_UI_WIFI_LINK_CONNECTING ||
        w->link == APP_UI_WIFI_LINK_ASSOCIATED || w->link == APP_UI_WIFI_LINK_DISCONNECTING ||
        w->link == APP_UI_WIFI_LINK_BACKOFF;
}
static const char *link_text(void)
{
    const app_ui_wifi_runtime_t *w = &s_model->wifi;
    if(!w->available) return "Wi-Fi unavailable";
    if(!w->enabled) return "Wi-Fi off";
    if(w->net_ready) return "Connected";
    if(w->link == APP_UI_WIFI_LINK_ASSOCIATED) return "Obtaining IP...";
    if(w->link == APP_UI_WIFI_LINK_DISCONNECTING) return "Disconnecting...";
    if(w->transitioning || w->link == APP_UI_WIFI_LINK_CONNECTING ||
       w->link == APP_UI_WIFI_LINK_SELECTING || w->link == APP_UI_WIFI_LINK_BACKOFF)
        return "Connecting...";
    return "Disconnected";
}
static void refresh_current(void)
{
    if(s_model->current_page == APP_UI_PAGE_NETWORK) network_refresh(s_model);
    else if(s_model->current_page == APP_UI_PAGE_WIFI_SAVED) saved_refresh(s_model);
    else if(s_model->current_page == APP_UI_PAGE_WIFI_DETAIL) detail_refresh(s_model);
}
static bool submit(app_ui_command_id_t id, uint32_t profile, bool enabled)
{
    bool interrupt = id == APP_UI_COMMAND_WIFI_DISCONNECT ||
        (id == APP_UI_COMMAND_WIFI_SET_ENABLED && !enabled);
    if((pending() && s_pending_command == id) ||
       !s_model->wifi.available || (busy() && !interrupt)) {
        App_UiShowMessage("Wi-Fi busy; please wait");
        return false;
    }
    if(++s_token == 0) ++s_token;
    app_ui_command_t command = {.id = id, .profile_id = profile,
        .enabled = enabled, .operation_token = s_token};
    if(!App_UiCommand_Submit(&command)) {
        App_UiShowMessage("Request not accepted; try again");
        return false;
    }
    s_pending = s_token;
    s_pending_command = id;
    refresh_current();
    return true;
}
static void navigate(app_ui_page_id_t page)
{
    app_ui_command_t command = {.id = APP_UI_COMMAND_NAV_PUSH, .page_id = page};
    if(!App_UiCommand_Submit(&command)) App_UiShowMessage("Navigation busy; try again");
}
static const app_ui_wifi_profile_t *profile(uint32_t id)
{
    for(uint8_t i = 0; i < s_model->profiles.count; ++i)
        if(s_model->profiles.items[i].id == id) return &s_model->profiles.items[i];
    return NULL;
}
static bool visible(uint32_t id)
{
    if(!s_model->wifi.enabled) return false;
    for(uint8_t i = 0; i < s_model->networks.count; ++i)
        if(s_model->networks.items[i].profile_id == id) return true;
    return false;
}
static void confirm_forget(bool confirmed, void *user_data)
{
    (void)user_data;
    if(confirmed) submit(APP_UI_COMMAND_WIFI_FORGET_PROFILE, s_selected, false);
}
static void activated(uint32_t id, void *user_data)
{
    (void)user_data;
    switch(id) {
    case ROW_SCAN: submit(APP_UI_COMMAND_WIFI_SCAN, 0, false); return;
    case ROW_SAVED: navigate(APP_UI_PAGE_WIFI_SAVED); return;
    case ROW_CURRENT:
        s_selected = s_model->wifi.connected_profile_id;
        navigate(APP_UI_PAGE_WIFI_DETAIL); return;
    case ROW_CONNECT:
        if(s_model->wifi.connected_profile_id == s_selected &&
           (s_model->wifi.net_ready || s_model->wifi.link != APP_UI_WIFI_LINK_IDLE))
            submit(APP_UI_COMMAND_WIFI_DISCONNECT, 0, false);
        else submit(APP_UI_COMMAND_WIFI_CONNECT_PROFILE, s_selected, false);
        return;
    case ROW_FORGET: {
        const app_ui_confirm_dialog_state_t state = {
            .title = "Forget network?",
            .message = "Saved credentials will be removed. Password entry is not available in this UI.",
            .confirm_text = "Forget", .cancel_text = "Cancel", .enabled = true};
        if(!App_UiConfirmDialog_Create(&s_detail.dialog, &state, confirm_forget, NULL))
            App_UiShowMessage("Cannot open confirmation");
        return;
    }
    default: break;
    }
    if(id >= ROW_NETWORK_BASE && id < ROW_NETWORK_BASE + APP_UI_WIFI_NETWORK_CAPACITY) {
        const app_ui_wifi_network_t *n = &s_network.keys[id - ROW_NETWORK_BASE];
        if(!s_network.used[id - ROW_NETWORK_BASE]) return;
        if(n->connected) {
            s_selected = n->profile_id;
            navigate(APP_UI_PAGE_WIFI_DETAIL);
        } else if(n->hidden) App_UiShowMessage("Manual network entry is not available yet");
        else if(!n->supported) App_UiShowMessage("Unsupported security type");
        else if(n->known && profile(n->profile_id))
            submit(APP_UI_COMMAND_WIFI_CONNECT_PROFILE, n->profile_id, false);
        else if(n->auth == APP_UI_WIFI_AUTH_OPEN)
            App_UiShowMessage("Only saved networks can be connected");
        else App_UiShowMessage("Password entry is not available yet");
    }
}
static void saved_activated(uint32_t id, void *user_data)
{
    (void)user_data;
    s_selected = id;
    navigate(APP_UI_PAGE_WIFI_DETAIL);
}
static void toggled(uint32_t id, bool checked, void *user_data)
{
    (void)user_data;
    if(id == ROW_POWER) {
        if(checked) s_scan_on_enter = true;
        submit(APP_UI_COMMAND_WIFI_SET_ENABLED, 0, checked);
    }
    else if(id == ROW_AUTO) submit(APP_UI_COMMAND_WIFI_SET_AUTO_CONNECT, 0, checked);
    else if(id == ROW_JOIN) submit(APP_UI_COMMAND_WIFI_SET_PROFILE_AUTO_JOIN, s_selected, checked);
    refresh_current(); /* Switches always reflect confirmed service state. */
}
static const app_ui_action_row_callbacks_t actions = {.on_activated = activated};
static const app_ui_action_row_callbacks_t saved_actions = {.on_activated = saved_activated};
static const app_ui_toggle_row_callbacks_t toggles = {.on_changed = toggled};
static void row(lv_obj_t *parent, app_ui_action_row_t *r, uint32_t id,
    const char *title, const char *subtitle, const char *value, bool enabled,
    const app_ui_action_row_callbacks_t *callbacks)
{
    app_ui_action_row_state_t state = {.id=id, .title=title,
        .subtitle=subtitle, .value=value, .enabled=enabled,
        .value_width=id>=ROW_NETWORK_BASE ? 60 : 0};
    if(r->root) App_UiActionRow_Update(r, &state);
    else App_UiActionRow_Create(parent, r, &state, callbacks);
}
static void toggle(lv_obj_t *parent, app_ui_toggle_row_t *r, uint32_t id,
    const char *title, bool checked, bool enabled)
{
    app_ui_toggle_row_state_t state = {.id=id, .title=title,
        .checked=checked, .enabled=enabled};
    if(r->root) App_UiToggleRow_Update(r, &state);
    else App_UiToggleRow_Create(parent, r, &state, &toggles);
}
static lv_obj_t *label(lv_obj_t *parent, const char *text)
{
    lv_obj_t *o = lv_label_create(parent);
    lv_obj_set_width(o, LV_PCT(100));
    lv_obj_set_style_text_font(o, App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY), 0);
    lv_obj_set_style_text_color(o, App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY), 0);
    lv_label_set_text(o, text);
    return o;
}
static bool same_network(const app_ui_wifi_network_t *a, const app_ui_wifi_network_t *b)
{
    return a->hidden == b->hidden && a->auth == b->auth &&
        (a->hidden ? memcmp(a->bssid, b->bssid, 6) == 0 : strcmp(a->ssid,b->ssid) == 0);
}
static void update_network_rows(void)
{
    bool keep[APP_UI_WIFI_NETWORK_CAPACITY] = {0};
    uint8_t slots[APP_UI_WIFI_NETWORK_CAPACITY] = {0};
    uint8_t count = s_model->wifi.enabled ? s_model->networks.count : 0;
    /* Match before recycling slots, preserving identity and encoder focus. */
    for(uint8_t i=0; i<count; ++i) {
        slots[i] = APP_UI_WIFI_NETWORK_CAPACITY;
        for(uint8_t j=0; j<APP_UI_WIFI_NETWORK_CAPACITY; ++j)
            if(s_network.used[j] && same_network(&s_network.keys[j], &s_model->networks.items[i])) {
                slots[i]=j; keep[j]=true; break;
            }
    }
    for(uint8_t j=0; j<APP_UI_WIFI_NETWORK_CAPACITY; ++j) {
        if(!keep[j] && s_network.rows[j].root) lv_obj_delete(s_network.rows[j].root);
        s_network.used[j]=keep[j];
    }
    for(uint8_t i=0; i<count; ++i) {
        uint8_t j=slots[i];
        if(j==APP_UI_WIFI_NETWORK_CAPACITY) {
            for(j=0; j<APP_UI_WIFI_NETWORK_CAPACITY && s_network.used[j]; ++j) {}
        }
        if(j>=APP_UI_WIFI_NETWORK_CAPACITY) break;
        s_network.used[j]=true;
        const app_ui_wifi_network_t *n=&s_model->networks.items[i];
        s_network.keys[j]=*n;
        char title[48], subtitle[48];
        if(n->hidden) snprintf(title,sizeof(title),"Hidden network %02X%02X",n->bssid[4],n->bssid[5]);
        else snprintf(title,sizeof(title),"%s",n->ssid);
        snprintf(subtitle,sizeof(subtitle),"%s %ddBm",auth_name(n->auth),n->rssi);
        row(s_network.list,&s_network.rows[j],ROW_NETWORK_BASE+j,title,subtitle,
            n->connected ? "Connected" : n->known ? "Saved" : "",true,&actions);
        lv_obj_move_to_index(s_network.rows[j].root,i + 6);
    }
}
static void maybe_scan(void)
{
    if(!s_scan_on_enter || !s_model->wifi.available || !s_model->wifi.enabled || busy()) return;
    s_scan_on_enter=false;
    if(!s_model->networks.generation || lv_tick_elaps(s_scan_tick)>15000u)
        submit(APP_UI_COMMAND_WIFI_SCAN,0,false);
}
static void network_refresh(const app_ui_model_t *model)
{
    s_model=model;
    if(!s_network.stack.root) return;
    if(s_scan_generation != model->networks.generation) {
        s_scan_generation=model->networks.generation; s_scan_tick=lv_tick_get();
    }
    bool available=model->wifi.available, enabled=model->wifi.enabled, blocked=busy();
    toggle(s_network.stack.root,&s_network.power,ROW_POWER,"Wi-Fi",enabled,
        available && (!blocked || enabled));
    row(s_network.stack.root,&s_network.current,ROW_CURRENT,
        model->wifi.ssid[0] ? model->wifi.ssid : "Current network",link_text(),
        model->wifi.net_ready ? model->wifi.ipv4 : "",enabled && model->wifi.ssid[0],&actions);
    row(s_network.stack.root,&s_network.scan,ROW_SCAN,
        model->wifi.scan==APP_UI_WIFI_SCAN_RUNNING ? "Scanning..." : "Scan",
        NULL,NULL,available && enabled && !blocked,&actions);
    toggle(s_network.stack.root,&s_network.automatic,ROW_AUTO,"Auto-connect",
        model->wifi.auto_connect,available && !blocked);
    const char *hint = !enabled ? "Turn on Wi-Fi to scan" :
        model->wifi.scan==APP_UI_WIFI_SCAN_FAILED ? "Scan failed. Tap Scan to retry." :
        model->wifi.scan==APP_UI_WIFI_SCAN_RUNNING ? "Refreshing networks..." :
        model->wifi.scan_truncated ? "Results limited; some networks omitted" :
        model->wifi.scan==APP_UI_WIFI_SCAN_READY && !model->networks.count ? "No networks found" :
        "Available networks";
    lv_label_set_text(s_network.hint,hint);
    update_network_rows();
    maybe_scan();
}
static void network_deleted(lv_event_t *event)
{
    (void)event;
    s_network.list=NULL; s_network.hint=NULL;
    memset(s_network.used,0,sizeof(s_network.used));
}
static void network_leave(void)
{
    s_scan_on_enter=false;
    if(s_network.stack.root) s_network.scroll=lv_obj_get_scroll_y(s_network.stack.root);
}
static void network_enter(app_ui_page_transition_t transition)
{
    if(transition==APP_UI_PAGE_TRANSITION_BACK && s_network.stack.root)
        lv_obj_scroll_to_y(s_network.stack.root,s_network.scroll,LV_ANIM_OFF);
    s_scan_on_enter=true;
    maybe_scan();
}
static void network_build(lv_obj_t *parent,const app_ui_model_t *model)
{
    s_model=model;
    lv_obj_t *root=App_UiScrollStack_Create(parent,&s_network.stack);
    lv_obj_add_event_cb(root,network_deleted,LV_EVENT_DELETE,NULL);
    toggle(root,&s_network.power,ROW_POWER,"Wi-Fi",false,false);
    row(root,&s_network.current,ROW_CURRENT,"Current network",NULL,NULL,false,&actions);
    row(root,&s_network.scan,ROW_SCAN,"Scan",NULL,NULL,false,&actions);
    row(root,&s_network.saved,ROW_SAVED,"Saved networks",NULL,NULL,true,&actions);
    toggle(root,&s_network.automatic,ROW_AUTO,"Auto-connect",false,false);
    s_network.hint=label(root,"Available networks");
    s_network.list=root;
    network_refresh(model);
}
static void saved_refresh(const app_ui_model_t *model)
{
    s_model=model;
    if(!s_saved.stack.root) return;
    lv_label_set_text(s_saved.hint,model->profiles.count ? "Saved networks" : "No saved networks");
    for(uint8_t j=0;j<APP_UI_WIFI_PROFILE_CAPACITY;++j) {
        if(s_saved.rows[j].root && !profile(s_saved.ids[j])) {
            lv_obj_delete(s_saved.rows[j].root);
            s_saved.ids[j]=0;
        }
    }
    for(uint8_t i=0;i<model->profiles.count;++i) {
        const app_ui_wifi_profile_t *p=&model->profiles.items[i];
        uint8_t slot=APP_UI_WIFI_PROFILE_CAPACITY;
        for(uint8_t j=0;j<APP_UI_WIFI_PROFILE_CAPACITY;++j)
            if(s_saved.rows[j].root && s_saved.ids[j]==p->id) { slot=j; break; }
        if(slot==APP_UI_WIFI_PROFILE_CAPACITY)
            for(uint8_t j=0;j<APP_UI_WIFI_PROFILE_CAPACITY;++j)
                if(!s_saved.rows[j].root) { slot=j; break; }
        if(slot==APP_UI_WIFI_PROFILE_CAPACITY) break;
        s_saved.ids[slot]=p->id;
        row(s_saved.stack.root,&s_saved.rows[slot],p->id,p->ssid,
            visible(p->id) ? "In scan results" : "Not seen in current scan",
            model->wifi.net_ready && model->wifi.connected_profile_id==p->id ? "Connected" : "",
            true,&saved_actions);
        lv_obj_move_to_index(s_saved.rows[slot].root,i+1);
    }
}
static void saved_build(lv_obj_t *parent,const app_ui_model_t *model)
{
    lv_obj_t *root=App_UiScrollStack_Create(parent,&s_saved.stack);
    s_saved.hint=label(root,"Saved networks");
    saved_refresh(model);
}
static void detail_refresh(const app_ui_model_t *model)
{
    s_model=model;
    if(!s_detail.stack.root) return;
    const app_ui_wifi_profile_t *p=profile(s_selected);
    bool current=model->wifi.connected_profile_id==s_selected && model->wifi.ssid[0];
    bool linked=current && model->wifi.link!=APP_UI_WIFI_LINK_IDLE;
    char description[180];
    if(p) snprintf(description,sizeof(description),"%s\n%s / %s\n%s%s%s",p->ssid,
        auth_name(p->auth),visible(p->id) ? "In scan results" : "Not seen in current scan",
        current ? link_text() : "Not connected",current && model->wifi.net_ready ? "\n" : "",
        current && model->wifi.net_ready ? model->wifi.ipv4 : "");
    else snprintf(description,sizeof(description),"%s", current ? model->wifi.ssid : "Profile no longer available");
    lv_label_set_text(s_detail.description,description);
    row(s_detail.stack.root,&s_detail.connect,ROW_CONNECT,linked ? "Disconnect" : "Connect",
        NULL,NULL,model->wifi.available && model->wifi.enabled &&
        (linked || (p && !busy())),&actions);
    toggle(s_detail.stack.root,&s_detail.join,ROW_JOIN,"Auto-join",p && p->auto_join,p && !busy());
    row(s_detail.stack.root,&s_detail.forget,ROW_FORGET,"Forget network",NULL,NULL,p && !busy(),&actions);
    if(!p) App_UiConfirmDialog_Destroy(&s_detail.dialog);
}
static void detail_leave(void) { App_UiConfirmDialog_Destroy(&s_detail.dialog); }
static void detail_deleted(lv_event_t *event) { (void)event; detail_leave(); }
static void detail_build(lv_obj_t *parent,const app_ui_model_t *model)
{
    lv_obj_t *root=App_UiScrollStack_Create(parent,&s_detail.stack);
    lv_obj_add_event_cb(root,detail_deleted,LV_EVENT_DELETE,NULL);
    s_detail.description=label(root,"");
    detail_refresh(model);
}
static void network_inset(int32_t inset) { App_UiScrollStack_SetBottomSpace(&s_network.stack,inset); }
static void saved_inset(int32_t inset) { App_UiScrollStack_SetBottomSpace(&s_saved.stack,inset); }
static void detail_inset(int32_t inset) { App_UiScrollStack_SetBottomSpace(&s_detail.stack,inset); }
const app_ui_page_t *App_UiPageNetwork_Get(void)
{
    static const app_ui_page_t page={.id=APP_UI_PAGE_NETWORK,.title="Network",
        .dirty_mask=APP_UI_DIRTY_WIFI,.show_back=true,.build=network_build,
        .refresh=network_refresh,.enter=network_enter,.leave=network_leave,.viewport_changed=network_inset};
    return &page;
}
const app_ui_page_t *App_UiPageWifiSaved_Get(void)
{
    static const app_ui_page_t page={.id=APP_UI_PAGE_WIFI_SAVED,.title="Saved networks",
        .dirty_mask=APP_UI_DIRTY_WIFI,.show_back=true,.build=saved_build,.refresh=saved_refresh,.viewport_changed=saved_inset};
    return &page;
}
const app_ui_page_t *App_UiPageWifiDetail_Get(void)
{
    static const app_ui_page_t page={.id=APP_UI_PAGE_WIFI_DETAIL,.title="Network details",
        .dirty_mask=APP_UI_DIRTY_WIFI,.show_back=true,.build=detail_build,
        .refresh=detail_refresh,.leave=detail_leave,.viewport_changed=detail_inset};
    return &page;
}
