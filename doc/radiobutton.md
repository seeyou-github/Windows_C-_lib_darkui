# RadioButton

## Overview

`darkui::RadioButton` is a dark owner-drawn radio button for Win32. It preserves native auto-radio grouping while replacing the light system glyph with a themed dark circle and selection dot.

## Files

- `include/darkui/radiobutton.h`
- `src/radiobutton.cpp`

## Suitable Scenarios

- Mutually exclusive display modes
- Theme or profile selection panels
- Compact settings groups in dark tools

## Main Features

- Dark circle, dot, and text rendering
- Hover and disabled states
- Keeps normal `WM_COMMAND + BN_CLICKED`
- Uses native auto-radio grouping semantics

## Basic Usage

```cpp
#include "darkui/radiobutton.h"

darkui::Theme theme;
theme.radioBackground = RGB(43, 47, 54);
theme.radioBackgroundHot = RGB(55, 60, 68);
theme.radioAccent = RGB(82, 132, 204);
theme.radioText = RGB(236, 239, 244);

darkui::RadioButton compact;
darkui::RadioButton detailed;

compact.Create(hwnd, 4401, L"Compact view", theme, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP);
detailed.Create(hwnd, 4402, L"Detailed view", theme);
compact.SetChecked(true);
MoveWindow(compact.hwnd(), 20, 20, 260, 28, TRUE);
MoveWindow(detailed.hwnd(), 20, 56, 260, 28, TRUE);
```

## Parent Message Handling

```cpp
case WM_COMMAND:
    if (HIWORD(wParam) == BN_CLICKED) {
        if (LOWORD(wParam) == 4401 || LOWORD(wParam) == 4402) {
            return 0;
        }
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
compact.SetChecked(true);
bool selected = compact.GetChecked();
```

### `SetSurfaceColor`

```cpp
compact.SetSurfaceColor(theme.panel);
```

## Theme Fields Used

- `radioBackground`
- `radioBackgroundHot`
- `radioAccent`
- `radioBorder`
- `radioText`
- `radioDisabledText`
- `uiFont`
- `background`

## Usage Notes

- Use `WS_GROUP` on the first radio button in a group
- Native auto-radio behavior clears the previously selected sibling
- Set the surface color when the control sits on a card or panel

## Demo Reference

For a complete example, see:

- `../demo/src/demo_radiobutton.cpp`

## Current Limitations

- No custom group container helper
- No per-item subtitle support
