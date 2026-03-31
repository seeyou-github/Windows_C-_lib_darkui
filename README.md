# Windows_C++_lib_darkui

`Windows_C++_lib_darkui` is a lightweight custom dark-control library for native Win32 applications. It does not try to replace a full GUI framework. Instead, it provides a focused set of dark-themed controls that can be embedded into existing Win32 projects while keeping the standard Win32 message model.

## Project Positioning

- Designed for native Win32 applications
- Suitable for source integration or static-library style reuse
- Keeps Win32-style control creation, message handling, and parent-child window structure
- Focuses on solving dark-theme consistency issues that are difficult to address with stock controls

## Available Custom Controls

### Button

- Custom dark owner-draw button
- Normal, hover, pressed, and disabled states
- Rounded corners and host surface color support
- Preserves `WM_COMMAND + BN_CLICKED`

See: [doc/button.md](doc/button.md)

### ComboBox

- Custom dark combo box
- Custom-painted button area, popup host, and list items
- Supports normal and accent items
- Preserves `WM_COMMAND + CBN_SELCHANGE`

See: [doc/combobox.md](doc/combobox.md)

### Edit

- Custom dark edit control
- Dark host window plus inner native `EDIT`
- Keeps native input, caret, selection, and IME behavior
- Supports placeholder text, rounded shape, read-only mode, and multiline vertical scrolling

See: [doc/edit.md](doc/edit.md)

### Dialog

- Modal dark popup dialog
- Custom black title bar and dark background
- Built-in confirm/cancel buttons and custom content host

See: [doc/dialog.md](doc/dialog.md)

### Static

- Dark static display control
- Supports text, icon, and bitmap presentation
- Suitable for labels, titles, badges, and small previews

See: [doc/static.md](doc/static.md)

### ListBox

- Dark list box
- Rounded host surface plus native keyboard and scroll behavior
- Supports single-select and multi-select usage

See: [doc/listbox.md](doc/listbox.md)

### CheckBox

- Dark owner-drawn checkbox
- Checked, unchecked, hover, and disabled states
- Preserves `WM_COMMAND + BN_CLICKED`

See: [doc/checkbox.md](doc/checkbox.md)

### RadioButton

- Dark owner-drawn radio button
- Native auto-radio grouping behavior
- Preserves `WM_COMMAND + BN_CLICKED`

See: [doc/radiobutton.md](doc/radiobutton.md)

### ProgressBar

- Custom dark progress bar
- Separate outer background, inner track, fill, and percentage text
- Supports host surface color
- Works well inside dark cards and panels

See: [doc/progress.md](doc/progress.md)

### ScrollBar

- Custom dark scrollbar
- Horizontal and vertical modes
- Drag, page-step, and keyboard support
- Preserves `WM_HSCROLL` / `WM_VSCROLL`

See: [doc/scrollbar.md](doc/scrollbar.md)

### Slider

- Custom dark slider
- Custom track, fill, thumb, and tick rendering
- Mouse and keyboard interaction
- Preserves `WM_HSCROLL`

See: [doc/slider.md](doc/slider.md)

### Tab

- Custom dark tab control
- Horizontal and vertical layouts
- Supports attached child-page windows
- Preserves `WM_NOTIFY + TCN_SELCHANGE`

See: [doc/tab.md](doc/tab.md)

### Table

- Custom dark table control
- Custom header, body, grid, and selection rendering
- Column and row data management
- Suitable for presentation-oriented data panels

See: [doc/table.md](doc/table.md)

### Toolbar

- Custom dark toolbar
- Supports standard buttons, icon buttons, right-aligned items, separators, drop-downs, and overflow
- Icon sizing uses `ToolbarItem::iconScalePercent` with a default of `80%`, preserving aspect ratio
- Use `theme.toolbarHeight` together with `MoveWindow(..., theme.toolbarHeight + 12, ...)` for the intended button height
- Preserves `WM_COMMAND`

See: [doc/toolbar.md](doc/toolbar.md)

## Main Characteristics

- Built around a shared `darkui::Theme` structure
- Most controls support runtime theme switching through `SetTheme()`
- Keeps native Win32 usage patterns where practical
- Control layout remains the responsibility of the host window
- Suitable for gradually replacing stock light-themed controls in existing projects

## Header Entry Points

Unified include:

```cpp
#include "darkui/darkui.h"
```

Per-control includes:

```cpp
#include "darkui/checkbox.h"
#include "darkui/button.h"
#include "darkui/combobox.h"
#include "darkui/dialog.h"
#include "darkui/edit.h"
#include "darkui/listbox.h"
#include "darkui/progress.h"
#include "darkui/radiobutton.h"
#include "darkui/scrollbar.h"
#include "darkui/slider.h"
#include "darkui/static.h"
#include "darkui/tab.h"
#include "darkui/table.h"
#include "darkui/toolbar.h"
```

## Directory Layout

```text
Windows_C++_lib_darkui/
  README.md
  include/
    darkui/
      *.h
  src/
    *.cpp
  demo/
    src/
      demo_*.cpp
    build/
    build_demo*.bat
  doc/
    button.md
    checkbox.md
    combobox.md
    dialog.md
    edit.md
    listbox.md
    progress.md
    radiobutton.md
    scrollbar.md
    slider.md
    static.md
    tab.md
    table.md
    toolbar.md
```

## Integration Options

Recommended approaches:

- Source integration: copy or include `include/darkui` and `src` in your Win32 project
- Static-library integration: build once, then link from other projects

If you only need the demo build scripts, the current `demo/build_demo*.bat` files already compile sources directly and do not depend on `CMakeLists.txt`.

## Documentation Index

- [Button](doc/button.md)
- [CheckBox](doc/checkbox.md)
- [ComboBox](doc/combobox.md)
- [Dialog](doc/dialog.md)
- [Edit](doc/edit.md)
- [ListBox](doc/listbox.md)
- [ProgressBar](doc/progress.md)
- [RadioButton](doc/radiobutton.md)
- [ScrollBar](doc/scrollbar.md)
- [Slider](doc/slider.md)
- [Static](doc/static.md)
- [Tab](doc/tab.md)
- [Table](doc/table.md)
- [Toolbar](doc/toolbar.md)
