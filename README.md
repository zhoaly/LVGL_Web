# LVGL Web

一个可直接上传到 ESP Remote Build 服务的 LVGL 9.3 WebAssembly 工程。工程使用 SDL2 作为 LVGL 显示/输入驱动，由 Emscripten 输出 `index.html`、JavaScript 和 WebAssembly 预览文件。

UI 代码已封装为独立组件 `src/lvgl_app/`，与平台层完全解耦，可一键移植到 ESP32 工程。

## 远端网页构建

1. 启动参考工程中的 Local Agent。
2. 打开 `http://100.87.225.67:8000/tools/lvgl/build`。
3. 选择 `Local upload build` 和 `Full web project`。
4. Project Path 填写本工程绝对路径：

   ```text
   C:\Users\13281\OneDrive\dev_projects\LVGL_Web
   ```

5. Project Name 填写 `LVGL_Web`，设置预览宽高，然后启动远端构建。

服务会忽略压缩包的外层目录并自动定位根目录的 `CMakeLists.txt`。构建成功后可在 LVGL Preview 页面运行或下载预览包。

## PowerShell 直接上传

不经过 Local Agent，也可以在工程根目录运行：

```powershell
.\scripts\remote_build.ps1
```

自定义画布：

```powershell
.\scripts\remote_build.ps1 -Width 320 -Height 480
```

脚本会排除 `build`、`build_web` 和 `.git`，上传到 `/api/lvgl/build/upload`，等待任务结束，并输出预览、产物和日志地址。

## 工程结构

```text
CMakeLists.txt                      Emscripten/LVGL 构建入口（Web 目标）
lv_conf.h                           LVGL 9.3 配置（Web 与 ESP32 共用）
src/
├── main.c                          SDL 驱动和浏览器主循环（平台相关）
├── ui.h / ui.c                     旧 UI 兼容层，已重定向至 lvgl_app
└── lvgl_app/                       ★ 可移植 UI 组件 — 可直接拷贝到 ESP32
    ├── lvgl_app.h                   UI 入口声明
    ├── lvgl_app.c                   极简演示 UI（仅显示字符 + 计时器）
    ├── CMakeLists.txt               ESP-IDF 组件 CMake（供 ESP32 使用）
    └── idf_component.yml            ESP-IDF 组件清单（自动拉取 lvgl）
scripts/
└── remote_build.ps1                打包、上传、轮询脚本
```

## 移植到 ESP32

将 `src/lvgl_app/` 整个目录拷贝到 ESP32 工程的 `components/` 下：

```text
ESP32_S3_wifi_ble_hub/
└── components/
    └── lvgl_app/              ← 从本工程拷贝
        ├── lvgl_app.h
        ├── lvgl_app.c
        ├── CMakeLists.txt
        └── idf_component.yml
```

然后在 ESP32 工程中：

1. **添加 LVGL 依赖** — 在 `main/idf_component.yml` 中新增：
   ```yaml
   dependencies:
     lvgl/lvgl: "^9.3"
   ```

2. **注册组件** — 在 `main/CMakeLists.txt` 的 `REQUIRES` 中添加 `lvgl_app`。

3. **调用入口** — 在 `main.c` 中初始化 LVGL 和显示驱动后调用：
   ```c
   #include "lvgl_app/lvgl_app.h"
   lvgl_app_init();
   ```

4. **运行 `idf.py reconfigure`** 自动拉取 LVGL 库。

---

构建服务若挂载了 `/lvgl_source`，CMake 会优先使用该缓存源码；否则固定从官方仓库获取 `v9.3.0`，以保证版本可复现。

