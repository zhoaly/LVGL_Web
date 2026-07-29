# UI asset pipeline

This directory contains designer-owned source resources. Runtime UI code must
consume assets through `src/lvgl_app/assets/App_UiAssets.h`.

## Generate

```powershell
python tools/assets/build_assets.py
python tools/assets/build_assets.py --check
```

Generated files are written to:

```text
src/lvgl_app/assets/generated/
├── App_UiIcons.c
├── App_UiIcons.h
└── assets_report.json
```

Generated files are deterministic and committed to Git so Web and ESP-IDF
builds do not need asset conversion dependencies.

## Current converter scope

The built-in generator converts simple SVG `line` and `polyline` strokes into
anti-aliased LVGL A8 images. This covers small monochrome UI icons and requires
only the Python standard library.

PNG/JPEG artwork and TTF/OTF fonts should be converted with the official LVGL
image/font converters. Their generated resources must still be registered
behind `App_UiAssets`; pages must not reference generated symbols or file paths
directly.

## Adding an icon

1. Add an SVG under `assets/icons/`.
2. Add an `icons` entry to `assets/manifest.yaml`.
3. Run the generator.
4. Add a semantic enum value and mapping in `App_UiAssets`.
5. Use `App_UiAssets_GetIcon()` from the component.

Use lowercase IDs and C symbols. Prefer A8 for any icon that needs theme-based
recoloring.
