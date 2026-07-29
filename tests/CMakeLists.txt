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

add_executable(test_ui_navigation
    test_ui_navigation.c
    ../src/lvgl_app/navigation/App_UiNav.c
)
target_include_directories(test_ui_navigation PRIVATE
    ../src/lvgl_app/app
    ../src/lvgl_app/navigation
)
add_test(NAME ui_navigation COMMAND test_ui_navigation)

add_executable(test_ui_pages
    test_ui_pages.c
    ../src/lvgl_app/pages/registry/App_UiPages.c
)
target_include_directories(test_ui_pages PRIVATE
    ../src/lvgl_app/app
    ../src/lvgl_app/model
    ../src/lvgl_app/pages/home
    ../src/lvgl_app/pages/text
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
    ../src/lvgl_app/view/App_UiView.c
    ../src/lvgl_app/pages/home/App_UiPageHome.c
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
