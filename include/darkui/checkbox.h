#pragma once

#include "darkui/combobox.h"

#include <memory>
#include <string>

namespace darkui {

// Owner-drawn dark checkbox that preserves Win32 BN_CLICKED behavior.
// Usage:
// - Create the checkbox with Create().
// - Position it with MoveWindow().
// - Handle state changes through WM_COMMAND / BN_CLICKED in the parent window.
class CheckBox {
public:
    struct Options {
        std::wstring text{};
        bool checked = false;
        COLORREF surfaceColor = CLR_INVALID;
        DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
        DWORD exStyle = 0;
    };

    CheckBox();
    ~CheckBox();

    CheckBox(const CheckBox&) = delete;
    CheckBox& operator=(const CheckBox&) = delete;

    // Creates the checkbox as a child window.
    // Parameters:
    // - parent: Parent window that receives BN_CLICKED notifications.
    // - controlId: Child control ID used in WM_COMMAND.
    // - text: Visible label text.
    // - theme: Visual theme used for drawing.
    // - style: Standard child-button style flags. BS_OWNERDRAW and BS_AUTOCHECKBOX are added internally.
    // - exStyle: Optional extended window style.
    bool Create(HWND parent, int controlId, const std::wstring& text, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP, DWORD exStyle = 0);
    // Creates the checkbox from an options structure.
    bool Create(HWND parent, int controlId, const Theme& theme, const Options& options) {
        if (!Create(parent, controlId, options.text, theme, options.style, options.exStyle)) {
            return false;
        }
        if (options.surfaceColor != CLR_INVALID) {
            SetSurfaceColor(options.surfaceColor);
        }
        if (options.checked) {
            SetChecked(true);
        }
        return true;
    }
    // Destroys the underlying checkbox window.
    void Destroy();

    HWND hwnd() const { return checkboxHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }
    COLORREF surface_color() const { return surfaceColor_; }

    void SetTheme(const Theme& theme);
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    bool GetChecked() const;
    void SetChecked(bool checked);
    void SetSurfaceColor(COLORREF color);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND checkboxHwnd_ = nullptr;
    int controlId_ = 0;
    COLORREF surfaceColor_ = RGB(34, 36, 40);
    Theme theme_{};
};

}  // namespace darkui
