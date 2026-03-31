# Quick Helpers

## Overview

`include/darkui/quick.h` adds a thin convenience layer on top of the existing control wrappers. It does not replace the normal Win32-style API. Instead, it reduces repeated setup code for the most common tasks:

- creating a ready-made semantic theme
- picking a built-in dark theme preset
- applying one theme to multiple controls
- opening a dark message dialog with one function call

## Files

- `include/darkui/quick.h`

## Theme Presets

Use `ThemePreset` when you want a ready-to-use palette without filling a long `Theme` manually.

```cpp
#include "darkui/darkui.h"

darkui::Theme theme = darkui::MakePresetTheme(darkui::ThemePreset::Graphite);
```

Available presets:

- `Graphite`
- `Ember`
- `Glacier`
- `Moss`
- `Mono`

## Compact Semantic Theme Creation

If you want a custom palette but still prefer semantic theme entry, use `MakeSemanticTheme(...)`:

```cpp
darkui::Theme theme = darkui::MakeSemanticTheme(
    RGB(20, 22, 26),
    RGB(32, 36, 42),
    RGB(228, 232, 238),
    RGB(248, 250, 252),
    RGB(82, 132, 204),
    RGB(48, 86, 148));
```

## Batch Theme Apply

Use `ApplyTheme(...)` when multiple controls need the same runtime theme update:

```cpp
darkui::ApplyTheme(theme, buttonA, buttonB, editA, listBoxA, radioA);
```

This helper calls `SetTheme(theme)` on each control in the parameter list.

## One-Shot Message Dialog

For a simple popup, you do not need to manually construct and configure `darkui::Dialog`.

```cpp
darkui::DialogOptions options;
options.title = L"Delete File";
options.message = L"Delete the selected file permanently?";
options.confirmText = L"Delete";
options.cancelText = L"Cancel";

darkui::Dialog::Result result =
    darkui::ShowMessageDialog(hwnd, 5001, theme, options);
```

## Confirm Dialog Shortcut

```cpp
darkui::Dialog::Result result = darkui::ShowConfirmDialog(
    hwnd,
    5002,
    theme,
    L"Publish Session",
    L"Apply the current settings to every workstation?",
    L"Publish",
    L"Cancel");
```

## Info Dialog Shortcut

Use `ShowInfoDialog(...)` when you only need one acknowledge button:

```cpp
darkui::ShowInfoDialog(hwnd, 5003, theme, L"Completed", L"Export finished.");
```

## Notes

- These helpers are additive. The original `Create(...)`, `SetTheme(...)`, and `Dialog` APIs continue to work unchanged.
- `ApplyTheme(...)` only handles theme distribution. Layout and `MoveWindow(...)` remain the caller's responsibility.
- `ShowMessageDialog(...)` is intended for simple text dialogs. For custom forms, continue using `darkui::Dialog` directly.
