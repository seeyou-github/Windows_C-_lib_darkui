# Table

## 概述

`darkui::Table` 是一个自定义暗色表格控件，用于展示型数据。它自绘表头、表体、网格和选中高亮，适合暗色数据面板。

## 头文件与实现

- `include/darkui/table.h`
- `src/table.cpp`

## 适用场景

- 设置列表
- 数据总览表
- 暗色管理后台中的展示表格

## 主要能力

- 自定义表头背景和文字
- 自定义表体背景、文字和网格线
- 支持列定义与行数据
- 支持选中高亮
- 支持是否在空白扩展区域继续绘制网格

## 核心类型

```cpp
struct TableColumn {
    std::wstring text;
    int width = 120;
    int format = LVCFMT_LEFT;
};

using TableRow = std::vector<std::wstring>;
```

## 创建方式

```cpp
#include "darkui/table.h"

darkui::Theme theme;
theme.tableBackground = RGB(25, 28, 33);
theme.tableText = RGB(228, 232, 238);
theme.tableHeaderBackground = RGB(38, 42, 48);
theme.tableHeaderText = RGB(245, 247, 250);
theme.tableGrid = RGB(69, 77, 89);
theme.tableRowHeight = 30;
theme.tableHeaderHeight = 34;

darkui::Table table;
table.Create(hwnd, 2001, theme);
table.SetColumns({
    {L"Name", 180, LVCFMT_LEFT},
    {L"Type", 120, LVCFMT_LEFT},
    {L"State", 100, LVCFMT_CENTER},
    {L"Notes", 260, LVCFMT_LEFT},
});
table.SetRows({
    {L"ComboBox", L"Control", L"Ready", L"Dark popup and button"},
    {L"Table", L"Control", L"Ready", L"Dark header, body and grid"},
});
MoveWindow(table.hwnd(), 20, 20, 640, 360, TRUE);
```

## 常用 API

### SetColumns

```cpp
table.SetColumns({
    {L"Control", 120},
    {L"Area", 120},
    {L"State", 100},
    {L"Notes", 220},
});
```

### SetRows / AddRow / ClearRows

```cpp
table.SetRows(rows);
table.AddRow({L"Theme", L"Config", L"Live", L"Updated at runtime"});
table.ClearRows();
```

### SetDrawEmptyGrid

```cpp
table.SetDrawEmptyGrid(false);
```

### SetTheme

```cpp
table.SetTheme(theme);
```

## 读取状态

```cpp
std::size_t rowCount = table.GetRowCount();
std::size_t columnCount = table.GetColumnCount();
bool drawEmptyGrid = table.draw_empty_grid();
```

## 主题字段

- `tableBackground`
- `tableText`
- `tableHeaderBackground`
- `tableHeaderText`
- `tableGrid`
- `tableRowHeight`
- `tableHeaderHeight`
- `textPadding`
- `buttonHot`

## 使用建议

- 更适合展示型表格，而不是强交互数据表
- 如果你需要排序、分页、编辑，建议在业务层自行扩展

## 当前限制

- 当前没有内建滚动封装
- 不支持列拖动和排序
- 不支持单元格编辑
- 不支持多选
