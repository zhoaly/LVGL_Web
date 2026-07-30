# LVGL Web project memory

## Stable architecture

- The project targets LVGL 9.3 on WebAssembly/SDL and keeps
  `src/lvgl_app/` portable to ESP-IDF.
- Pages are static descriptors registered in
  `src/lvgl_app/pages/registry/App_UiPages.c`. The View keeps active and
  outgoing page hosts inside the shared content viewport so Push/Back can
  animate safely; title and bottom navigation remain outside those hosts.
- The navigation bar is a bottom-level child of `screen_root`, not a page
  child. It uses a black-and-white image-based icon style.
- UI code uses `App_UiAssets` for resource lookup and `App_UiTheme` for
  semantic colors/fonts.
- Runtime data sources publish typed `app_ui_event_t` values through
  `App_UiPostEvent()`. They never mutate the Model or LVGL object tree
  directly; Model dirty masks drive page refreshes.

## Component architecture

- Reusable visual components live in their own
  `src/lvgl_app/components/<component_name>/` directory with a public header
  and implementation file.
- A stateful component exposes an instance context plus `Create` and `Update`
  APIs. Pages own the instance and compose components; they do not reach into
  a component's internal object tree.
- Component state is plain platform-neutral data. Hardware, Wi-Fi, Bluetooth,
  time synchronization and other services update the Model first; components
  never call those services directly.
- Component interactions are exposed through callbacks or the shared Action
  layer so the component remains reusable by different pages.
- The Home top bar is implemented by
  `components/status_bar/App_UiStatusBar.{c,h}`. It owns menu, time, weather,
  Wi-Fi and Bluetooth presentation, while the Home page supplies their current
  state.
- The Home menu is implemented by
  `components/menu_drawer/App_UiMenuDrawer.{c,h}`. It is a Home-owned overlay
  created on `lv_layer_top()` and must be destroyed when the Home page root is
  deleted.
- Menu destinations use the normal `APP_ACTION_ID_UI_NAV_PUSH` path. Network,
  HID Hub and Settings are registered pages; non-Home pages use the persistent
  bottom Back/Home navigation bar.
- Interactive components use `App_UiComponent_ApplyFocusStyle()` for consistent
  light- or dark-surface focus feedback. Keep focus visible for encoder input,
  but avoid hard-coded blue outlines.
- Shared motion tokens and helpers live in
  `components/motion/App_UiMotion.{c,h}`. Buttons use style transitions;
  page hosts, small status icons and Toast use `lv_anim`. Runtime levels are
  Normal, Reduced and Off, with Normal as the default. Do not animate full-page
  opacity or zoom, and keep repeating animations limited to small transient
  status icons.
- Web-only mock controls and Emscripten bridges live outside `src/lvgl_app/`.
  They must feed the same typed event path used by future ESP32 services and
  must not be added to the ESP-IDF component source list.

## Asset workflow

- Designer-owned source assets live only under `assets/`.
- `assets/manifest.yaml` is JSON-compatible YAML and is the resource source of
  truth.
- Run `python tools/assets/build_assets.py` after changing the manifest or SVG
  icon sources.
- Run `python tools/assets/build_assets.py --check` in validation/CI.
- Commit generated files under `src/lvgl_app/assets/generated/`; never edit
  them manually.
- Pages and components must not include generated asset headers or hard-code
  generated symbols. Add a semantic ID to `App_UiAssets` and use its getter.
- Use `App_UiTheme` rather than new literal UI colors or direct font symbols.
- Monochrome icons should use A8 so they can be recolored by LVGL styles.
- Subset CJK fonts using files in `assets/glyphsets/`; do not add a full CJK
  font to firmware without an explicit size review.
- Large or replaceable images/fonts may later move to LVGL filesystem assets,
  but the public `App_UiAssets` API must remain stable.

## Build and verification

- Web and ESP-IDF source lists are both explicit. If a new non-aggregated
  generated C file is introduced, update both the root `CMakeLists.txt` and
  `src/lvgl_app/CMakeLists.txt`.
- The standalone Web artifact keeps its built-in Mock Data panel. When it is
  embedded by the remote preview service, it declares `device` preview mode,
  hides that internal panel, and exposes Mock controls through the
  same-origin `zlyhub.lvgl.mock.v1` `postMessage` bridge.
- New Web artifacts expose `zlyhub.lvgl.preview.v1`; its `ready` signal is
  sent only after `App_UiStart()` succeeds and declares Mock/encoder
  capabilities. The remote preview must use it for accurate loading state
  while retaining iframe-load fallback for legacy artifacts.
- The remote preview page uses two independently stacked columns:
  Preview/Encoder and Mock Data/Actions. Mock and encoder cards show skeleton
  states until their bridges are ready, and either capability may be absent
  without disabling the other.
- Local unit tests:
  `cmake --build build_tests_mingw` followed by
  `ctest --test-dir build_tests_mingw --output-on-failure`.
- Full Web verification uses
  `scripts/remote_build.ps1 -Width 240 -Height 320` when the remote builder is
  available.
