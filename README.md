# LVGL Web

基于 LVGL 9.3、SDL2 与 Emscripten 的 WebAssembly 预览工程。`src/lvgl_app/`
保持平台无关，可作为 ESP-IDF UI 组件迁移。

## 远程构建

```powershell
.\scripts\remote_build.ps1
```

自定义画布：

```powershell
.\scripts\remote_build.ps1 -Width 320 -Height 480
```

## UI 结构

```text
src/lvgl_app/
├── include/App_Ui.h              对外状态事件与生命周期 API
├── App_Ui.c                      事件/命令队列、Action 回调和刷新调度
├── App_UiNav.c                   页面栈
├── App_UiModel.c                 UI 状态快照与 dirty 标记
├── App_UiView.c                  屏幕宿主、标题、内容区和 Toast
├── action/                       ESP app_action 的 Web UI-only 版本
├── components/                   导航栏、状态卡片、信息行等复用组件
├── pages/                        所有页面及页面注册表
├── port/                         PC/ESP32 平台适配
└── CMakeLists.txt                ESP-IDF 组件构建入口
```

页面不再使用 View 模板。新增页面时实现一个独立页面描述符（`build`、`refresh`、
标题和 dirty mask），然后加入 `App_UiPages_Get()` 使用的注册表。

## Action 约束

所有可交互 LVGL 控件必须绑定 `app_action_request_t`。页面和组件不得直接调用
`App_UiNav_*`：

```text
LVGL 点击
  -> app_action_submit()
  -> Action dispatcher
  -> UI 命令队列
  -> App_UiNav
  -> 页面切换
```

Web 构建使用 `APP_ACTION_UI_ONLY`，仅执行 UI 导航 Action；其他 ESP 硬件命令返回
`ESP_ERR_NOT_SUPPORTED`。ESP-IDF 版本由完整 `app_action/task_action` worker 执行相同
UI Action，并通过注册回调把命令安全地投递回 UI 队列。

## 迁移到 ESP-IDF

将 `src/lvgl_app/` 复制为 ESP-IDF 组件，并确保工程同时提供完整的 `app_action`
组件。`lvgl_app` 依赖 `lvgl` 和 `app_action`，显示、输入以及墨水屏刷新策略由
`port/App_UiPort_Esp32.c` 接入。初始化 LVGL 后调用：

```c
#include "lvgl_app.h"

lvgl_app_init();
```
