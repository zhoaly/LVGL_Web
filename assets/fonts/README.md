# Font sources

Place licensed TTF/OTF source fonts in this directory. Do not add an entire CJK
font to the firmware without subsetting it first.

Font conversion should use the character lists under `assets/glyphsets/`, and
the resulting LVGL font must be exposed through `App_UiAssets`/`App_UiTheme`.
