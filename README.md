# lib_darkui

`lib_darkui` 是一个面向原生 Win32 的轻量级自定义控件库。当前正式提供的控件是 `darkui::ComboBox`，后续可以继续按同样模式扩展 `ListBox`、`Button`、`TreeView` 等独立控件类。

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
