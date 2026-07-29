cmake_minimum_required(VERSION 3.20)

project(lvgl_web C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXECUTABLE_SUFFIX ".html")

set(LVGL_SIM_WIDTH "240" CACHE STRING "Simulator canvas width")
set(LVGL_SIM_HEIGHT "320" CACHE STRING "Simulator canvas height")
set(LVGL_LOCAL_SOURCE_DIR "/lvgl_source" CACHE PATH
    "Optional LVGL source mounted by the remote build service")

# Emscripten's SDL2 port supplies the browser canvas, mouse, wheel, keyboard,
# touch and rendering backend used by LVGL's SDL driver.
add_compile_options(-sUSE_SDL=2)
add_link_options(-sUSE_SDL=2)

set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)

# Make lv_conf.h visible while the LVGL library itself is being compiled.
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

if(LVGL_LOCAL_SOURCE_DIR AND EXISTS "${LVGL_LOCAL_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using remote builder LVGL source: ${LVGL_LOCAL_SOURCE_DIR}")
    add_subdirectory("${LVGL_LOCAL_SOURCE_DIR}" lvgl)
else()
    message(STATUS "Fetching LVGL v9.3.0")
    include(FetchContent)
    FetchContent_Declare(
        lvgl
        GIT_REPOSITORY https://github.com/lvgl/lvgl.git
        GIT_TAG v9.3.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(lvgl)
endif()

add_executable(index
    src/main.c
    src/lvgl_app/app/App_Ui.c
    src/lvgl_app/action/app_action.c
    src/lvgl_app/action/task_action.c
    src/lvgl_app/model/App_UiModel.c
    src/lvgl_app/navigation/App_UiNav.c
    src/lvgl_app/view/App_UiView.c
    src/lvgl_app/components/App_UiComponents.c
    src/lvgl_app/components/status_bar/App_UiStatusBar.c
    src/lvgl_app/assets/App_UiAssets.c
    src/lvgl_app/assets/App_UiTheme.c
    src/lvgl_app/assets/generated/App_UiIcons.c
    src/lvgl_app/pages/registry/App_UiPages.c
    src/lvgl_app/pages/home/App_UiPageHome.c
    src/lvgl_app/pages/text/App_UiPageText.c
)

if(EMSCRIPTEN)
    target_sources(index PRIVATE
        src/lvgl_app/port/pc/App_UiPort_Pc.c
    )
else()
    target_sources(index PRIVATE
        src/lvgl_app/port/esp32/App_UiPort_Esp32.c
    )
endif()

target_compile_definitions(index PRIVATE
    LVGL_SIM_WIDTH=${LVGL_SIM_WIDTH}
    LVGL_SIM_HEIGHT=${LVGL_SIM_HEIGHT}
)

target_include_directories(index PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(index PRIVATE lvgl)

target_link_options(index PRIVATE
    -sUSE_SDL=2
    -sALLOW_MEMORY_GROWTH=1
    -sASSERTIONS=1
    -sEXIT_RUNTIME=0
)
