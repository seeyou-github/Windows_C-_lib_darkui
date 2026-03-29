# Edit

## 概述

`darkui::Edit` 是一个暗色输入框控件。它通过“暗色宿主窗口 + 内层原生 `EDIT`”的方式，保留原生输入行为，同时去掉原生浅色边框，统一暗色外观。

## 头文件与实现

- `include/darkui/edit.h`
- `src/edit.cpp`

## 适用场景

- 暗色搜索框
- 暗色表单输入框
- 需要保留原生 IME、选区、光标行为的文本输入场景

## 主要能力

- 暗色背景
- 自定义占位文本颜色
- 圆角外形
- 只读模式
- 转发标准 `EN_*` 通知

## 创建方式

```cpp
#include "darkui/edit.h"

darkui::Theme theme;
theme.editBackground = RGB(43, 47, 54);
theme.editText = RGB(236, 239, 244);
theme.editPlaceholder = RGB(128, 137, 150);

darkui::Edit edit;
edit.Create(hwnd, 9001, L"", theme);
edit.SetCueBanner(L"Search or enter text");
edit.SetCornerRadius(16);
MoveWindow(edit.hwnd(), 20, 20, 260, 42, TRUE);
```

## 父窗口消息处理

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 9001 && HIWORD(wParam) == EN_CHANGE) {
        std::wstring text = edit.GetText();
        return 0;
    }
    break;
```

## 常用 API

### Create

```cpp
bool Create(HWND parent, int controlId, const std::wstring& text = L"", const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, DWORD exStyle = 0);
```

### SetTheme

```cpp
edit.SetTheme(theme);
```

### SetText / GetText

```cpp
edit.SetText(L"Updated text");
std::wstring text = edit.GetText();
```

### SetCueBanner

```cpp
edit.SetCueBanner(L"Placeholder");
```

### SetCornerRadius

```cpp
edit.SetCornerRadius(16);
```

### SetReadOnly

```cpp
edit.SetReadOnly(true);
```

### hwnd / edit_hwnd

```cpp
HWND host = edit.hwnd();
HWND nativeEdit = edit.edit_hwnd();
```

## 主题字段

- `editBackground`
- `editText`
- `editPlaceholder`
- `uiFont`

## 使用建议

- 布局时移动的是 `edit.hwnd()`，不是 `edit.edit_hwnd()`
- 需要兼容原生编辑消息时，用 `edit.edit_hwnd()` 作为目标窗口
- 只读模式下仍保留暗色外观，不会切回系统浅色样式

## 当前限制

- 当前偏向单行输入
- 不提供富文本能力
- 不提供内建校验逻辑
- 不负责复杂表单状态管理
