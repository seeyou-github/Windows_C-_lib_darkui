# ComboBox

## 概述

`darkui::ComboBox` 是一个自定义暗色下拉框。它不是直接包装原生 `COMBOBOX`，而是由按钮、弹层和列表组合而成，用来获得更稳定的暗色外观控制。

## 头文件与实现

- `include/darkui/combobox.h`
- `src/combobox.cpp`

## 适用场景

- 暗色主题下的格式选择、模式选择、筛选器
- 需要自定义列表项高亮和强调色的下拉框
- 希望继续沿用 `CBN_SELCHANGE` 风格通知的 Win32 工程

## 主要能力

- 自绘按钮区域
- 自绘下拉弹层与列表项
- 支持普通项和强调项
- 支持运行时替换整个项目列表
- 保持 `WM_COMMAND + CBN_SELCHANGE`

## 核心类型

```cpp
struct ComboItem {
    std::wstring text;
    std::uintptr_t userData = 0;
    bool accent = false;
};
```

## 创建方式

```cpp
#include "darkui/combobox.h"

darkui::Theme theme;
theme.panel = RGB(37, 42, 49);
theme.button = RGB(57, 66, 78);
theme.buttonHot = RGB(74, 85, 99);
theme.border = RGB(71, 79, 92);
theme.text = RGB(231, 235, 240);
theme.arrow = RGB(160, 168, 178);
theme.popupItem = RGB(37, 42, 49);
theme.popupItemHot = RGB(57, 66, 78);
theme.popupAccentItem = RGB(28, 54, 95);
theme.popupAccentItemHot = RGB(42, 75, 126);

darkui::ComboBox combo;
combo.Create(hwnd, 1001, theme);
combo.SetItems({
    {L"default", 0, false},
    {L"mp4", 1, true},
    {L"mp3", 2, false},
});
combo.SetSelection(0);
MoveWindow(combo.hwnd(), 20, 20, 280, 38, TRUE);
```

## 父窗口消息处理

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 1001 && HIWORD(wParam) == CBN_SELCHANGE) {
        auto item = combo.GetItem(combo.GetSelection());
        SetWindowTextW(hwnd, item.text.c_str());
        return 0;
    }
    break;
```

## 常用 API

### Create

```cpp
bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, DWORD exStyle = 0);
```

### SetItems

```cpp
combo.SetItems({
    {L"A", 1, false},
    {L"B", 2, true},
});
```

### AddItem

```cpp
combo.AddItem({L"C", 3, false});
```

### ClearItems

```cpp
combo.ClearItems();
```

### SetSelection / GetSelection

```cpp
combo.SetSelection(1);
int index = combo.GetSelection();
```

### GetItem / GetText / GetCount

```cpp
auto item = combo.GetItem(index);
std::wstring text = combo.GetText();
std::size_t count = combo.GetCount();
```

### SetTheme

```cpp
combo.SetTheme(theme);
```

## 主题字段

`ComboBox` 主要使用这些 `Theme` 字段：

- `panel`
- `button`
- `buttonHot`
- `border`
- `text`
- `arrow`
- `popupItem`
- `popupItemHot`
- `popupAccentItem`
- `popupAccentItemHot`
- `uiFont`
- `itemHeight`
- `arrowWidth`
- `arrowRightPadding`
- `textPadding`
- `popupBorder`
- `popupOffsetY`

## 使用建议

- `accent = true` 适合当前推荐项、重点项
- `userData` 可以存放业务枚举值或 ID
- 布局仍由宿主自己负责，通常在 `WM_SIZE` 中 `MoveWindow()`

## 当前限制

- 不支持原生可编辑下拉框
- 不支持多选
- 不带内置搜索
- 不负责复杂数据绑定
