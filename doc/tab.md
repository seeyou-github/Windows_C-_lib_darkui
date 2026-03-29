# Tab

## 概述

`darkui::Tab` 是一个自定义暗色标签页控件。它绘制自己的标签条，并允许应用层把页面窗口挂接到各标签项上。

## 头文件与实现

- `include/darkui/tab.h`
- `src/tab.cpp`

## 适用场景

- 设置窗口
- 多页工作区
- 暗色后台管理界面

## 主要能力

- 水平或垂直标签布局
- 动态增删标签项
- 挂接已有子窗口作为页面
- 保持 `WM_NOTIFY + TCN_SELCHANGE`

## 核心类型

```cpp
struct TabItem {
    std::wstring text;
    std::uintptr_t userData = 0;
};
```

## 创建方式

```cpp
#include "darkui/tab.h"

darkui::Theme theme;
theme.tabBackground = RGB(24, 27, 31);
theme.tabItem = RGB(48, 53, 60);
theme.tabItemActive = RGB(78, 120, 184);
theme.tabText = RGB(206, 211, 218);
theme.tabTextActive = RGB(245, 247, 250);
theme.tabHeight = 38;

darkui::Tab tab;
tab.Create(hwnd, 7001, theme);
tab.SetItems({
    {L"Overview", 1},
    {L"Metrics", 2},
    {L"Notes", 3},
});
MoveWindow(tab.hwnd(), 20, 20, 640, 420, TRUE);
```

## 挂接页面

```cpp
HWND pageOne = CreateWindowExW(0, L"STATIC", L"Overview", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);
HWND pageTwo = CreateWindowExW(0, L"STATIC", L"Metrics", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);
HWND pageThree = CreateWindowExW(0, L"STATIC", L"Notes", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);

tab.AttachPage(0, pageOne);
tab.AttachPage(1, pageTwo);
tab.AttachPage(2, pageThree);
tab.SetSelection(0);
```

## 父窗口消息处理

```cpp
case WM_NOTIFY: {
    auto* hdr = reinterpret_cast<NMHDR*>(lParam);
    if (hdr && hdr->hwndFrom == tab.hwnd() && hdr->code == TCN_SELCHANGE) {
        int index = tab.GetSelection();
        return 0;
    }
    break;
}
```

## 常用 API

### SetVertical

```cpp
tab.SetVertical(true);
```

### SetItems / AddItem / ClearItems

```cpp
tab.SetItems({{L"A", 1}, {L"B", 2}});
tab.AddItem({L"C", 3});
tab.ClearItems();
```

### AttachPage

```cpp
tab.AttachPage(0, pageOne);
```

### SetSelection / GetSelection

```cpp
tab.SetSelection(1, true);
int index = tab.GetSelection();
```

### GetContentRect

```cpp
RECT contentRect = tab.GetContentRect();
```

### SetTheme

```cpp
tab.SetTheme(theme);
```

## 主题字段

- `tabBackground`
- `tabItem`
- `tabItemActive`
- `tabText`
- `tabTextActive`
- `tabHeight`
- `tabWidth`
- `background`
- `textPadding`
- `uiFont`

## 使用建议

- 页面窗口的创建与布局仍由应用层负责
- 垂直模式适合左侧导航
- 水平模式适合传统页签界面

## 当前限制

- 不支持关闭按钮
- 不支持拖拽排序
- 不支持图标标签项
- 不支持禁用项
