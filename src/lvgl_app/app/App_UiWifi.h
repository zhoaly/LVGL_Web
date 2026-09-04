#ifndef APP_UI_WIFI_H
#define APP_UI_WIFI_H

#include <stdbool.h>
#include <stdint.h>

#define APP_UI_WIFI_NETWORK_CAPACITY 20u
#define APP_UI_WIFI_PROFILE_CAPACITY 8u
#define APP_UI_WIFI_OPERATION_CAPACITY 8u

typedef enum {
    APP_UI_WIFI_LINK_IDLE, APP_UI_WIFI_LINK_SELECTING,
    APP_UI_WIFI_LINK_CONNECTING, APP_UI_WIFI_LINK_ASSOCIATED,
    APP_UI_WIFI_LINK_READY, APP_UI_WIFI_LINK_DISCONNECTING,
    APP_UI_WIFI_LINK_BACKOFF
} app_ui_wifi_link_t;
typedef enum {
    APP_UI_WIFI_SCAN_IDLE, APP_UI_WIFI_SCAN_RUNNING,
    APP_UI_WIFI_SCAN_READY, APP_UI_WIFI_SCAN_FAILED
} app_ui_wifi_scan_t;
typedef enum {
    APP_UI_WIFI_AUTH_OPEN, APP_UI_WIFI_AUTH_WPA, APP_UI_WIFI_AUTH_WPA2,
    APP_UI_WIFI_AUTH_WPA3, APP_UI_WIFI_AUTH_WPA2_WPA3, APP_UI_WIFI_AUTH_WEP,
    APP_UI_WIFI_AUTH_ENTERPRISE, APP_UI_WIFI_AUTH_WAPI, APP_UI_WIFI_AUTH_OWE,
    APP_UI_WIFI_AUTH_UNKNOWN
} app_ui_wifi_auth_t;

typedef struct {
    uint32_t revision;
    uint32_t scan_generation;
    uint32_t connected_profile_id;
    uint16_t raw_ap_count;
    bool available, enabled, auto_connect, net_ready, transitioning;
    bool scan_truncated;
    app_ui_wifi_link_t link;
    app_ui_wifi_scan_t scan;
    char ssid[33];
    char ipv4[16];
} app_ui_wifi_runtime_t;

typedef struct {
    char ssid[33];
    uint8_t bssid[6];
    int8_t rssi;
    app_ui_wifi_auth_t auth;
    uint32_t profile_id;
    bool known, connected, hidden, supported;
} app_ui_wifi_network_t;
typedef struct {
    uint32_t generation;
    uint8_t count;
    app_ui_wifi_network_t items[APP_UI_WIFI_NETWORK_CAPACITY];
} app_ui_wifi_networks_t;
typedef struct {
    uint32_t id;
    char ssid[33];
    app_ui_wifi_auth_t auth;
    bool hidden, auto_join;
} app_ui_wifi_profile_t;
typedef struct {
    uint32_t generation;
    uint8_t count;
    app_ui_wifi_profile_t items[APP_UI_WIFI_PROFILE_CAPACITY];
} app_ui_wifi_profiles_t;

typedef enum {
    APP_UI_WIFI_OPERATION_ACCEPTED, APP_UI_WIFI_OPERATION_DONE,
    APP_UI_WIFI_OPERATION_FAILED, APP_UI_WIFI_OPERATION_CANCELED
} app_ui_wifi_operation_state_t;
typedef struct {
    uint32_t token;
    uint32_t command_id;
    uint32_t profile_id;
    app_ui_wifi_operation_state_t state;
    char message[48];
} app_ui_wifi_operation_t;

#endif
