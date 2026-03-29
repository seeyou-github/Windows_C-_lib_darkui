# Slider

## 概述

`darkui::Slider` 是一个自定义暗色滑块控件，用于数值范围选择。它目前为水平模式，并通过标准 `WM_HSCROLL` 通知父窗口。

## 头文件与实现

- `include/darkui/slider.h`
- `src/slider.cpp`

## 适用场景

- 参数调节
- 音量、亮度、曝光、缩放等连续数值控制
- 暗色设置面板

## 主要能力

- 水平滑块
- 自定义轨道、填充、滑块样式
- 可选刻度
- 鼠标拖拽和键盘操作

## 创建方式

```cpp
#include "darkui/slider.h"

darkui::Theme theme;
theme.sliderBackground = RGB(28, 31, 36);
theme.sliderTrack = RGB(52, 56, 62);
theme.sliderFill = RGB(78, 120, 184);
theme.sliderThumb = RGB(224, 227, 232);
theme.sliderThumbHot = RGB(245, 247, 250);
theme.sliderTick = RGB(102, 110, 122);
theme.sliderTrackHeight = 6;
theme.sliderThumbRadius = 10;

darkui::Slider slider;
slider.Create(hwnd, 4001, theme);
slider.SetRange(0, 100);
slider.SetValue(38);
slider.SetShowTicks(true);
slider.SetTickCount(11);
MoveWindow(slider.hwnd(), 20, 20, 320, 80, TRUE);
```

## 父窗口消息处理

```cpp
case WM_HSCROLL:
    if (reinterpret_cast<HWND>(lParam) == slider.hwnd()) {
        int value = slider.GetValue();
        return 0;
    }
    break;
```

## 常用 API

### SetRange

```cpp
slider.SetRange(0, 100);
```

### SetValue

```cpp
slider.SetValue(50);
slider.SetValue(60, true);
```

### SetShowTicks / SetTickCount

```cpp
slider.SetShowTicks(true);
slider.SetTickCount(11);
```

### SetTheme

```cpp
slider.SetTheme(theme);
```

## 读取状态

```cpp
int value = slider.GetValue();
int minValue = slider.GetMinimum();
int maxValue = slider.GetMaximum();
bool showTicks = slider.show_ticks();
int tickCount = slider.tick_count();
```

## 主题字段

- `sliderBackground`
- `sliderTrack`
- `sliderFill`
- `sliderThumb`
- `sliderThumbHot`
- `sliderTick`
- `sliderTrackHeight`
- `sliderThumbRadius`

## 使用建议

- 适合连续数值控制
- 需要即时反馈时，可在 `WM_HSCROLL` 中同步更新业务值
- 想做更细粒度步进时，由业务层自行对值进行换算

## 当前限制

- 仅支持水平模式
- 不支持气泡提示
- 不支持禁用态专用视觉
