# lib_darkui

`lib_darkui` 是一个面向原生 Win32 的轻量级自定义控件库。
这个库的目标不是封装整套 GUI 框架，而是提供一组“可直接嵌入原生 Win32 工程”的暗黑风格自定义控件。

## 当前能力

- 自定义暗黑下拉框 `darkui::ComboBox`
- 可自定义背景色、按钮色、边框色、文字色、箭头色
- 下拉列表项支持普通项和强调项两种视觉状态
- 通过标准 `WM_COMMAND + CBN_SELCHANGE` 通知父窗口
- 调用方式保持 Win32 风格，便于嵌入现有项目

## 目录结构

```text
lib_darkui/
  CMakeLists.txt
  build_demo.bat
  build_demo_combobox_only.bat
  README.md
  include/
    darkui/
      darkui.h
      combobox.h
  src/
    combobox.cpp
  demo/
    demo_combobox.cpp
    demo_combobox_only.cpp
```

## 设计说明

当前这个下拉框不是系统原生 `COMBOBOX`，而是由以下部分组合而成：

- 一个 owner-draw `BUTTON`，负责显示当前选中项
- 一个自绘宿主窗口，负责承载下拉层背景和边框
- 一个 owner-draw `LISTBOX`，负责绘制下拉项

这样做的主要原因是：Win32 原生 `COMBOBOX` 在暗黑模式下很难完全去掉系统浅色区域、边框和箭头按钮，而自定义组合后可控性更高。

## 头文件与命名空间

统一入口头文件：

```cpp
#include "darkui/darkui.h"
```

它当前会引入：

```cpp
#include "darkui/combobox.h"
```

主要公开类型：

- `darkui::FontSpec`
- `darkui::Theme`
- `darkui::ComboItem`
- `darkui::ComboBox`

## 接入步骤

### 1. 把库文件加入你的工程

最少需要这几个文件：

- `include/darkui/darkui.h`
- `include/darkui/combobox.h`
- `src/combobox.cpp`

如果你是直接用 `g++`：

```powershell
g++ your_app.cpp lib_darkui/src/combobox.cpp -Ilib_darkui/include -std=c++17 -DUNICODE -D_UNICODE -lcomctl32 -o your_app.exe
```

如果你是 CMake：

```cmake
add_executable(YourApp your_app.cpp lib_darkui/src/combobox.cpp)
target_include_directories(YourApp PRIVATE lib_darkui/include)
target_compile_definitions(YourApp PRIVATE UNICODE _UNICODE)
target_link_libraries(YourApp PRIVATE comctl32)
```

### 2. 在父窗口里持有一个 `darkui::ComboBox`

通常做法是把它放进窗口状态结构体里：

```cpp
struct WindowState {
    darkui::Theme theme;
    darkui::ComboBox formatCombo;
};
```

### 3. 创建主题

```cpp
darkui::Theme MakeTheme() {
    darkui::Theme theme;
    theme.background = RGB(23, 26, 31);
    theme.panel = RGB(37, 42, 49);
    theme.button = RGB(57, 66, 78);
    theme.buttonHot = RGB(74, 85, 99);
    theme.border = RGB(71, 79, 92);
    theme.text = RGB(231, 235, 240);
    theme.mutedText = RGB(160, 168, 178);
    theme.arrow = RGB(160, 168, 178);
    theme.popupItem = RGB(37, 42, 49);
    theme.popupItemHot = RGB(57, 66, 78);
    theme.popupAccentItem = RGB(28, 54, 95);
    theme.popupAccentItemHot = RGB(42, 75, 126);
    theme.uiFont.family = L"Segoe UI";
    theme.uiFont.height = -20;
    return theme;
}
```

### 4. 在 `WM_CREATE` 中创建下拉框

```cpp
case WM_CREATE: {
    auto* state = new WindowState();
    state->theme = MakeTheme();

    state->formatCombo.Create(hwnd, 1001, state->theme);
    state->formatCombo.SetItems({
        {L"bestaudio", 1, false},
        {L"bestvideo", 2, true},
        {L"137 - mp4 1080p", 3, true},
        {L"140 - m4a 128k", 4, false},
    });
    state->formatCombo.SetSelection(0);

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    return 0;
}
```

### 5. 在 `WM_SIZE` 里布置控件位置

库只负责控件本身，不接管布局。你仍然需要自己 `MoveWindow`：

```cpp
case WM_SIZE:
    if (state) {
        MoveWindow(state->formatCombo.hwnd(), 40, 40, 320, 40, TRUE);
    }
    return 0;
```

### 6. 在父窗口处理选中变化

当用户选择新项时，父窗口会收到：

- `WM_COMMAND`
- `LOWORD(wParam) == 你的控件 ID`
- `HIWORD(wParam) == CBN_SELCHANGE`

示例：

```cpp
case WM_COMMAND:
    if (state && LOWORD(wParam) == 1001 && HIWORD(wParam) == CBN_SELCHANGE) {
        const int index = state->formatCombo.GetSelection();
        const auto item = state->formatCombo.GetItem(index);
        SetWindowTextW(hwnd, item.text.c_str());
        return 0;
    }
    break;
```

### 7. 在销毁窗口时释放状态

```cpp
case WM_DESTROY:
    delete state;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
    PostQuitMessage(0);
    return 0;
```

`ComboBox` 自身会在析构时调用 `Destroy()`，一般不需要你手工销毁内部子窗口。

## 最小可运行示例

下面这段就是接入时最常见的写法：

```cpp
#include <windows.h>
#include <commctrl.h>
#include "darkui/darkui.h"

struct WindowState {
    darkui::Theme theme;
    darkui::ComboBox combo;
};

darkui::Theme MakeTheme() {
    darkui::Theme theme;
    theme.panel = RGB(37, 42, 49);
    theme.button = RGB(57, 66, 78);
    theme.buttonHot = RGB(74, 85, 99);
    theme.border = RGB(71, 79, 92);
    theme.text = RGB(231, 235, 240);
    theme.arrow = RGB(160, 168, 178);
    return theme;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<WindowState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE:
        state = new WindowState();
        state->theme = MakeTheme();
        state->combo.Create(hwnd, 1001, state->theme);
        state->combo.SetItems({
            {L"default", 0, false},
            {L"mp4", 1, true},
            {L"mp3", 2, false},
        });
        state->combo.SetSelection(0);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
        return 0;

    case WM_SIZE:
        if (state) {
            MoveWindow(state->combo.hwnd(), 20, 20, 280, 38, TRUE);
        }
        return 0;

    case WM_COMMAND:
        if (state && LOWORD(wParam) == 1001 && HIWORD(wParam) == CBN_SELCHANGE) {
            const auto item = state->combo.GetItem(state->combo.GetSelection());
            SetWindowTextW(hwnd, item.text.c_str());
            return 0;
        }
        break;

    case WM_DESTROY:
        delete state;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
```

## `ComboBox` 调用方法

### `bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = ..., DWORD exStyle = 0)`

创建控件。

参数说明：

- `parent`：父窗口句柄
- `controlId`：控件 ID，用于 `WM_COMMAND` 识别来源
- `theme`：当前控件使用的主题
- `style`：按钮基础样式，内部会自动补 `BS_OWNERDRAW`
- `exStyle`：扩展样式

示例：

```cpp
combo.Create(hwnd, 1001, theme);
```

### `void SetItems(const std::vector<ComboItem>& items)`

一次性设置全部项目，并把当前选择重置到第 0 项。

示例：

```cpp
combo.SetItems({
    {L"bestaudio", 1, false},
    {L"bestvideo", 2, true},
    {L"251 - webm opus", 3, false},
});
```

### `void AddItem(const ComboItem& item)`

在现有列表后追加一项。

```cpp
combo.AddItem({L"140 - m4a 128k", 4, false});
```

### `void ClearItems()`

清空所有项目。

```cpp
combo.ClearItems();
```

### `void SetSelection(int index, bool notify = false)`

设置当前选中项。

- `index`：目标索引
- `notify`：是否立即向父窗口发送 `CBN_SELCHANGE`

```cpp
combo.SetSelection(2);
combo.SetSelection(1, true);
```

### `int GetSelection() const`

获取当前选中索引。

```cpp
int index = combo.GetSelection();
```

### `ComboItem GetItem(int index) const`

获取指定项的数据。

```cpp
auto item = combo.GetItem(combo.GetSelection());
```

### `std::wstring GetText() const`

获取当前选中项文本。

```cpp
std::wstring text = combo.GetText();
```

### `std::size_t GetCount() const`

获取项目总数。

```cpp
std::size_t count = combo.GetCount();
```

### `void SetTheme(const Theme& theme)`

运行时切换主题。

```cpp
darkui::Theme newTheme = theme;
newTheme.button = RGB(90, 44, 61);
newTheme.text = RGB(255, 225, 233);
newTheme.arrow = RGB(241, 170, 194);
combo.SetTheme(newTheme);
```

## `Theme` 字段说明

### 颜色字段

- `background`：通常给顶层窗口使用的背景色，控件内部不直接使用它绘制按钮
- `panel`：下拉框默认面板底色、popup 宿主背景色
- `button`：按钮常态背景色
- `buttonHot`：按钮按下/激活态背景色
- `border`：按钮边框和 popup 边框颜色
- `text`：主文字颜色
- `mutedText`：弱化文字色，当前主要用于你自定义主题时参考
- `arrow`：右侧倒三角箭头颜色
- `popupItem`：下拉项常态背景色
- `popupItemHot`：下拉项选中/高亮背景色
- `popupAccentItem`：强调项常态背景色
- `popupAccentItemHot`：强调项高亮背景色

### 字体字段

- `uiFont.family`：字体名
- `uiFont.height`：字体高度，负值表示逻辑高度
- `uiFont.weight`：字重，如 `FW_NORMAL`、`FW_BOLD`
- `uiFont.italic`：是否斜体
- `uiFont.monospace`：是否偏向等宽字体

### 尺寸字段

- `itemHeight`：下拉项高度
- `arrowWidth`：箭头绘制宽度
- `arrowRightPadding`：箭头距右边距
- `textPadding`：文字左右内边距
- `popupBorder`：popup 边框厚度
- `popupOffsetY`：popup 相对按钮的垂直偏移

## `ComboItem` 字段说明

```cpp
struct ComboItem {
    std::wstring text;
    std::uintptr_t userData = 0;
    bool accent = false;
};
```

- `text`：显示文本
- `userData`：业务侧自定义附加值，可存索引、枚举值、ID
- `accent`：是否按强调项颜色绘制

使用举例：

```cpp
combo.SetItems({
    {L"default", 0, false},
    {L"mp4", 1, true},
    {L"mp3", 2, false},
});
```

## 常见使用场景

### 场景 1：把枚举值存进 `userData`

```cpp
enum OutputFormat {
    FORMAT_DEFAULT = 0,
    FORMAT_MP4 = 1,
    FORMAT_MP3 = 2,
};

combo.SetItems({
    {L"default", FORMAT_DEFAULT, false},
    {L"mp4", FORMAT_MP4, true},
    {L"mp3", FORMAT_MP3, false},
});

auto item = combo.GetItem(combo.GetSelection());
OutputFormat format = static_cast<OutputFormat>(item.userData);
```

### 场景 2：动态切换主题

```cpp
darkui::Theme teal = theme;
teal.button = RGB(30, 74, 80);
teal.buttonHot = RGB(40, 97, 105);
teal.border = RGB(71, 145, 150);
teal.text = RGB(213, 246, 242);
teal.arrow = RGB(116, 220, 210);
combo.SetTheme(teal);
```

### 场景 3：在一个窗口里放多个下拉框

```cpp
darkui::ComboBox videoCombo;
darkui::ComboBox audioCombo;
darkui::ComboBox outputCombo;

videoCombo.Create(hwnd, 1001, videoTheme);
audioCombo.Create(hwnd, 1002, audioTheme);
outputCombo.Create(hwnd, 1003, outputTheme);
```

然后在 `WM_COMMAND` 中根据 `controlId` 分别处理：

```cpp
case WM_COMMAND:
    if (HIWORD(wParam) == CBN_SELCHANGE) {
        switch (LOWORD(wParam)) {
        case 1001:
            break;
        case 1002:
            break;
        case 1003:
            break;
        }
    }
    break;
```

## 编译 demo

### MinGW-w64

完整 demo：

```powershell
.\build_demo.bat
```

输出：

```text
build\darkui_combobox_demo.exe
```

多主题最小 demo：

```powershell
.\build_demo_combobox_only.bat
```

输出：

```text
build\darkui_combobox_only_demo.exe
```

### CMake

```powershell
cmake -S . -B build
cmake --build build --target darkui_combobox_demo
```

## 注意事项

- 这个库目前只封装控件本身，不负责窗口布局系统
- 父窗口背景仍建议由调用方自己绘制
- 选中事件仍然遵循 Win32 标准消息机制，而不是自定义回调
- 一个控件类对应一组独立源文件，后续新增控件建议继续保持这个结构
- 当前 `Theme::background` 主要是给 demo 或宿主窗口参考，不等于控件按钮底色

## Table Control

`darkui::Table` is a custom dark table control for Win32. It is fully custom-painted, so the body background, header background, text, and grid colors remain consistent in dark themes.

### Files

- `include/darkui/table.h`
- `src/table.cpp`
- `demo/demo_table.cpp`
- `build_demo_table.bat`

### Public Types

```cpp
struct TableColumn {
    std::wstring text;
    int width = 120;
    int format = LVCFMT_LEFT;
};

using TableRow = std::vector<std::wstring>;
```

### Create

```cpp
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
```

### Columns And Rows

`TableColumn::width` is used as a relative width weight. The control distributes the available width proportionally, and the last column absorbs the remaining pixels.

```cpp
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
```

### Runtime API

```cpp
table.AddRow({L"Theme", L"Config", L"Live", L"Updated at runtime"});
table.ClearRows();
table.SetTheme(theme);
table.SetDrawEmptyGrid(false);
```

`SetDrawEmptyGrid(bool enabled)` controls whether the empty expanded area below the last data row continues drawing grid lines.

- `true`: keep drawing grid lines in the empty expanded area
- `false`: leave the empty expanded area as plain `tableBackground`
- default: `true`

### Theme Fields Used By Table

- `tableBackground`: body background color
- `tableText`: body text color
- `tableHeaderBackground`: header background color
- `tableHeaderText`: header text color
- `tableGrid`: grid and border color
- `tableRowHeight`: body row height
- `tableHeaderHeight`: header row height
- `textPadding`: cell text left/right padding
- `buttonHot`: selected row background color

### Limitations

- Current table behavior focuses on display and single-row selection.
- It does not yet expose built-in scrolling, sorting, or in-place editing.

### Table Demo

The table demo includes a live checkbox named `Draw grid in empty expanded area` so you can switch this behavior at runtime.

```powershell
.\build_demo_table.bat
```

Output:

```text
build\darkui_table_demo.exe
```

## Button Control

`darkui::Button` is an owner-draw dark button for Win32. It keeps the standard `WM_COMMAND + BN_CLICKED` behavior while allowing full control over button, hover/pressed, border, and text colors through `darkui::Theme`.

### Files

- `src/button.cpp`
- `demo/demo_button.cpp`
- `build_demo_button.bat`

### Create

```cpp
darkui::Theme theme;
theme.button = RGB(52, 61, 72);
theme.buttonHot = RGB(72, 86, 104);
theme.border = RGB(84, 96, 112);
theme.text = RGB(232, 236, 241);

darkui::Button button;
button.Create(hwnd, 3001, L"Run", theme);
button.SetCornerRadius(14);
button.SetSurfaceColor(theme.panel);
```

### Runtime API

```cpp
button.SetText(L"Processing...");
button.SetTheme(theme);
button.SetCornerRadius(18);
button.SetSurfaceColor(theme.panel);
std::wstring text = button.GetText();
```

`SetSurfaceColor(COLORREF color)` is useful when a rounded button sits on a card or panel whose color differs from `theme.background`. It fills the corner area outside the rounded body with the host surface color, preventing visible seams around the button.

### Click Handling

The button uses the normal Win32 notification path:

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 3001 && HIWORD(wParam) == BN_CLICKED) {
        // handle click
        return 0;
    }
    break;
```

### Theme Fields Used By Button

- `button`: normal background color
- `buttonHover`: hover background color
- `buttonHot`: pressed background color
- `buttonDisabled`: disabled background color
- `buttonDisabledText`: disabled text color
- `border`: border color
- `text`: button text color
- `textPadding`: inner horizontal padding
- `uiFont`: button font
- `background`: default host surface color used by `Create()`

### Notes

- Rounded buttons painted on custom cards should usually call `SetSurfaceColor(cardColor)`.
- If you do not call `SetSurfaceColor()`, the button defaults to `theme.background` as its host surface fallback.

### Button Demo

```powershell
.\build_demo_button.bat
```

Output:

```text
build\darkui_button_demo.exe
```

## Slider Control

`darkui::Slider` is a custom horizontal dark slider for Win32. It is fully custom-painted and currently supports track, fill, thumb, outer background, and optional tick marks.

### Files

- `include/darkui/slider.h`
- `src/slider.cpp`
- `demo/demo_slider.cpp`
- `build_demo_slider.bat`

### Create

```cpp
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
```

### Runtime API

```cpp
slider.SetTheme(theme);
slider.SetRange(0, 100);
slider.SetValue(50);
slider.SetValue(60, true);
slider.SetShowTicks(true);
slider.SetTickCount(11);
```

`SetValue(value, true)` sends `WM_HSCROLL` to the parent window with `SB_THUMBPOSITION`.

### Parent Notification

The slider uses the standard Win32 notification path through `WM_HSCROLL`:

```cpp
case WM_HSCROLL:
    if (reinterpret_cast<HWND>(lParam) == slider.hwnd()) {
        int value = slider.GetValue();
        return 0;
    }
    break;
```

### Theme Fields Used By Slider

- `sliderBackground`: outer control background color
- `sliderTrack`: track background color
- `sliderFill`: filled progress color
- `sliderThumb`: thumb normal color
- `sliderThumbHot`: thumb hover/drag color
- `sliderTick`: tick mark color
- `sliderTrackHeight`: track thickness
- `sliderThumbRadius`: thumb radius

### Current Scope

- Horizontal slider only
- Mouse click and drag
- Keyboard arrows, `Home`, `End`
- Optional tick marks
- Standard `WM_HSCROLL` parent notification

Not included yet:

- Vertical slider
- Disabled-state visuals
- Value bubble / tooltip
- Custom step size

### Slider Demo

The slider demo uses custom `darkui::Button` controls on the right side to modify slider colors and dimensions in real time.

```powershell
.\build_demo_slider.bat
```

Output:

```text
build\darkui_slider_demo.exe
```

## ProgressBar Control

`darkui::ProgressBar` is a custom dark progress bar for Win32. It is fully custom-painted and separates the outer control background, track background, fill color, and percentage text color.

### Files

- `include/darkui/progress.h`
- `src/progress.cpp`
- `demo/demo_progress.cpp`
- `build_demo_progress.bat`

### Create

```cpp
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
```

### Runtime API

```cpp
progress.SetTheme(theme);
progress.SetRange(0, 100);
progress.SetValue(72);
progress.SetShowPercentage(true);
progress.SetSurfaceColor(theme.panel);
```

`SetSurfaceColor(COLORREF color)` controls the host/card color around the inner progress track. Use it when the progress bar sits on a panel whose color differs from `theme.background`.

### Theme Fields Used By ProgressBar

- `progressBackground`: outer track shell color
- `progressTrack`: inner recessed track color
- `progressFill`: filled progress color
- `progressText`: percentage text color
- `progressHeight`: track height
- `uiFont`: percentage text font
- `background`: default host surface color used by `Create()`

### Notes

- The full control rectangle is filled with the host surface color first.
- The visible progress track is then painted inside that surface using `progressBackground` and `progressTrack`.
- Rounded cards or custom panels should usually call `SetSurfaceColor(cardColor)`.

### Current Scope

- Horizontal progress bar
- Fixed-value display
- Optional centered percentage text
- Custom outer background and inner track colors

Not included yet:

- Rounded corners
- Gradient fill
- Indeterminate / marquee mode
- Animation
- Parent notifications

### ProgressBar Demo

The demo includes `-10`, `+10`, and `Toggle Percent` buttons to inspect the control visually.

```powershell
.\build_demo_progress.bat
```

Output:

```text
build\darkui_progress_demo.exe
```

## ScrollBar Control

`darkui::ScrollBar` is a custom dark scrollbar for Win32. It supports both horizontal and vertical modes, custom track and thumb colors, drag interaction, page jump, keyboard control, and standard scroll notifications.

### Files

- `include/darkui/scrollbar.h`
- `src/scrollbar.cpp`
- `demo/demo_scrollbar.cpp`
- `build_demo_scrollbar.bat`

### Create

```cpp
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

// horizontal
// scrollBar.Create(hwnd, 6002, false, theme);
```

### Runtime API

```cpp
scrollBar.SetTheme(theme);
scrollBar.SetRange(0, 100);
scrollBar.SetPageSize(20);
scrollBar.SetValue(40);
scrollBar.SetValue(48, true);
```

`SetValue(value, true)` sends a standard parent notification:
- `WM_VSCROLL` for vertical scrollbars
- `WM_HSCROLL` for horizontal scrollbars

### Parent Notification

```cpp
case WM_VSCROLL:
    if (reinterpret_cast<HWND>(lParam) == verticalBar.hwnd()) {
        int value = verticalBar.GetValue();
        return 0;
    }
    break;

case WM_HSCROLL:
    if (reinterpret_cast<HWND>(lParam) == horizontalBar.hwnd()) {
        int value = horizontalBar.GetValue();
        return 0;
    }
    break;
```

### Theme Fields Used By ScrollBar

- `scrollBarBackground`: outer scrollbar background color
- `scrollBarTrack`: inner track color
- `scrollBarThumb`: thumb normal color
- `scrollBarThumbHot`: thumb hover and drag color
- `scrollBarThickness`: recommended control thickness
- `scrollBarMinThumbSize`: minimum thumb length

### Current Scope

- Horizontal and vertical modes
- Thumb drag
- Click track for page movement
- Keyboard arrows, `Home`, `End`
- Standard `WM_HSCROLL` / `WM_VSCROLL` notifications

Not included yet:

- Arrow buttons
- Rounded thumb
- Disabled-state visuals
- Auto-repeat on hold
- Linked content container helper

### ScrollBar Demo

The demo shows one horizontal and one vertical scrollbar and prints the current values in real time.

```powershell
.\build_demo_scrollbar.bat
```

Output:

```text
build\darkui_scrollbar_demo.exe
```

## Tab Control

`darkui::Tab` is a custom dark tab control for Win32. It draws its own tab strip, supports external child pages, and sends a standard `TCN_SELCHANGE` notification through `WM_NOTIFY`.

### Files

- `include/darkui/tab.h`
- `src/tab.cpp`
- `demo/demo_tab.cpp`
- `build_demo_tab.bat`

### Create

```cpp
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
```

### Attach Pages

```cpp
HWND pageOne = CreateWindowExW(0, L"STATIC", L"Overview page", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);
HWND pageTwo = CreateWindowExW(0, L"STATIC", L"Metrics page", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);
HWND pageThree = CreateWindowExW(0, L"STATIC", L"Notes page", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, tab.hwnd(), nullptr, instance, nullptr);

tab.AttachPage(0, pageOne);
tab.AttachPage(1, pageTwo);
tab.AttachPage(2, pageThree);
tab.SetSelection(0);
```

### Runtime API

```cpp
tab.SetTheme(theme);
tab.AddItem({L"Logs", 4});
tab.SetSelection(1, true);
int index = tab.GetSelection();
RECT contentRect = tab.GetContentRect();
```

### Parent Notification

The tab control uses the standard `WM_NOTIFY + TCN_SELCHANGE` path:

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

### Theme Fields Used By Tab

- `tabBackground`: tab control background color
- `tabItem`: inactive tab background color
- `tabItemActive`: active tab background color
- `tabText`: inactive tab text color
- `tabTextActive`: active tab text color
- `tabHeight`: tab strip height
- `background`: content area background color
- `text`: content child text color used by `WM_CTLCOLORSTATIC`
- `textPadding`: horizontal text padding inside each tab item
- `uiFont`: tab text font

### Current Scope

- Custom tab strip drawing
- Mouse click selection
- Keyboard `Left`, `Right`, `Home`, `End`
- External child-page attachment
- Standard `TCN_SELCHANGE` parent notification
- Content area uses `theme.background`

Not included yet:

- Scrollable tab strip
- Close buttons
- Drag reordering
- Per-tab icons
- Disabled tabs

### Tab Demo

The demo shows three tabs with three attached child pages.

```powershell
.\build_demo_tab.bat
```

Output:

```text
build\darkui_tab_demo.exe
```

## Toolbar Control

`darkui::Toolbar` is a custom dark toolbar for Win32. It draws its own background, clickable items, checked state, hover state, separators, and disabled items while keeping a standard `WM_COMMAND` notification path.

### Files

- `include/darkui/toolbar.h`
- `src/toolbar.cpp`
- `demo/demo_toolbar.cpp`
- `build_demo_toolbar.bat`

### Public Types

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

### Create

```cpp
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
```

### Runtime API

```cpp
toolbar.SetTheme(theme);
toolbar.AddItem({L"Export", 8104});
toolbar.SetChecked(0, true);
toolbar.SetDisabled(3, true);
toolbar.SetItem(1, {L"Open", 8102, iconOpen});
std::size_t count = toolbar.GetCount();
```

Runtime notes:

- `SetItems()` replaces the full toolbar model and closes any visible drop-down or overflow popup first.
- `ClearItems()` also closes any visible popup before removing all items.
- `SetTheme()` rebuilds internal brushes, pens, and fonts; if theme resource creation fails, the previous theme stays active.

### Parent Notification

The toolbar sends standard `WM_COMMAND` notifications using each item's `commandId`:

```cpp
case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case 8101:
        return 0;
    case 8102:
        return 0;
    }
    break;
```

### Theme Fields Used By Toolbar

- `toolbarBackground`: toolbar background color
- `toolbarItem`: normal item background color
- `toolbarItemHot`: hover item background color
- `toolbarItemActive`: checked or pressed item background color
- `toolbarText`: normal item text color
- `toolbarTextActive`: active item text color
- `toolbarSeparator`: separator line color
- `toolbarHeight`: recommended toolbar height
- `buttonDisabledText`: disabled item text color
- `textPadding`: horizontal item padding
- `uiFont`: toolbar text font

### Layout And Icons

- `ToolbarItem::icon` draws a small left-side icon before the text.
- `ToolbarItem::alignRight = true` places the item into the right-aligned tool group.
- `ToolbarItem::iconOnly = true` draws a compact square icon button without text.
- Even for `iconOnly` items, keep `text` populated so overflow popups can show a readable label.
- `ToolbarItem::dropDown = true` draws a drop-down arrow and opens `popupMenu`.
- `ToolbarItem::dropDown = true` also responds to keyboard `Enter` and `Space`.
- Left-aligned and right-aligned groups are laid out independently, so secondary actions can stay pinned to the far edge.
- When width is insufficient, hidden items move into an overflow `...` button.

### Drop-Down And Overflow Behavior

- Toolbar drop-downs and overflow menus use a custom dark popup instead of the native light `HMENU` UI.
- Overflow preserves drop-down structure: a drop-down item remains a submenu entry inside overflow rather than flattening all child commands into one list.
- Clicking the same drop-down trigger or the same overflow `...` trigger again closes the popup.
- The currently open drop-down trigger stays visually active while its popup is visible.
- `SetItems()` and `ClearItems()` both close any visible popup first, so runtime toolbar rebuilds do not leave stale menu UI behind.
- The same `Toolbar` can be used as a menu-bar style control by combining text-first `dropDown` items with application-owned `HMENU` objects.

### Current Scope

- Clickable toolbar items
- Optional small icons
- Optional icon-only buttons
- Optional drop-down buttons backed by `HMENU`
- Hover and pressed visuals
- Checked state
- Disabled state
- Separator items
- Right-aligned item group
- Automatic overflow handling
- Standard `WM_COMMAND` parent notification

Not included yet:

- Keyboard navigation between items

### Toolbar Demo

The toolbar demo shows icons, icon-only tools, a drop-down button, a right-aligned action group, overflow behavior when the window narrows, and a simple status line updated through `WM_COMMAND`.

```powershell
.\build_demo_toolbar.bat
```

Output:

```text
build\darkui_toolbar_demo.exe
```

### Toolbar Menu-Bar Demo

The menu-bar demo uses `darkui::Toolbar` as a text-first application menu with `File / Edit / View / Window / Help` style drop-downs plus right-aligned actions.

```powershell
.\build_demo_toolbar_menubar.bat
```

Output:

```text
build\darkui_toolbar_menubar_demo.exe
```

## Edit Control

`darkui::Edit` is a custom dark input box for Win32. It uses a lightweight dark host window plus an inner borderless native `EDIT`, so normal caret, selection, keyboard, and IME behavior remain intact while the thin system border line stays hidden.

### Files

- `include/darkui/edit.h`
- `src/edit.cpp`
- `demo/demo_edit.cpp`
- `build_demo_edit.bat`

### Create

```cpp
darkui::Theme theme;
theme.editBackground = RGB(43, 47, 54);
theme.editText = RGB(236, 239, 244);
theme.editPlaceholder = RGB(128, 137, 150);

darkui::Edit edit;
edit.Create(hwnd, 9001, L"", theme);
edit.SetCueBanner(L"Search or enter text");
edit.SetCornerRadius(16);
```

### Runtime API

```cpp
edit.SetTheme(theme);
edit.SetText(L"Updated text");
edit.SetCueBanner(L"Placeholder");
edit.SetCornerRadius(16);
edit.SetReadOnly(false);
std::wstring text = edit.GetText();
```

Runtime notes:

- `SetCornerRadius()` updates both the painted shape and the host window region.
- `SetCueBanner()` updates the custom placeholder overlay immediately.
- `SetReadOnly(true)` keeps the same dark colors as editable mode.

### Parent Notification

The inner native edit forwards standard `EN_*` notifications to the parent through `WM_COMMAND` using the edit control ID:

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 9001 && HIWORD(wParam) == EN_CHANGE) {
        return 0;
    }
    break;
```

For compatibility with normal Win32 edit handling, the forwarded `WM_COMMAND` uses the inner native `EDIT` handle as `lParam`.

### Theme Fields Used By Edit

- `editBackground`: input background color
- `editText`: input text color
- `editPlaceholder`: placeholder text color
- `uiFont`: edit text font

### Notes

- The host window paints the dark background, so no native border line is shown.
- `SetCornerRadius()` controls the rounded outer shape of the host surface.
- The inner `EDIT` is created without `WS_BORDER` or `WS_EX_CLIENTEDGE`.
- `SetCueBanner()` uses a custom placeholder overlay, so color and visibility stay consistent even if `EM_SETCUEBANNER` is unsupported.
- Read-only mode keeps the same dark colors as editable mode.
- `hwnd()` returns the dark host window, and `edit_hwnd()` returns the inner native `EDIT`.

### Edit Demo

The edit demo shows two dark input boxes, cue-banner placeholder text, forwarded `EN_CHANGE` notifications, and a sample button that reads both edit values.

```powershell
.\build_demo_edit.bat
```

Output:

```text
build\darkui_edit_demo.exe
```
