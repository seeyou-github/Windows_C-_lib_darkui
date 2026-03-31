#pragma once

#include "darkui/combobox.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace darkui {

// Single item stored by darkui::ListBox.
struct ListBoxItem {
    // Text shown to the user.
    std::wstring text;
    // Optional application-owned payload value.
    std::uintptr_t userData = 0;
};

// Dark list box built from a rounded host window plus an inner native LISTBOX.
// Usage:
// - Create the control with Create().
// - Fill it with SetItems() or AddItem().
// - Position the host window with MoveWindow().
// - Listen for LBN_SELCHANGE in the parent window.
// Notes:
// - The inner LISTBOX keeps native keyboard, selection, and scrolling behavior.
// - Single-selection and multi-selection styles are both supported through the
//   standard Win32 list-box style flags passed to Create().
class ListBox {
public:
    struct Impl;
    struct Options {
        std::vector<ListBoxItem> items;
        int selection = -1;
        int cornerRadius = -1;
        DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL;
        DWORD exStyle = 0;
    };

    ListBox();
    ~ListBox();

    ListBox(const ListBox&) = delete;
    ListBox& operator=(const ListBox&) = delete;

    // Creates the dark list box as a child window.
    // Parameters:
    // - parent: Parent window that receives forwarded LBN_* notifications.
    // - controlId: Child control ID used in WM_COMMAND.
    // - theme: Visual theme used for border, background, and text.
    // - style: Standard LISTBOX style flags. Border styles are stripped internally.
    // - exStyle: Optional extended style for the host window.
    // Returns:
    // - true on success.
    // - false on failure.
    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, DWORD exStyle = 0);
    // Creates the list box from an options structure.
    bool Create(HWND parent, int controlId, const Theme& theme, const Options& options) {
        if (!Create(parent, controlId, theme, options.style, options.exStyle)) {
            return false;
        }
        if (options.cornerRadius >= 0) {
            SetCornerRadius(options.cornerRadius);
        }
        if (!options.items.empty()) {
            SetItems(options.items);
        }
        if (options.selection >= 0) {
            SetSelection(options.selection);
        }
        return true;
    }
    // Destroys the host and inner list-box windows.
    void Destroy();

    // Returns the dark host HWND.
    HWND hwnd() const { return hostHwnd_; }
    // Returns the inner native LISTBOX HWND.
    HWND list_hwnd() const { return listHwnd_; }
    // Returns the parent HWND passed to Create().
    HWND parent() const { return parentHwnd_; }
    // Returns the child control ID passed to Create().
    int control_id() const { return controlId_; }
    // Returns the active theme.
    const Theme& theme() const { return theme_; }
    // Returns the current corner radius in pixels.
    int corner_radius() const { return cornerRadius_; }

    // Replaces the current theme and repaints the control.
    void SetTheme(const Theme& theme);
    // Replaces all items at once.
    void SetItems(const std::vector<ListBoxItem>& items);
    // Appends a single item to the end of the list.
    void AddItem(const ListBoxItem& item);
    // Removes all items.
    void ClearItems();
    // Returns the number of items currently stored in the control.
    std::size_t GetCount() const;
    // Returns the current zero-based selection index for single-select mode.
    int GetSelection() const;
    // Returns all selected indices for single-select or multi-select mode.
    std::vector<int> GetSelections() const;
    // Changes the current selection.
    // Parameters:
    // - index: Zero-based item index.
    // - notify: When true, forwards LBN_SELCHANGE to the parent window.
    void SetSelection(int index, bool notify = false);
    // Returns a copy of the item at the specified index.
    ListBoxItem GetItem(int index) const;
    // Sets the outer corner radius of the dark host surface.
    void SetCornerRadius(int radius);

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND hostHwnd_ = nullptr;
    HWND listHwnd_ = nullptr;
    int controlId_ = 0;
    int cornerRadius_ = 10;
    Theme theme_{};
};

}  // namespace darkui
