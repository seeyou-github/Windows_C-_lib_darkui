#pragma once

#include "darkui/combobox.h"

#include <commctrl.h>

#include <memory>
#include <string>
#include <vector>

namespace darkui {

struct TableColumn {
    std::wstring text;
    int width = 120;
    int format = LVCFMT_LEFT;
};

using TableRow = std::vector<std::wstring>;

class Table {
public:
    struct Impl;

    Table();
    ~Table();

    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;

    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return tableHwnd_; }
    HWND header() const { return headerHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }

    void SetTheme(const Theme& theme);
    void SetColumns(const std::vector<TableColumn>& columns);
    void SetRows(const std::vector<TableRow>& rows);
    void AddRow(const TableRow& row);
    void ClearRows();
    std::size_t GetRowCount() const;
    std::size_t GetColumnCount() const;

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND tableHwnd_ = nullptr;
    HWND headerHwnd_ = nullptr;
    int controlId_ = 0;
    Theme theme_{};
};

}  // namespace darkui
