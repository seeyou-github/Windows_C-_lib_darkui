# Button

## 概述

`darkui::Button` 是一个自绘暗色按钮控件。它保留标准 Win32 按钮通知方式，同时允许应用层控制按钮颜色、圆角和宿主表面色。

## 头文件与实现

- `include/darkui/button.h`
- `src/button.cpp`

## 适用场景

- 暗色窗口中的主按钮、次按钮
- 放置在卡片、面板、工具区中的圆角按钮
- 需要保持 `BN_CLICKED` 兼容行为的业务按钮

## 主要能力

- 普通、悬停、按下、禁用四种视觉状态
- 自定义边框颜色和文字颜色
- 自定义圆角
- 支持宿主表面色，避免圆角外侧出现背景接缝

## 创建方式

```cpp
#include "darkui/button.h"

darkui::Theme theme;
theme.button = RGB(52, 61, 72);
theme.buttonHover = RGB(64, 75, 88);
theme.buttonHot = RGB(72, 86, 104);
theme.buttonDisabled = RGB(58, 62, 70);
theme.buttonDisabledText = RGB(138, 144, 152);
theme.border = RGB(84, 96, 112);
theme.text = RGB(232, 236, 241);

darkui::Button button;
button.Create(hwnd, 3001, L"Run", theme);
button.SetCornerRadius(14);
button.SetSurfaceColor(theme.panel);
MoveWindow(button.hwnd(), 20, 20, 140, 40, TRUE);
```

## 父窗口消息处理

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 3001 && HIWORD(wParam) == BN_CLICKED) {
        MessageBoxW(hwnd, L"clicked", L"info", MB_OK);
        return 0;
    }
    break;
```

## 常用 API

### Create

```cpp
bool Create(HWND parent, int controlId, const std::wstring& text, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, DWORD exStyle = 0);
```

### SetTheme

```cpp
button.SetTheme(theme);
```

### SetText / GetText

```cpp
button.SetText(L"Processing...");
std::wstring text = button.GetText();
```

### SetCornerRadius

```cpp
button.SetCornerRadius(18);
```

### SetSurfaceColor

```cpp
button.SetSurfaceColor(theme.panel);
```

作用：

- 设置按钮圆角外侧的宿主背景色
- 当按钮位于卡片、面板上时，建议显式设置

## 主题字段

`Button` 主要使用这些 `Theme` 字段：

- `button`
- `buttonHover`
- `buttonHot`
- `buttonDisabled`
- `buttonDisabledText`
- `border`
- `text`
- `textPadding`
- `uiFont`
- `background`

## 使用建议

- 默认按钮可以直接使用 `theme.background` 作为宿主表面色
- 卡片或面板上的按钮建议调用 `SetSurfaceColor(cardColor)`
- 需要禁用时直接对 `button.hwnd()` 调用 `EnableWindow(..., FALSE)`

## 当前限制

- 不带图标
- 不带下拉箭头
- 不带默认按钮光环样式
- 不提供按钮组管理逻辑
