# ProgressBar

## 概述

`darkui::ProgressBar` 是一个自定义暗色进度条控件。它把宿主背景、外层壳、内层轨道、填充和百分比文字拆开处理，便于在暗色卡片和面板中保持层次。

## 头文件与实现

- `include/darkui/progress.h`
- `src/progress.cpp`

## 适用场景

- 下载进度
- 任务执行进度
- 暗色卡片中的统计条

## 主要能力

- 自定义逻辑范围
- 自定义填充值
- 可显示或隐藏百分比
- 支持宿主表面色

## 创建方式

```cpp
#include "darkui/progress.h"

darkui::Theme theme;
theme.progressBackground = RGB(49, 54, 61);
theme.progressTrack = RGB(36, 40, 46);
theme.progressFill = RGB(78, 120, 184);
theme.progressText = RGB(240, 244, 248);
theme.progressHeight = 18;

darkui::ProgressBar progress;
progress.Create(hwnd, 5001, theme);
progress.SetSurfaceColor(theme.panel);
progress.SetRange(0, 100);
progress.SetValue(64);
MoveWindow(progress.hwnd(), 20, 20, 300, 40, TRUE);
```

## 常用 API

### SetRange

```cpp
progress.SetRange(0, 100);
```

### SetValue

```cpp
progress.SetValue(72);
```

### SetShowPercentage

```cpp
progress.SetShowPercentage(true);
```

### SetSurfaceColor

```cpp
progress.SetSurfaceColor(theme.panel);
```

### SetTheme

```cpp
progress.SetTheme(theme);
```

## 读取状态

```cpp
int value = progress.GetValue();
int minValue = progress.GetMinimum();
int maxValue = progress.GetMaximum();
bool showText = progress.show_percentage();
```

## 主题字段

- `progressBackground`
- `progressTrack`
- `progressFill`
- `progressText`
- `progressHeight`
- `uiFont`
- `background`

## 使用建议

- 如果进度条放在自定义卡片上，建议调用 `SetSurfaceColor(cardColor)`
- 逻辑范围变化后，当前值会被自动夹紧

## 当前限制

- 当前只支持水平模式
- 不支持不确定进度动画
- 不支持渐变填充
- 不发送父窗口通知
