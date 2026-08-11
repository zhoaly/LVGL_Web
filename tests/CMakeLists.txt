cmake_minimum_required(VERSION 3.20)

project(lvgl_app_unit_tests C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_executable(test_app_action
    test_app_action.c
    ../src/lvgl_app/action/app_action.c
    ../src/lvgl_app/action/task_action.c
)
target_include_directories(test_app_action PRIVATE ../src/lvgl_app/action)
add_test(NAME app_action COMMAND test_app_action)

add_executable(test_ui_model
    test_ui_model.c
    ../src/lvgl_app/model/App_UiModel.c
)
target_include_directories(test_ui_model PRIVATE
    ../src/lvgl_app/app
    ../src/lvgl_app/model
)
add_test(NAME ui_model COMMAND test_ui_model)

add_executable(test_ui_event_queue
    test_ui_event_queue.c
    ../src/lvgl_app/app/App_Ui.c
    ../src/lvgl_app/model/App_UiModel.c
    ../src/lvgl_app/navigation/App_UiNav.c
)
target_include_directories(test_ui_event_queue PRIVATE
    stubs
    ../src
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_ui_event_queue PRIVATE -Wall -Wextra -Werror)
endif()
add_test(NAME ui_event_queue COMMAND test_ui_event_queue)

add_executable(test_ui_navigation
    test_ui_navigation.c
    ../src/lvgl_app/navigation/App_UiNav.c
)
target_include_directories(test_ui_navigation PRIVATE
    ../src/lvgl_app/app
    ../src/lvgl_app/navigation
)
add_test(NAME ui_navigation COMMAND test_ui_navigation)

add_executable(test_ui_motion
    test_ui_motion.c
    ../src/lvgl_app/components/motion/App_UiMotion.c
)
target_include_directories(test_ui_motion PRIVATE
    stubs
    ../src
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_ui_motion PRIVATE -Wall -Wextra -Werror)
endif()
add_test(NAME ui_motion COMMAND test_ui_motion)

add_executable(test_ui_pages
    test_ui_pages.c
    ../src/lvgl_app/pages/registry/App_UiPages.c
)
target_include_directories(test_ui_pages PRIVATE
    ../src/lvgl_app/app
    ../src/lvgl_app/model
    ../src/lvgl_app/pages/debug
    ../src/lvgl_app/pages/home
    ../src/lvgl_app/pages/hid_hub
    ../src/lvgl_app/pages/network
    ../src/lvgl_app/pages/settings
    ../src/lvgl_app/pages/text
    ../src/lvgl_app/pages/controls_gallery
    ../src/lvgl_app/pages/registry
)
add_test(NAME ui_pages COMMAND test_ui_pages)

# Compile LVGL-facing UI sources against a declaration-only LVGL 9.3 stub.
# This catches local API/type errors without requiring SDL, Emscripten or a
# full LVGL link in the host-side unit-test build.
add_library(test_ui_compile OBJECT
    ../src/lvgl_app/assets/generated/App_UiIcons.c
    ../src/lvgl_app/assets/App_UiAssets.c
    ../src/lvgl_app/assets/App_UiTheme.c
    ../src/lvgl_app/components/App_UiComponents.c
    ../src/lvgl_app/components/widgets/action_row/App_UiActionRow.c
    ../src/lvgl_app/components/widgets/menu_drawer/App_UiMenuDrawer.c
    ../src/lvgl_app/components/motion/App_UiMotion.c
    ../src/lvgl_app/components/widgets/scroll_stack/App_UiScrollStack.c
    ../src/lvgl_app/components/widgets/slider_row/App_UiSliderRow.c
    ../src/lvgl_app/components/widgets/status_bar/App_UiStatusBar.c
    ../src/lvgl_app/components/widgets/toggle_row/App_UiToggleRow.c
    ../src/lvgl_app/components/widgets/vertical_menu/App_UiVerticalMenu.c
    ../src/lvgl_app/view/App_UiView.c
    ../src/lvgl_app/pages/controls_gallery/App_UiPageControlsGallery.c
    ../src/lvgl_app/pages/debug/App_UiPageDebug.c
    ../src/lvgl_app/pages/home/App_UiPageHome.c
    ../src/lvgl_app/pages/hid_hub/App_UiPageHidHub.c
    ../src/lvgl_app/pages/network/App_UiPageNetwork.c
    ../src/lvgl_app/pages/settings/App_UiPageSettings.c
    ../src/lvgl_app/pages/text/App_UiPageText.c
)
target_include_directories(test_ui_compile PRIVATE
    stubs
    ../src
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_ui_compile PRIVATE -Wall -Wextra -Werror)
endif()

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(
        NAME ui_assets_current
        COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/../tools/assets/build_assets.py
                --check
    )
    set_tests_properties(
        ui_assets_current
        PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
endif()

find_program(NODE_EXECUTABLE node)
if(NODE_EXECUTABLE)
    add_test(
        NAME preview_bridge
        COMMAND ${NODE_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/test_preview_bridge.js
                ${CMAKE_CURRENT_SOURCE_DIR}/../web/preview_bridge.js
    )
    add_test(
        NAME mock_controls
        COMMAND ${NODE_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/test_mock_controls.js
                ${CMAKE_CURRENT_SOURCE_DIR}/../web/mock_controls.js
    )
endif()
