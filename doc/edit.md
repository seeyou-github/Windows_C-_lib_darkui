# Edit

## Overview

`darkui::Edit` is a dark input control built from a dark host window plus an inner native `EDIT`. This keeps native text input, caret, selection, and IME behavior while removing the thin default light border.

## Files

- `include/darkui/edit.h`
- `src/edit.cpp`

## Suitable Scenarios

- Dark search boxes
- Dark form inputs
- Input fields that must keep native IME and selection behavior

## Main Features

- Dark background and text rendering
- Custom placeholder color
- Rounded outer shape
- Read-only mode
- Supports multiline input with `ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL`
- Standard forwarded `EN_*` notifications

## Recommended Usage

```cpp
#include "darkui/edit.h"

darkui::Theme theme;
theme.editBackground = RGB(43, 47, 54);
theme.editText = RGB(236, 239, 244);
theme.editPlaceholder = RGB(128, 137, 150);

darkui::Edit edit;
darkui::Edit::Options options;
options.cueBanner = L"Search or enter text";
options.cornerRadius = 16;

edit.Create(hwnd, 9001, theme, options);

darkui::ThemeManager themeManager(theme);
themeManager.Bind(edit);
themeManager.Apply();

MoveWindow(edit.hwnd(), 20, 20, 260, 42, TRUE);
```

## Parent Message Handling

```cpp
case WM_COMMAND:
    if (LOWORD(wParam) == 9001 && HIWORD(wParam) == EN_CHANGE) {
        std::wstring text = edit.GetText();
        return 0;
    }
    break;
```

## Common API

### `Create`

```cpp
bool Create(HWND parent, int controlId, const Theme& theme, const Edit::Options& options);
```

### `Options`

```cpp
darkui::Edit::Options options;
options.cueBanner = L"Placeholder";
options.cornerRadius = 16;
```

### `SetText` / `GetText`

```cpp
edit.SetText(L"Updated text");
std::wstring text = edit.GetText();
```

### `SetCueBanner`

```cpp
edit.SetCueBanner(L"Placeholder");
```

### `SetCornerRadius`

```cpp
edit.SetCornerRadius(16);
```

### `SetReadOnly`

```cpp
edit.SetReadOnly(true);
```

## Multiline Example

```cpp
darkui::Edit notes;
darkui::Edit::Options notesOptions;
notesOptions.text = L"Line 1\r\nLine 2\r\nLine 3";
notesOptions.cueBanner = L"Write notes here";
notesOptions.cornerRadius = 16;
notesOptions.style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL;
notes.Create(hwnd, 9002, theme, notesOptions);
MoveWindow(notes.hwnd(), 20, 72, 320, 140, TRUE);
```

### `hwnd` / `edit_hwnd`

```cpp
HWND host = edit.hwnd();
HWND nativeEdit = edit.edit_hwnd();
```

## Theme Fields Used

- `editBackground`
- `editText`
- `editPlaceholder`
- `uiFont`

## Usage Notes

- Move and size the host returned by `edit.hwnd()`
- Use `edit.edit_hwnd()` when you need direct access to the inner native `EDIT`
- Read-only mode keeps the same dark appearance
- Multiline mode is enabled by filling `Edit::Options::style` with normal Win32 edit flags

## Demo Reference

For a complete example, see:

- `../demo/src/demo_edit.cpp`
- `../demo/src/demo_toolbar_menubar.cpp`

## Current Limitations

- Currently oriented toward single-line input
- No rich-text support
- No built-in validation rules
- No form-state management
