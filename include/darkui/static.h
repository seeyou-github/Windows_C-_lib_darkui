#pragma once

#include "darkui/combobox.h"

#include <memory>
#include <string>

namespace darkui {

// Dark static control that can render text, an icon, or a bitmap.
// Usage:
// - Create the control with Create().
// - Position it with MoveWindow().
// - Use SetText(), SetIcon(), or SetBitmap() to switch presentation mode.
// Notes:
// - The control is primarily presentation-oriented and usually non-interactive.
// - If you need click notifications, pass SS_NOTIFY in the style flags.
class Static {
public:
    enum class ContentMode {
        Text,
        Icon,
        Bitmap
    };
    struct Options {
        std::wstring text{};
        HICON icon = nullptr;
        HBITMAP bitmap = nullptr;
        COLORREF backgroundColor = CLR_INVALID;
        SurfaceRole surfaceRole = SurfaceRole::Auto;
        UINT textFormat = DT_LEFT;
        bool ellipsis = true;
        DWORD style = WS_CHILD | WS_VISIBLE | SS_LEFT;
        DWORD exStyle = 0;
    };

    Static();
    ~Static();

    Static(const Static&) = delete;
    Static& operator=(const Static&) = delete;

    // Creates the static control as a child window.
    // Parameters:
    // - parent: Parent window that owns the control.
    // - controlId: Child control ID used by Win32 notifications when enabled.
    // - text: Initial text shown when the content mode is Text.
    // - theme: Visual theme used for background and text.
    // - style: Standard STATIC style flags.
    // - exStyle: Optional extended style.
    // Returns:
    // - true on success.
    // - false on failure.
    bool Create(HWND parent, int controlId, const std::wstring& text = L"", const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | SS_LEFT, DWORD exStyle = 0);
    // Creates the static control from an options structure.
    bool Create(HWND parent, int controlId, const Theme& theme, const Options& options) {
        if (!Create(parent, controlId, options.text, theme, options.style, options.exStyle)) {
            return false;
        }
        SetTextFormat(options.textFormat);
        SetEllipsis(options.ellipsis);
        SetBackgroundColor(options.backgroundColor != CLR_INVALID ? options.backgroundColor : ResolveSurfaceColor(theme, options.surfaceRole));
        if (options.bitmap) {
            SetBitmap(options.bitmap);
        } else if (options.icon) {
            SetIcon(options.icon);
        }
        return true;
    }
    // Destroys the underlying window and resets wrapper state.
    void Destroy();

    // Returns the underlying HWND.
    HWND hwnd() const { return staticHwnd_; }
    // Returns the parent HWND passed to Create().
    HWND parent() const { return parentHwnd_; }
    // Returns the child control ID passed to Create().
    int control_id() const { return controlId_; }
    // Returns the active theme.
    const Theme& theme() const { return theme_; }
    // Returns the current content mode.
    ContentMode content_mode() const { return contentMode_; }
    // Returns the background color currently used by the control.
    COLORREF background_color() const { return backgroundColor_; }

    // Replaces the current theme and repaints the control.
    void SetTheme(const Theme& theme);
    // Updates the visible text and switches the mode to Text.
    void SetText(const std::wstring& text);
    // Returns the current text value.
    std::wstring GetText() const;
    // Switches the control to Icon mode.
    // Notes:
    // - The icon handle is not owned or destroyed by darkui.
    void SetIcon(HICON icon);
    // Switches the control to Bitmap mode.
    // Notes:
    // - The bitmap handle is not owned or destroyed by darkui.
    void SetBitmap(HBITMAP bitmap);
    // Clears any assigned icon or bitmap and returns to Text mode.
    void ClearImage();
    // Sets the background color used behind text, icons, or bitmaps.
    void SetBackgroundColor(COLORREF color);
    // Sets DrawText alignment flags such as DT_LEFT / DT_CENTER / DT_RIGHT.
    // Notes:
    // - Vertical centering is handled internally.
    void SetTextFormat(UINT format);
    // Enables or disables end ellipsis for long text.
    void SetEllipsis(bool enabled);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND staticHwnd_ = nullptr;
    int controlId_ = 0;
    Theme theme_{};
    ContentMode contentMode_ = ContentMode::Text;
    COLORREF backgroundColor_ = RGB(34, 36, 40);
    UINT textFormat_ = DT_LEFT;
    bool ellipsis_ = true;
};

}  // namespace darkui
