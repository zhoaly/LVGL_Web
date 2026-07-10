# =============================================================================
# ESP-IDF 组件 CMake — 将此目录拷贝到 ESP32 工程的 components/ 下使用。
#
# 使用步骤（在 ESP32 工程中）：
#   1. 将 lvgl_app/ 整个目录拷贝到 components/ 下
#   2. 在 main/idf_component.yml 中添加依赖:
#        dependencise:
#          lvgl/lvgl: "^9.3"
#   3. 在 main/CMakeLists.txt 的 REQUIRES 中添加 lvgl_app
#   4. 在 main.c 中调用 lvgl_app_init()
# =============================================================================

idf_component_register(
    SRCS "lvgl_app.c"
    INCLUDE_DIRS "."
    REQUIRES lvgl
)
