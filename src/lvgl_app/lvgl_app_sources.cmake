# Portable LVGL application sources shared by Web and ESP-IDF integrations.
# Platform ports and product-specific command backends intentionally stay out
# of this list.

set(LVGL_APP_CORE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/app/App_Ui.c"
    "${CMAKE_CURRENT_LIST_DIR}/command/App_UiCommand.c"
    "${CMAKE_CURRENT_LIST_DIR}/model/App_UiModel.c"
    "${CMAKE_CURRENT_LIST_DIR}/navigation/App_UiNav.c"
    "${CMAKE_CURRENT_LIST_DIR}/view/App_UiView.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/App_UiComponents.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/motion/App_UiMotion.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/action_row/App_UiActionRow.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/confirm_dialog/App_UiConfirmDialog.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/menu_drawer/App_UiMenuDrawer.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/scroll_stack/App_UiScrollStack.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/slider_row/App_UiSliderRow.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/status_bar/App_UiStatusBar.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/toggle_row/App_UiToggleRow.c"
    "${CMAKE_CURRENT_LIST_DIR}/components/widgets/vertical_menu/App_UiVerticalMenu.c"
    "${CMAKE_CURRENT_LIST_DIR}/assets/App_UiAssets.c"
    "${CMAKE_CURRENT_LIST_DIR}/assets/App_UiTheme.c"
    "${CMAKE_CURRENT_LIST_DIR}/assets/generated/App_UiIcons.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/registry/App_UiPages.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/controls_gallery/App_UiPageControlsGallery.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/debug/App_UiPageDebug.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/home/App_UiPageHome.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/hid_hub/App_UiPageHidHub.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/network/App_UiPageNetwork.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/settings/App_UiPageSettings.c"
    "${CMAKE_CURRENT_LIST_DIR}/pages/text/App_UiPageText.c"
)

set(LVGL_APP_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/app"
    "${CMAKE_CURRENT_LIST_DIR}/command"
)
