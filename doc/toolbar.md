# Toolbar

## 概述

`darkui::Toolbar` 是一个自定义暗色工具栏控件。它支持普通按钮、图标按钮、分隔项、右对齐项、下拉菜单和溢出菜单，适合顶部操作栏和菜单栏风格界面。

## 头文件与实现

- `include/darkui/toolbar.h`
- `src/toolbar.cpp`

## 适用场景

- 顶部工具栏
- 文件管理器风格操作栏
- 菜单栏式下拉操作入口

## 主要能力

- 普通文字按钮
- 图标按钮与 icon-only 按钮
- 分隔项
- 右对齐按钮组
- 下拉菜单
- 空间不足时自动溢出
- 保持 `WM_COMMAND`

## 核心类型

```cpp
struct ToolbarItem {
    std::wstring text;
    int commandId = 0;
    HICON icon = nullptr;
    HMENU popupMenu = nullptr;
    std::uintptr_t userData = 0;
    bool separator = false;
    bool checked = false;
    bool disabled = false;
    bool alignRight = false;
    bool iconOnly = false;
    bool dropDown = false;
};
```

## 创建方式

```cpp
#include "darkui/toolbar.h"

darkui::Theme theme;
theme.toolbarBackground = RGB(25, 28, 32);
theme.toolbarItem = RGB(46, 51, 58);
theme.toolbarItemHot = RGB(64, 71, 82);
theme.toolbarItemActive = RGB(78, 120, 184);
theme.toolbarText = RGB(228, 232, 238);
theme.toolbarTextActive = RGB(248, 250, 252);
theme.toolbarSeparator = RGB(70, 76, 86);
theme.toolbarHeight = 44;

darkui::Toolbar toolbar;
toolbar.Create(hwnd, 8001, theme);
toolbar.SetItems({
    {L"New", 8101, iconNew},
    {L"Open", 8102, iconOpen},
    {L"", 0, nullptr, nullptr, 0, true},
    {L"", 8104, iconSearch, nullptr, 0, false, false, false, false, true},
    {L"Share", 8103, iconShare, nullptr, 0, false, false, false, true},
    {L"Layout", 8105, iconLayout, layoutMenu, 0, false, false, false, true, false, true},
});
MoveWindow(toolbar.hwnd(), 0, 0, 900, 44, TRUE);
```

## 父窗口消息处理

```cpp
case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case 8101:
        return 0;
    case 8102:
        return 0;
    case 8103:
        return 0;
    }
    break;
```

## 常用 API

### SetItems / AddItem / ClearItems

```cpp
toolbar.SetItems(items);
toolbar.AddItem({L"Export", 8104});
toolbar.ClearItems();
```

### SetItem

```cpp
toolbar.SetItem(1, {L"Open", 8102, iconOpen});
```

### SetChecked / SetDisabled

```cpp
toolbar.SetChecked(0, true);
toolbar.SetDisabled(3, true);
```

### SetTheme

```cpp
toolbar.SetTheme(theme);
```

### GetCount / GetItem

```cpp
std::size_t count = toolbar.GetCount();
auto item = toolbar.GetItem(0);
```

## 主题字段

- `toolbarBackground`
- `toolbarItem`
- `toolbarItemHot`
- `toolbarItemActive`
- `toolbarText`
- `toolbarTextActive`
- `toolbarSeparator`
- `toolbarHeight`
- `buttonDisabledText`
- `textPadding`
- `uiFont`

## 使用建议

- `iconOnly = true` 时，仍建议填写 `text`，便于溢出菜单显示
- `dropDown = true` 时需要提供有效 `HMENU`
- `alignRight = true` 适合次级操作区或窗口右侧工具组
- 作为菜单栏使用时，可以配合文本型下拉项

## 下拉与溢出行为

- 下拉菜单使用自定义暗色弹层，不直接显示原生浅色菜单
- 控件宽度不足时，多余项自动进入溢出按钮
- 溢出中会尽量保留 drop-down 层级关系

## 当前限制

- 当前不支持完整键盘导航
- 不提供工具栏自定义拖拽布局
- 不提供内建图标资源管理
