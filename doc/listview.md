# ListView

`darkui::ListView` is a native Win32 `SysListView32` wrapper with a dark header and dark color setup. It keeps native scrolling, column resizing, selection, and keyboard behavior while applying a restrained dark-mode treatment similar to the reference Android log viewer project.

## Files

- `include/darkui/listview.h`
- `src/listview.cpp`

## Good Fit

- Dark log viewers, result tables, and data panes
- Pages that want native column resize and native scrollbars
- Cases where a restrained dark wrapper is preferred over a fully custom control

## Characteristics

- Native `SysListView32`
- Dark `DarkMode_Explorer` theme setup
- Custom-painted header
- Native column resize, native scrollbars, native keyboard navigation
- Runtime theme updates through `SetTheme(...)` and `ThemeManager`

## Basic Example

```cpp
#include "darkui/listview.h"

darkui::ListView listView;
darkui::ListView::Options options;
options.columns = {
    {L"Time", 180},
    {L"Level", 80},
    {L"Tag", 160},
    {L"Message", 520},
};
options.rows = {
    {L"12:30:11.120", L"Info", L"Renderer", L"Pipeline ready"},
    {L"12:30:11.422", L"Warn", L"Network", L"Reconnect scheduled"},
};

listView.Create(hwnd, 9001, darkui::MakePresetTheme(), options);
MoveWindow(listView.hwnd(), 24, 24, 720, 320, TRUE);
```

## Notes

- `hwnd()` returns the outer host window.
- `list_hwnd()` returns the inner native `SysListView32`.
- Column widths are fixed pixel widths supplied to the native control.
