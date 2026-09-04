/** Web-only asynchronous Wi-Fi backend using the production typed protocol. */
#include "App_UiWifiMock.h"
#include "../lvgl_app/app/App_Ui.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>
static app_ui_command_submitter_fn s_dispatch;
static void *s_dispatch_data;
static app_ui_wifi_runtime_t s_runtime;
static app_ui_wifi_networks_t s_networks;
static app_ui_wifi_profiles_t s_profiles;
static app_ui_command_t s_pending;
static app_ui_wifi_operation_t s_results[APP_UI_WIFI_OPERATION_CAPACITY];
static uint32_t s_started;
static int s_scenario;
static bool s_runtime_dirty=true,s_networks_dirty=true,s_profiles_dirty=true;
static app_ui_event_t s_event;
static app_ui_wifi_profile_t *profile(uint32_t id)
{
    for(uint8_t i=0;i<s_profiles.count;++i) if(s_profiles.items[i].id==id) return &s_profiles.items[i];
    return NULL;
}
static int compare(const void *a,const void *b)
{
    const app_ui_wifi_network_t *x=a,*y=b;
    if(x->connected!=y->connected) return x->connected ? -1 : 1;
    if(x->known!=y->known) return x->known ? -1 : 1;
    return y->rssi-x->rssi;
}
static void annotate(void)
{
    for(uint8_t i=0;i<s_networks.count;++i) {
        app_ui_wifi_network_t *n=&s_networks.items[i];
        n->known=false; n->profile_id=0;
        for(uint8_t j=0;j<s_profiles.count;++j)
            if(!strcmp(n->ssid,s_profiles.items[j].ssid) && n->auth==s_profiles.items[j].auth) {
                n->known=true; n->profile_id=s_profiles.items[j].id; break;
            }
        n->connected=s_runtime.net_ready && n->profile_id && n->profile_id==s_runtime.connected_profile_id;
    }
    qsort(s_networks.items,s_networks.count,sizeof(s_networks.items[0]),compare);
    s_networks_dirty=true; s_runtime_dirty=true;
    ++s_runtime.revision;
}
static void results(void)
{
    static const app_ui_wifi_network_t examples[]={
        {.ssid="Home Wi-Fi",.rssi=-42,.auth=APP_UI_WIFI_AUTH_WPA2,.supported=true},
        {.ssid="Office",.rssi=-58,.auth=APP_UI_WIFI_AUTH_WPA2,.supported=true},
        {.ssid="Guest (password)",.rssi=-51,.auth=APP_UI_WIFI_AUTH_WPA2,.supported=true},
        {.ssid="Cafe open",.rssi=-66,.auth=APP_UI_WIFI_AUTH_OPEN,.supported=true},
        {.ssid="Enterprise",.rssi=-71,.auth=APP_UI_WIFI_AUTH_ENTERPRISE},
        {.bssid={0,0,0,0,0xAB,0x12},.rssi=-78,.hidden=true,.auth=APP_UI_WIFI_AUTH_WPA2,.supported=true}};
    memset(s_networks.items,0,sizeof(s_networks.items));
    s_networks.count=s_scenario==1 ? 0 : 6;
    if(s_networks.count) memcpy(s_networks.items,examples,sizeof(examples));
    if(s_scenario==2) {
        s_networks.count=20;
        for(unsigned i=6;i<20;++i) {
            snprintf(s_networks.items[i].ssid,33,"Neighbour %02u",i);
            s_networks.items[i].auth=APP_UI_WIFI_AUTH_WPA2;
            s_networks.items[i].rssi=-80; s_networks.items[i].supported=true;
        }
    }
    ++s_networks.generation;
    s_runtime.scan_generation=s_networks.generation;
    s_runtime.scan_truncated=s_scenario==2;
    s_runtime.raw_ap_count=s_scenario==2 ? 40 : s_networks.count;
    s_runtime.scan=APP_UI_WIFI_SCAN_READY;
    annotate();
}
static void outcome(const app_ui_command_t *c,app_ui_wifi_operation_state_t state,const char *message)
{
    app_ui_wifi_operation_t *o=&s_results[c->operation_token % APP_UI_WIFI_OPERATION_CAPACITY];
    *o=(app_ui_wifi_operation_t){.token=c->operation_token,.command_id=c->id,
        .profile_id=c->profile_id,.state=state};
    snprintf(o->message,sizeof(o->message),"%s",message);
}
static void disconnect(void)
{
    s_runtime.net_ready=false; s_runtime.link=APP_UI_WIFI_LINK_IDLE;
    s_runtime.connected_profile_id=0; s_runtime.ssid[0]=0; s_runtime.ipv4[0]=0;
    annotate();
}
static bool submit(const app_ui_command_t *c,void *data)
{
    (void)data;
    if(c->id<APP_UI_COMMAND_WIFI_SET_ENABLED) return s_dispatch(c,s_dispatch_data);
    if(s_results[c->operation_token % APP_UI_WIFI_OPERATION_CAPACITY].token) return false;
    if(s_scenario==7) return false;
    if(s_pending.operation_token) {
        if(c->id!=APP_UI_COMMAND_WIFI_DISCONNECT &&
           !(c->id==APP_UI_COMMAND_WIFI_SET_ENABLED && !c->enabled)) return false;
        outcome(&s_pending,APP_UI_WIFI_OPERATION_CANCELED,"Operation canceled");
        s_pending=(app_ui_command_t){0}; s_runtime.scan=APP_UI_WIFI_SCAN_IDLE;
    }
    if((c->id==APP_UI_COMMAND_WIFI_SCAN || c->id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE) && !s_runtime.enabled)
        return false;
    s_pending=*c; s_started=lv_tick_get();
    outcome(c,APP_UI_WIFI_OPERATION_ACCEPTED,"");
    if(c->id==APP_UI_COMMAND_WIFI_SCAN) s_runtime.scan=APP_UI_WIFI_SCAN_RUNNING;
    if(c->id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE) {
        app_ui_wifi_profile_t *p=profile(c->profile_id);
        s_runtime.net_ready=false;
        s_runtime.link=APP_UI_WIFI_LINK_CONNECTING;
        s_runtime.connected_profile_id=c->profile_id;
        snprintf(s_runtime.ssid,33,"%s",p ? p->ssid : "");
    }
    annotate();
    return true;
}
static void finish(void)
{
    app_ui_command_t c=s_pending;
    s_pending=(app_ui_command_t){0};
    app_ui_wifi_profile_t *p=profile(c.profile_id);
    const char *failure=NULL;
    if(c.id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE) {
        if(!p || s_scenario==4) failure="Network not found";
        else if(s_scenario==3) failure="Authentication failed; update saved password";
        else if(s_scenario==5) failure="Could not obtain an IP address";
        if(failure) disconnect();
        else {
            s_runtime.link=APP_UI_WIFI_LINK_READY; s_runtime.net_ready=true;
            snprintf(s_runtime.ipv4,16,"192.168.1.42");
        }
    } else if(c.id==APP_UI_COMMAND_WIFI_SCAN) {
        if(s_scenario==8) { s_runtime.scan=APP_UI_WIFI_SCAN_FAILED; failure="Scan failed; try again"; }
        else results();
    } else if(c.id==APP_UI_COMMAND_WIFI_SET_ENABLED) {
        s_runtime.enabled=c.enabled;
        if(!c.enabled) disconnect();
    } else if(c.id==APP_UI_COMMAND_WIFI_DISCONNECT) disconnect();
    else if(s_scenario==6) failure="Could not save network settings";
    else if(c.id==APP_UI_COMMAND_WIFI_SET_AUTO_CONNECT) s_runtime.auto_connect=c.enabled;
    else if(!p) failure="Network settings are no longer available";
    else if(c.id==APP_UI_COMMAND_WIFI_SET_PROFILE_AUTO_JOIN) p->auto_join=c.enabled;
    else if(c.id==APP_UI_COMMAND_WIFI_FORGET_PROFILE) {
        if(s_runtime.connected_profile_id==p->id) disconnect();
        size_t index=(size_t)(p-s_profiles.items);
        for(size_t i=index;i+1<s_profiles.count;++i) s_profiles.items[i]=s_profiles.items[i+1];
        --s_profiles.count;
    }
    ++s_profiles.generation; s_profiles_dirty=true; annotate();
    outcome(&c,failure ? APP_UI_WIFI_OPERATION_FAILED : APP_UI_WIFI_OPERATION_DONE,
        failure ? failure : c.id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE ? "Connected" :
        c.id==APP_UI_COMMAND_WIFI_FORGET_PROFILE ? "Network forgotten" : "");
}
static void pump(lv_timer_t *timer)
{
    (void)timer;
    if(!App_UiIsReady()) return;
    if(s_pending.operation_token) {
        uint32_t age=lv_tick_elaps(s_started);
        if(s_pending.id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE && age>700 && age<1600 &&
           s_runtime.link!=APP_UI_WIFI_LINK_ASSOCIATED) {
            s_runtime.link=APP_UI_WIFI_LINK_ASSOCIATED; s_runtime_dirty=true;
        }
        if(age > (s_pending.id==APP_UI_COMMAND_WIFI_CONNECT_PROFILE ? 1600u : 800u)) finish();
    }
    memset(&s_event,0,sizeof(s_event));
    if(s_runtime_dirty) {
        s_event.type=APP_UI_EVENT_WIFI_RUNTIME; s_event.data.wifi_runtime=s_runtime;
        if(App_UiPostEvent(&s_event)) s_runtime_dirty=false;
    }
    if(s_networks_dirty) {
        s_event.type=APP_UI_EVENT_WIFI_NETWORKS; s_event.data.wifi_networks=s_networks;
        if(App_UiPostEvent(&s_event)) s_networks_dirty=false;
    }
    if(s_profiles_dirty) {
        s_event.type=APP_UI_EVENT_WIFI_PROFILES; s_event.data.wifi_profiles=s_profiles;
        if(App_UiPostEvent(&s_event)) s_profiles_dirty=false;
    }
    for(unsigned i=0;i<APP_UI_WIFI_OPERATION_CAPACITY;++i) if(s_results[i].token) {
        s_event.type=APP_UI_EVENT_WIFI_OPERATION; s_event.data.wifi_operation=s_results[i];
        if(App_UiPostEvent(&s_event)) s_results[i].token=0;
    }
}
EMSCRIPTEN_KEEPALIVE
int app_ui_mock_wifi_scenario(int scenario)
{
    if(scenario<0 || scenario>8 || s_pending.operation_token) return 0;
    s_scenario=scenario;
    results();
    return 1;
}
int App_UiWifiMock_SetStatus(int state)
{
    if(state<0 || state>2) return 0;
    if(s_pending.operation_token) {
        outcome(&s_pending,APP_UI_WIFI_OPERATION_CANCELED,"Operation canceled");
        s_pending=(app_ui_command_t){0};
        s_runtime.scan=APP_UI_WIFI_SCAN_IDLE;
    }
    disconnect(); s_runtime.enabled=true;
    if(state>0 && s_profiles.count) {
        s_runtime.connected_profile_id=s_profiles.items[0].id;
        snprintf(s_runtime.ssid,33,"%s",s_profiles.items[0].ssid);
        s_runtime.link=state==2 ? APP_UI_WIFI_LINK_READY : APP_UI_WIFI_LINK_CONNECTING;
        s_runtime.net_ready=state==2;
        if(state==2) snprintf(s_runtime.ipv4,16,"192.168.1.42");
    }
    annotate();
    return 1;
}
bool App_UiWifiMock_Bind(app_ui_command_submitter_fn dispatcher,void *user_data)
{
    s_dispatch=dispatcher; s_dispatch_data=user_data;
    s_runtime=(app_ui_wifi_runtime_t){.available=true,.enabled=true,.auto_connect=true};
    s_profiles=(app_ui_wifi_profiles_t){.generation=1,.count=3,.items={
        {.id=1,.ssid="Home Wi-Fi",.auth=APP_UI_WIFI_AUTH_WPA2,.auto_join=true},
        {.id=2,.ssid="Office",.auth=APP_UI_WIFI_AUTH_WPA2,.auto_join=true},
        {.id=3,.ssid="Travel router",.auth=APP_UI_WIFI_AUTH_WPA2,.auto_join=false}}};
    lv_timer_create(pump,100,NULL);
    return App_UiCommand_SetSubmitter(submit,NULL);
}
