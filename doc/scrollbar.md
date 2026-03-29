# ScrollBar

## 概述

`darkui::ScrollBar` 是一个自定义暗色滚动条，支持横向和纵向模式，并保留标准 Win32 滚动通知方式。

## 头文件与实现

- `include/darkui/scrollbar.h`
- `src/scrollbar.cpp`

## 适用场景

- 自定义内容区域滚动
- 暗色侧栏滚动
- 与自绘控件或自定义画布配合使用

## 主要能力

- 横向和纵向滚动条
- 自定义逻辑范围
- 自定义页大小
- 拖拽滚动块
- 点击轨道翻页
- 键盘方向键、Home、End

## 创建方式

```cpp
#include "darkui/scrollbar.h"

darkui::Theme theme;
theme.scrollBarBackground = RGB(24, 27, 31);
theme.scrollBarTrack = RGB(48, 53, 60);
theme.scrollBarThumb = RGB(120, 128, 140);
theme.scrollBarThumbHot = RGB(160, 170, 184);
theme.scrollBarThickness = 14;
theme.scrollBarMinThumbSize = 28;

darkui::ScrollBar verticalBar;
verticalBar.Create(hwnd, 6001, true, theme);
verticalBar.SetRange(0, 100);
verticalBar.SetPageSize(24);
verticalBar.SetValue(56);
MoveWindow(verticalBar.hwnd(), 20, 20, 16, 220, TRUE);
```

## 父窗口消息处理

### 纵向

```cpp
case WM_VSCROLL:
    if (reinterpret_cast<HWND>(lParam) == verticalBar.hwnd()) {
        int value = verticalBar.GetValue();
        return 0;
    }
    break;
```

### 横向

```cpp
case WM_HSCROLL:
    if (reinterpret_cast<HWND>(lParam) == horizontalBar.hwnd()) {
        int value = horizontalBar.GetValue();
        return 0;
    }
    break;
```

## 常用 API

### SetRange

```cpp
scrollBar.SetRange(0, 100);
```

### SetPageSize

```cpp
scrollBar.SetPageSize(20);
```

### SetValue

```cpp
scrollBar.SetValue(40);
scrollBar.SetValue(48, true);
```

### SetTheme

```cpp
scrollBar.SetTheme(theme);
```

## 读取状态

```cpp
int value = scrollBar.GetValue();
int minValue = scrollBar.GetMinimum();
int maxValue = scrollBar.GetMaximum();
int pageSize = scrollBar.GetPageSize();
bool isVertical = scrollBar.vertical();
```

## 主题字段

- `scrollBarBackground`
- `scrollBarTrack`
- `scrollBarThumb`
- `scrollBarThumbHot`
- `scrollBarThickness`
- `scrollBarMinThumbSize`

## 使用建议

- 这个控件只负责滚动条本身，不自动联动内容区域
- 宿主窗口收到滚动值变化后，应自行更新内容偏移
- `pageSize` 会影响滚动块长度和最大可滚范围

## 当前限制

- 不带箭头按钮
- 不支持长按自动连发
- 不提供滚动容器封装
