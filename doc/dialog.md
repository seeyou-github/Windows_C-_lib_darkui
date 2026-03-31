# Dialog

## Overview

`darkui::Dialog` is a modal dark popup window for Win32 with a custom black title bar, dark background, and built-in confirm/cancel buttons. The center area can either use a built-in message `Static` or host your own child controls such as `Static`, `Edit`, `Button`, and multiline `Edit`.

## Files

- `include/darkui/dialog.h`
- `src/dialog.cpp`

## Suitable Scenarios

- Confirmation dialogs
- Input prompts
- Small custom forms and notes popups
- Dark replacement for basic system message boxes

## Main Features

- Custom dark title bar and background
- Built-in confirm and cancel buttons
- Built-in centered message label
- Exposes `content_hwnd()` for custom child controls
- Modal message loop with owner-window disabling

## Basic Usage

```cpp
#include "darkui/dialog.h"

darkui::Dialog dialog;
dialog.Create(hwnd, 5001, L"Delete File", theme, 460, 240);
dialog.SetMessage(L"Delete the selected item permanently?");
dialog.SetConfirmText(L"Delete");
dialog.SetCancelText(L"Cancel");

darkui::Dialog::Result result = dialog.ShowModal();
if (result == darkui::Dialog::Result::Confirm) {
    // confirmed
}
```

## Custom Content Example

```cpp
darkui::Dialog dialog;
dialog.Create(hwnd, 5002, L"Create Note", theme, 560, 360);
dialog.SetMessageVisible(false);

darkui::Static label;
darkui::Edit titleEdit;
darkui::Button fillButton;
darkui::Edit notesEdit;

label.Create(dialog.content_hwnd(), 5101, L"Title", theme);
titleEdit.Create(dialog.content_hwnd(), 5102, L"", theme);
fillButton.Create(dialog.content_hwnd(), 5103, L"Fill Sample", theme);
notesEdit.Create(dialog.content_hwnd(),
                 5104,
                 L"",
                 theme,
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL);

MoveWindow(label.hwnd(), 16, 16, 120, 24, TRUE);
MoveWindow(titleEdit.hwnd(), 16, 44, 360, 38, TRUE);
MoveWindow(fillButton.hwnd(), 392, 44, 120, 38, TRUE);
MoveWindow(notesEdit.hwnd(), 16, 96, 496, 150, TRUE);

dialog.ShowModal();
```

## Common API

### `Create`

```cpp
bool Create(HWND owner, int controlId, const std::wstring& title, const Theme& theme = Theme{}, int width = 480, int height = 280);
```

### `SetMessage`

```cpp
dialog.SetMessage(L"Operation completed.");
```

### `SetMessageVisible`

```cpp
dialog.SetMessageVisible(false);
```

### `content_hwnd`

```cpp
HWND host = dialog.content_hwnd();
```

### `ShowModal`

```cpp
darkui::Dialog::Result result = dialog.ShowModal();
```

### `EndDialog`

```cpp
dialog.EndDialog(darkui::Dialog::Result::Cancel);
```

## Theme Fields Used

- `background`
- `panel`
- `border`
- `text`
- `mutedText`
- `button`
- `buttonHover`
- `buttonHot`
- `buttonDisabled`
- `buttonDisabledText`
- `staticBackground`
- `staticText`
- `uiFont`

## Usage Notes

- `ShowModal()` disables the owner window until the popup closes
- Use `SetMessageVisible(false)` when you want a fully custom body
- Unknown `WM_COMMAND` and `WM_NOTIFY` traffic from custom child controls is forwarded to the owner window

## Demo Reference

For a complete example, see:

- `../demo/src/demo_dialog.cpp`

## Current Limitations

- No resize grip
- No maximize or minimize buttons
- No native task-dialog style command links
