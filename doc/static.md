# Static

## Overview

`darkui::Static` is a dark presentation control for Win32 that can render text, an icon, or a bitmap inside a themed rectangular area.

## Files

- `include/darkui/static.h`
- `src/static.cpp`

## Suitable Scenarios

- Section titles and helper text in dark panels
- Status icons or product glyphs
- Bitmap previews or decorative badges

## Main Features

- Dark background and themed text rendering
- Text, icon, and bitmap display modes
- Configurable text alignment and ellipsis behavior
- Keeps normal `STATIC` window usage and optional `SS_NOTIFY`

## Basic Usage

```cpp
#include "darkui/static.h"

darkui::Theme theme;
theme.staticBackground = RGB(24, 27, 31);
theme.staticText = RGB(236, 239, 244);

darkui::Static title;
title.Create(hwnd, 1201, L"Repository Overview", theme, WS_CHILD | WS_VISIBLE | SS_LEFT);
title.SetTextFormat(DT_LEFT | DT_SINGLELINE);
MoveWindow(title.hwnd(), 20, 20, 260, 36, TRUE);
```

## Icon And Bitmap Modes

```cpp
title.SetIcon(LoadIconW(nullptr, IDI_INFORMATION));
title.SetBitmap(previewBitmap);
```

Notes:

- `SetIcon()` and `SetBitmap()` do not take ownership of the passed handles
- Call `SetText()` or `ClearImage()` to return to text mode

## Common API

### `Create`

```cpp
bool Create(HWND parent, int controlId, const std::wstring& text = L"", const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | SS_LEFT, DWORD exStyle = 0);
```

### `SetText` / `GetText`

```cpp
title.SetText(L"Queued jobs");
std::wstring text = title.GetText();
```

### `SetIcon`

```cpp
title.SetIcon(LoadIconW(nullptr, IDI_INFORMATION));
```

### `SetBitmap`

```cpp
title.SetBitmap(bitmapHandle);
```

### `SetBackgroundColor`

```cpp
title.SetBackgroundColor(theme.panel);
```

### `SetTextFormat`

```cpp
title.SetTextFormat(DT_CENTER | DT_SINGLELINE);
```

## Theme Fields Used

- `staticBackground`
- `staticText`
- `textPadding`
- `uiFont`

## Usage Notes

- Use `DT_SINGLELINE` when you want centered title text
- Omit `DT_SINGLELINE` when you want wrapped paragraph-style text
- For icons and bitmaps, the control centers the image inside the client area

## Demo Reference

For a complete example, see:

- `../demo/src/demo_static.cpp`

## Current Limitations

- No automatic image scaling
- No alpha-composited bitmap pipeline
- No built-in hyperlink behavior
