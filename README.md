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
- Supports a unified semantic theme entry through `Theme::useSemanticPalette`
- Keeps native Win32 usage patterns where practical
- Control layout remains the responsibility of the host window
- Suitable for gradually replacing stock light-themed controls in existing projects

## Unified Theme Entry

If you want every control to follow one palette and one typography system, enable semantic palette mode:

```cpp
darkui::Theme theme;
theme.useSemanticPalette = true;
theme.primaryBackground = RGB(20, 22, 26);
theme.secondaryBackground = RGB(32, 36, 42);
theme.primaryText = RGB(228, 232, 238);
theme.highlightText = RGB(248, 250, 252);
theme.accent = RGB(82, 132, 204);
theme.accentSecondary = RGB(48, 86, 148);
theme.fontFamily = L"Segoe UI";
theme.fontSize = 20;
theme.secondaryFontSize = 18;
```

Notes:

- All controls still use the same `Create(..., theme)` and `SetTheme(theme)` entry points
- In semantic mode, darkui expands those values into the detailed per-control colors internally
- Existing per-control theme fields remain available for compatibility when `useSemanticPalette` is left `false`

## Quick Helpers

If the calling side should stay as short as possible, use the helper layer in `darkui/quick.h`.

## Options-Based Creation

Every custom control now provides an `Options` structure plus an overload shaped like:

```cpp
control.Create(parent, controlId, theme, options);
```

This is available for:

- `Button::Options`
- `CheckBox::Options`
- `ComboBox::Options`
- `Dialog::Options`
- `Edit::Options`
- `ListBox::Options`
- `ProgressBar::Options`
- `RadioButton::Options`
- `ScrollBar::Options`
- `Slider::Options`
- `Static::Options`
- `Tab::Options`
- `Table::Options`
- `Toolbar::Options`

Example:

```cpp
darkui::Button button;
darkui::Button::Options options;
options.text = L"Refresh";
options.cornerRadius = 14;
options.surfaceColor = theme.panel;

button.Create(hwnd, 1001, theme, options);
```

Old `Create(...)` overloads remain valid for compatibility.

Ready-made preset theme:

```cpp
darkui::Theme theme = darkui::MakePresetTheme(darkui::ThemePreset::Graphite);
```

Compact semantic theme creation:

```cpp
darkui::Theme theme = darkui::MakeSemanticTheme(
    RGB(20, 22, 26),
    RGB(32, 36, 42),
    RGB(228, 232, 238),
    RGB(248, 250, 252),
    RGB(82, 132, 204),
    RGB(48, 86, 148));
```

Batch theme update:

```cpp
darkui::ApplyTheme(theme, buttonA, buttonB, editA, listBoxA);
```

One-shot dark dialog:

```cpp
darkui::ShowConfirmDialog(
    hwnd,
    5001,
    theme,
    L"Publish Session",
    L"Apply the current settings to every workstation?",
    L"Publish",
    L"Cancel");
```

See: [doc/quick.md](doc/quick.md)

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
#include "darkui/quick.h"
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
- [Quick Helpers](doc/quick.md)
- [RadioButton](doc/radiobutton.md)
- [ScrollBar](doc/scrollbar.md)
- [Slider](doc/slider.md)
- [Static](doc/static.md)
- [Tab](doc/tab.md)
- [Table](doc/table.md)
- [Toolbar](doc/toolbar.md)
