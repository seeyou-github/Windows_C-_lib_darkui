# CheckBox

## Overview

`darkui::CheckBox` is a dark owner-drawn checkbox for Win32. It preserves standard `BN_CLICKED` handling while replacing the light native glyph with a themed dark box and check mark.

## Files

- `include/darkui/checkbox.h`
- `src/checkbox.cpp`

## Suitable Scenarios

- Feature toggles in dark settings pages
- Confirmation options in dialogs
- Compact preference panels

## Main Features

- Dark box, border, and text rendering
- Checked, unchecked, hover, and disabled states
- Keeps normal `WM_COMMAND + BN_CLICKED`
- Supports host surface color blending

## Basic Usage

```cpp
#include "darkui/checkbox.h"

darkui::Theme theme;
theme.checkBackground = RGB(43, 47, 54);
theme.checkBackgroundHot = RGB(55, 60, 68);
theme.checkAccent = RGB(82, 132, 204);
theme.checkText = RGB(236, 239, 244);

darkui::CheckBox remember;
remember.Create(hwnd, 3301, L"Remember last workspace", theme);
remember.SetSurfaceColor(theme.panel);
remember.SetChecked(true);
MoveWindow(remember.hwnd(), 20, 20, 280, 28, TRUE);
```

## Parent Message Handling

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 3301 && HIWORD(wParam) == BN_CLICKED) {
        bool checked = remember.GetChecked();
        return 0;
    }
    break;
```

## Common API

### `Create`

```cpp
bool Create(HWND parent, int controlId, const std::wstring& text, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP, DWORD exStyle = 0);
```

### `SetChecked` / `GetChecked`

```cpp
remember.SetChecked(true);
bool checked = remember.GetChecked();
```

### `SetSurfaceColor`

```cpp
remember.SetSurfaceColor(theme.panel);
```

## Theme Fields Used

- `checkBackground`
- `checkBackgroundHot`
- `checkAccent`
- `checkBorder`
- `checkText`
- `checkDisabledText`
- `uiFont`
- `background`

## Usage Notes

- Use `SetChecked()` for initial state setup
- Disable the control through `EnableWindow(checkbox.hwnd(), FALSE)`
- Set the surface color when the control sits on a card or panel

## Demo Reference

For a complete example, see:

- `../demo/src/demo_checkbox.cpp`

## Current Limitations

- No tri-state mode yet
- No built-in validation group logic
