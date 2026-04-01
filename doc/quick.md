# Quick Helpers

## Overview

`include/darkui/quick.h` adds a thin convenience layer on top of the existing control wrappers. It does not replace the normal Win32-style API. Instead, it reduces repeated setup code for the most common tasks:

- creating a ready-made semantic theme
- picking a built-in dark theme preset
- binding one theme to multiple controls
- opening a dark message dialog with one function call

This helper layer is the recommended companion for the new per-control `Options` structs:

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

## Files

- `include/darkui/quick.h`

## Theme Presets

Use `ThemePreset` when you want a ready-to-use palette without filling a long `Theme` manually.

```cpp
#include "darkui/darkui.h"

darkui::Theme theme = darkui::MakePresetTheme(darkui::ThemePreset::Graphite);
```

## Options-Based Control Creation

```cpp
darkui::Button button;
darkui::Button::Options options;
options.text = L"Apply";
options.cornerRadius = 14;
options.surfaceRole = darkui::SurfaceRole::Panel;

button.Create(hwnd, 4101, theme, options);
```

If you need an explicit color, `surfaceColor` still overrides `surfaceRole`.

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

## ThemeManager

Use `ThemeManager` as the preferred repeated theme-switch entry:

```cpp
darkui::ThemeManager themeManager(darkui::MakePresetTheme());
themeManager.Bind(buttonA, buttonB, editA, listBoxA);
themeManager.Apply();

themeManager.SetTheme(darkui::MakePresetTheme(darkui::ThemePreset::Moss));
themeManager.Apply();
```

Use `ThemeBinder` when you only want the remembered binding list without storing the current theme separately.

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

- These helpers are additive. `Options + ThemeManager` is the intended caller-facing path.
- Theme distribution and layout are still separate responsibilities. `ThemeManager` only updates bound controls.
- `ShowMessageDialog(...)` is intended for simple text dialogs. For custom forms, continue using `darkui::Dialog` directly.
