# Widgets 独立视觉组件

本目录存放能够被多个页面组合使用的独立 LVGL 视觉组件。组件只负责展示、
局部交互和对象生命周期，不直接读取硬件、网络、蓝牙、时间同步或其他平台服务。
运行时服务应先更新 Model，再由页面或 View 把平台无关的状态传给组件。

共享能力不放在这里：动效令牌和动画辅助位于 `../motion/`，Action 绑定和统一焦点
样式位于 `../App_UiComponents.{c,h}`。完整页面仍位于 `src/lvgl_app/pages/`。

## 目录约定

每个组件使用独立子目录，并至少包含一个公开头文件和一个实现文件：

```text
widgets/
├── action_row/
│   └── App_UiActionRow.{c,h}
├── menu_drawer/
│   └── App_UiMenuDrawer.{c,h}
├── scroll_stack/
│   └── App_UiScrollStack.{c,h}
├── slider_row/
│   └── App_UiSliderRow.{c,h}
├── status_bar/
│   └── App_UiStatusBar.{c,h}
├── toggle_row/
│   └── App_UiToggleRow.{c,h}
└── vertical_menu/
    └── App_UiVerticalMenu.{c,h}
```

| 组件 | 用途 | 所有权与交互 |
| --- | --- | --- |
| Action Row | 标题、副标题和尾部值组成的设置操作行 | 页面提供状态；组件通过稳定 ID 回调上报点击 |
| Menu Drawer | 全局左侧导航抽屉 | View 持有 Context；目标页通过共享 Action 导航 |
| Scroll Stack | 可组合任意控件的纵向滚动容器 | 页面把返回的根对象作为组合入口，并显式配置底部安全空间 |
| Slider Row | 带离散步进和值显示的设置滑杆 | 页面提供范围和值；组件通过稳定 ID 回调上报用户变化 |
| Status Bar | 全局时间、天气及无线状态 | View 提供状态；菜单和状态按钮通过回调上报 |
| Toggle Row | 整行可操作的设置开关 | 页面提供状态；组件通过稳定 ID 和布尔值回调上报变化 |
| Vertical Menu | 可滚动的纵向操作列表 | 页面提供条目；组件通过条目 ID 回调上报点击 |

## API 和状态

- 有状态组件公开实例 Context，以及 `Create` 和 `Update` API。
- 页面或 View 持有 Context，并保证它至少与组件根对象同寿命。
- 组件状态使用普通、平台无关的数据；不要把服务句柄放入组件状态。
- 交互通过回调或共享 Action 层暴露，组件不得直接调用页面导航或平台服务。
- 根对象删除时应清空 Context 中保存的 LVGL 对象引用。
- 组件不得要求调用方访问其内部子对象树。

## 视觉和输入

- 颜色和字体必须通过 `App_UiTheme` 获取。
- 图标和图片必须通过 `App_UiAssets` 获取，不得包含生成资源头文件。
- 可交互对象使用 `App_UiComponent_ApplyFocusStyle()` 保持编码器焦点可见。
- 动画使用 `App_UiMotion`，并遵守 Normal、Reduced 和 Off 三个等级。
- 组件应同时考虑触控、鼠标和编码器输入；可滚动组件要保证焦点项可见。

## 新增组件

1. 在 `widgets/<component_name>/` 创建公开头文件和实现文件。
2. 定义最小平台无关状态、实例 Context、`Create`/`Update` API 和必要回调。
3. 使用 Theme、Assets、Focus Style 和 Motion，不直接调用硬件或服务。
4. 将非聚合 C 源码同时加入根 `CMakeLists.txt`、`src/lvgl_app/CMakeLists.txt`
   和 `tests/CMakeLists.txt` 的声明级编译目标。
5. 更新本文件的组件表，并运行本地测试及 240×320 Web 完整构建。

如果组件在 `lv_layer_top()` 创建覆盖层，其组合所有者负责在自身根对象删除时显式
销毁它，避免覆盖层脱离所有者生命周期继续存在。

## Confirm Dialog

`confirm_dialog/App_UiConfirmDialog.{c,h}` provides a reusable confirmation
surface with caller-owned Context, `Create`/`Update`/`Destroy`, plain text state,
and a `confirmed` callback. It has no service or page dependencies. Cancel is
focused by default. The dialog temporarily redirects grouped input devices,
blocks pointer input below its scrim, and restores the previous group/focus on
close. The composition owner must call `Destroy` before leaving its page and
when its root is deleted. Never reuse a live Context for a second dialog.

Pages may receive the actual Dock overlap through `viewport_changed`; pass
that value to Scroll Stack instead of duplicating fixed Dock measurements.
