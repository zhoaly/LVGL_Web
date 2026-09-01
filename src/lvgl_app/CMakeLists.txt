include("${CMAKE_CURRENT_LIST_DIR}/lvgl_app_sources.cmake")

idf_component_register(
    SRCS
        ${LVGL_APP_CORE_SOURCES}
        "port/esp32/App_UiPort_Esp32.c"
    INCLUDE_DIRS
        ${LVGL_APP_INCLUDE_DIRS}
    REQUIRES
        lvgl
    PRIV_REQUIRES
        freertos
)
