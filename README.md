# Windows_C++_lib_darkui

`Windows_C++_lib_darkui` 是一个面向原生 Win32 的轻量级暗色自定义控件库。它不替代完整 GUI 框架，而是提供一组可以直接嵌入现有 Win32 工程的暗色控件，保持标准 Win32 消息机制，同时统一控件外观、状态反馈和主题风格。

## 项目定位

- 面向原生 Win32 应用
- 适合源码集成，也适合编译为静态库后复用
- 保持 Win32 风格的创建方式、消息机制和父子窗口组织方式
- 重点解决暗色界面下原生控件难以统一样式的问题

## 当前包含的自定义控件

### Button

- 自定义暗色按钮
- 支持普通、悬停、按下、禁用状态
- 支持圆角和宿主表面色
- 保持 `WM_COMMAND + BN_CLICKED` 标准行为

详细用法请查看：[doc/button.md](doc/button.md)

### ComboBox

- 自定义暗色下拉框
- 自绘按钮区域、弹层和列表项
- 支持普通项和强调项
- 保持 `WM_COMMAND + CBN_SELCHANGE` 标准通知

详细用法请查看：[doc/combobox.md](doc/combobox.md)

### Edit

- 自定义暗色输入框
- 外层暗色宿主 + 内层原生 `EDIT`
- 保留原生输入、光标、选择和 IME 行为
- 支持占位文本、圆角、只读模式

详细用法请查看：[doc/edit.md](doc/edit.md)

### ProgressBar

- 自定义暗色进度条
- 分离外层背景、轨道、填充和百分比文字
- 支持宿主表面色
- 适合暗色卡片和面板场景

详细用法请查看：[doc/progress.md](doc/progress.md)

### ScrollBar

- 自定义暗色滚动条
- 支持横向和纵向
- 支持拖拽、翻页和键盘操作
- 保持 `WM_HSCROLL` / `WM_VSCROLL` 标准通知

详细用法请查看：[doc/scrollbar.md](doc/scrollbar.md)

### Slider

- 自定义暗色滑块
- 支持轨道、填充、滑块和刻度样式
- 支持鼠标和键盘操作
- 保持 `WM_HSCROLL` 标准通知

详细用法请查看：[doc/slider.md](doc/slider.md)

### Tab

- 自定义暗色标签页
- 支持水平和垂直布局
- 支持外挂子页面窗口
- 保持 `WM_NOTIFY + TCN_SELCHANGE` 标准通知

详细用法请查看：[doc/tab.md](doc/tab.md)

### Table

- 自定义暗色表格
- 自绘表头、表体、网格和选中状态
- 支持列定义和行数据管理
- 适合展示型数据面板

详细用法请查看：[doc/table.md](doc/table.md)

### Toolbar

- 自定义暗色工具栏
- 支持普通按钮、图标按钮、右对齐项、分隔项
- 支持下拉菜单与溢出菜单
- 保持 `WM_COMMAND` 标准通知

详细用法请查看：[doc/toolbar.md](doc/toolbar.md)

## 主要特点

- 基于统一的 `darkui::Theme` 主题结构
- 多数控件支持运行时调用 `SetTheme()` 切换外观
- 尽量保持 Win32 原生使用习惯
- 控件本身不接管整体布局，布局继续由宿主窗口负责
- 适合逐步替换现有项目中的原生浅色控件

## 头文件入口

统一入口头文件：

```cpp
#include "darkui/darkui.h"
```

也可以按控件单独包含：

```cpp
#include "darkui/button.h"
#include "darkui/combobox.h"
#include "darkui/edit.h"
#include "darkui/progress.h"
#include "darkui/scrollbar.h"
#include "darkui/slider.h"
#include "darkui/tab.h"
#include "darkui/table.h"
#include "darkui/toolbar.h"
```

## 目录结构

```text
Windows_C++_lib_darkui/
  README.md
  include/
    darkui/
      *.h
  src/
    *.cpp
  demo/
    src/
      demo_*.cpp
    build/
    build_demo*.bat
  doc/
    button.md
    combobox.md
    edit.md
    progress.md
    scrollbar.md
    slider.md
    tab.md
    table.md
    toolbar.md
```

## 集成方式

推荐两种方式：

- 源码集成：把 `include/darkui` 和 `src` 并入你的 Win32 工程
- 静态库集成：先编译成库，再由其他项目链接

如果你只想使用 demo 构建脚本，当前 `demo/build_demo*.bat` 已经改成直接编译源码，不依赖 `CMakeLists.txt`。

## 文档索引

- [Button 文档](doc/button.md)
- [ComboBox 文档](doc/combobox.md)
- [Edit 文档](doc/edit.md)
- [ProgressBar 文档](doc/progress.md)
- [ScrollBar 文档](doc/scrollbar.md)
- [Slider 文档](doc/slider.md)
- [Tab 文档](doc/tab.md)
- [Table 文档](doc/table.md)
- [Toolbar 文档](doc/toolbar.md)
