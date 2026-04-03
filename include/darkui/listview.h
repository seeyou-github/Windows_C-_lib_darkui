#pragma once

#include "darkui/combobox.h"

#include <commctrl.h>

#include <memory>
#include <string>
#include <vector>

namespace darkui {

// Describes a single native ListView column.
struct ListViewColumn {
    // Column header text.
    std::wstring text;
    // Fixed column width in pixels.
    int width = 120;
    // Header and cell text alignment, for example LVCFMT_LEFT or LVCFMT_CENTER.
    int format = LVCFMT_LEFT;
};

// Represents one row. Each string corresponds to one column cell.
using ListViewRow = std::vector<std::wstring>;

// Native ListView wrapper with a dark header and dark color setup.
// Usage:
// - Fill ListView::Options and call Create(parent, id, theme, options).
// - Position the wrapper with MoveWindow().
// - Bind it through ThemedWindowHost::theme_manager() when the page supports runtime theme changes.
// Notes:
// - The inner control is SysListView32.
// - Native scrollbars, column resize, keyboard navigation, and selection behavior are preserved.
class ListView {
public:
    struct Impl;
    struct Options {
        std::vector<ListViewColumn> columns;
        std::vector<ListViewRow> rows;
        int selection = -1;
        DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS;
        DWORD exStyle = LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP;
    };

    ListView();
    ~ListView();

    ListView(const ListView&) = delete;
    ListView& operator=(const ListView&) = delete;

    bool Create(HWND parent, int controlId, const Theme& theme, const Options& options);
    void Destroy();

    // Returns the outer host HWND.
    HWND hwnd() const { return hostHwnd_; }
    // Returns the inner native SysListView32 HWND.
    HWND list_hwnd() const { return listHwnd_; }
    // Returns the native header HWND.
    HWND header_hwnd() const { return headerHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }

    // Low-level theme hook used by ThemeManager.
    void SetTheme(const Theme& theme);
    void SetColumns(const std::vector<ListViewColumn>& columns);
    bool SetColumnWidth(std::size_t index, int width);
    int GetColumnWidth(std::size_t index) const;
    void SetRows(const std::vector<ListViewRow>& rows);
    void AddRow(const ListViewRow& row);
    void ClearRows();
    int GetSelection() const;
    void SetSelection(int index, bool notify = false);
    std::size_t GetRowCount() const;
    std::size_t GetColumnCount() const;

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND hostHwnd_ = nullptr;
    HWND listHwnd_ = nullptr;
    HWND headerHwnd_ = nullptr;
    int controlId_ = 0;
    Theme theme_{};
};

}  // namespace darkui
