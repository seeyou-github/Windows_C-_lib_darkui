#pragma once

#include "darkui/combobox.h"

#include <memory>

namespace darkui {

// Single item stored by darkui::Toolbar.
struct ToolbarItem {
    // Text shown inside the toolbar item.
    std::wstring text;
    // Command/control identifier reported to the parent window on click.
    int commandId = 0;
    // Optional application-owned payload value.
    std::uintptr_t userData = 0;
    // Marks the item as a separator instead of a clickable button when true.
    bool separator = false;
    // Marks the item as checked/active when true.
    bool checked = false;
    // Disables pointer and keyboard activation when true.
    bool disabled = false;
};

// Custom dark toolbar for Win32.
// Usage:
// - Create the toolbar with Create().
// - Fill the item list with SetItems() or AddItem().
// - Position it with MoveWindow().
// - Handle WM_COMMAND in the parent window for item clicks.
class Toolbar {
public:
    struct Impl;

    // Constructs an empty toolbar wrapper.
    Toolbar();
    // Destroys the underlying toolbar window if it still exists.
    ~Toolbar();

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;

    // Creates the toolbar as a child window.
    // Parameters:
    // - parent: Parent window that receives WM_COMMAND notifications.
    // - controlId: Child control ID used for identification.
    // - theme: Visual theme used for drawing.
    // - style: Standard child-window style flags.
    // - exStyle: Optional extended window style.
    // Returns:
    // - true on success.
    // - false if the window or drawing resources could not be created.
    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP, DWORD exStyle = 0);
    // Destroys the toolbar window and resets wrapper state.
    void Destroy();

    // Returns the underlying HWND, or nullptr before Create().
    HWND hwnd() const { return toolbarHwnd_; }
    // Returns the parent HWND passed to Create().
    HWND parent() const { return parentHwnd_; }
    // Returns the child control ID passed to Create().
    int control_id() const { return controlId_; }
    // Returns the theme currently stored by the control.
    const Theme& theme() const { return theme_; }

    // Replaces the current theme and repaints the toolbar.
    void SetTheme(const Theme& theme);
    // Replaces all toolbar items at once.
    void SetItems(const std::vector<ToolbarItem>& items);
    // Appends a single toolbar item.
    void AddItem(const ToolbarItem& item);
    // Removes all toolbar items.
    void ClearItems();
    // Replaces one item in place.
    void SetItem(int index, const ToolbarItem& item);
    // Updates the checked state of one item.
    void SetChecked(int index, bool checked);
    // Updates the disabled state of one item.
    void SetDisabled(int index, bool disabled);
    // Returns the number of toolbar items currently stored.
    std::size_t GetCount() const;
    // Returns a copy of the item at the specified index.
    ToolbarItem GetItem(int index) const;

private:
    // Internal implementation object.
    std::unique_ptr<Impl> impl_;
    // Parent window handle.
    HWND parentHwnd_ = nullptr;
    // Underlying toolbar window handle.
    HWND toolbarHwnd_ = nullptr;
    // Child control ID.
    int controlId_ = 0;
    // Theme currently used by the control.
    Theme theme_{};
};

}  // namespace darkui
