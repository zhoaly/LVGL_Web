# LVGL Web

基于 LVGL 9.3、SDL2 和 Emscripten 的 WebAssembly UI 工程。`src/lvgl_app/`
保持平台无关，可作为一个独立 ESP-IDF 组件迁移到 ESP32 工程。

当前工程只包含一个初始页面：标题为 `LVGL App`，内容为 `Ready`。USB、Wi-Fi、
Runtime 等演示业务已经移除，保留的代码均用于后续扩展框架。

## Web 远程构建

```powershell
.\scripts\remote_build.ps1 -Width 240 -Height 320
```

## 目录结构

```text
src/lvgl_app/
├── app/                 UI 生命周期、事件队列和总调度
├── action/              通用 UI Action、Dispatcher 和任务快照
├── assets/              资源注册、主题和自动生成的 LVGL 资源
├── model/               UI 状态与 dirty 标志
├── navigation/          页面栈和导航操作
├── view/                屏幕容器、页面渲染和 Toast
├── components/          可复用控件、Motion 和 Action/Focus 辅助
│   ├── motion/          共享动效令牌和动画辅助
│   └── widgets/         跨页面复用的独立视觉组件
├── pages/
│   ├── registry/        页面描述符和页面注册表
│   ├── home/            首页
│   └── text/            纯文字示例页面
├── port/
│   ├── pc/              Web/SDL 平台适配
│   └── esp32/           ESP32 平台适配骨架
├── CMakeLists.txt       ESP-IDF 组件构建入口
└── idf_component.yml    ESP-IDF 组件依赖
```

设计源资源位于仓库根目录的 `assets/`。图标由
`tools/assets/build_assets.py` 转换并写入
`src/lvgl_app/assets/generated/`。页面和组件通过 `App_UiAssets` 获取资源，通过
`App_UiTheme` 获取语义颜色和字体，不直接引用生成符号。

```powershell
python tools/assets/build_assets.py
python tools/assets/build_assets.py --check
```

## 新增页面

1. 在 `app/App_Ui.h` 的 `app_ui_page_id_t` 中添加页面 ID。
2. 在 `pages/` 下为页面创建独立文件夹，实现自己的 `build`、`refresh` 和
   `App_UiPageXxx_Get()`。
3. 在 `pages/registry/App_UiPages.c` 中注册页面描述符。
4. 需要跳转时，为控件绑定 `APP_ACTION_ID_UI_NAV_PUSH`，并传入目标页面 ID。

首页不会显示导航栏。新增的非首页页面会自动使用通用 Home 导航，并在页面描述符
允许且导航栈可返回时显示 Back 按钮。

## 新增状态或事件

1. 在 `app/App_Ui.h` 中添加事件类型和对应 dirty 标志。
2. 在 `model/App_UiModel.h` 中添加最小必要状态。
3. 在 `model/App_UiModel.c` 的 `App_UiModel_ApplyEvent()` 中更新状态和 dirty 标志。
4. 让相关页面的 `dirty_mask` 包含该标志，并在 `refresh` 中更新控件。

外部任务通过 `App_UiPostEvent()` 投递事件，不应直接操作 LVGL 控件。

## 新增通用组件

独立视觉组件放入 `components/widgets/<component_name>/`，每个组件拥有自己的
公开头文件和实现文件。共享 Motion、Action 和焦点辅助继续放在 `components/`
下。组件通过回调或 `App_UiComponent_BindAction()` 暴露交互，页面负责组合组件，
不应直接访问组件内部对象树。完整约定见
[`components/widgets/README.md`](src/lvgl_app/components/widgets/README.md)。

## 单元测试

```powershell
cmake -S tests -B build_tests
cmake --build build_tests
ctest --test-dir build_tests --output-on-failure
```

测试覆盖通用 Action、Model 消息事件和导航栈。构建过程还会使用声明桩编译
LVGL 资源、主题、组件、View 和页面源码，并检查自动生成资源是否为最新版本；
这些本地检查不需要 SDL、Emscripten 或完整 LVGL 链接。

## 迁移到 ESP-IDF

将 `src/lvgl_app/` 复制到 ESP-IDF 工程的 `components/lvgl_app/`。初始化 LVGL 显示
和输入设备后调用：

```c
#include "App_Ui.h"

if (App_UiInit()) {
    App_UiStart();
}
```

在真实硬件上，需要在 `port/esp32/App_UiPort_Esp32.c` 中接入显示、输入和屏幕刷新
策略。Action 已包含在该组件内，不再依赖外部 `app_action` 组件。
