#pragma once

#include <windows.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace darkui {

struct FontSpec {
    std::wstring family = L"Segoe UI";
    int height = -20;
    int weight = FW_NORMAL;
    bool italic = false;
    bool monospace = false;
};

struct Theme {
    COLORREF background = RGB(34, 36, 40);
    COLORREF panel = RGB(44, 47, 52);
    COLORREF button = RGB(65, 72, 82);
    COLORREF buttonHot = RGB(78, 86, 98);
    COLORREF border = RGB(61, 66, 74);
    COLORREF text = RGB(224, 227, 232);
    COLORREF mutedText = RGB(156, 164, 174);
    COLORREF arrow = RGB(156, 164, 174);
    COLORREF popupItem = RGB(44, 47, 52);
    COLORREF popupItemHot = RGB(65, 72, 82);
    COLORREF popupAccentItem = RGB(27, 47, 83);
    COLORREF popupAccentItemHot = RGB(39, 66, 116);
    FontSpec uiFont{};
    int itemHeight = 24;
    int arrowWidth = 12;
    int arrowRightPadding = 10;
    int textPadding = 8;
    int popupBorder = 1;
    int popupOffsetY = 2;
};

struct ComboItem {
    std::wstring text;
    std::uintptr_t userData = 0;
    bool accent = false;
};

HFONT CreateFont(const FontSpec& spec);

class ComboBox {
public:
    ComboBox();
    ~ComboBox();

    ComboBox(const ComboBox&) = delete;
    ComboBox& operator=(const ComboBox&) = delete;

    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return comboHwnd_; }
    HWND popup_list() const { return popupList_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }

    void SetTheme(const Theme& theme);
    void SetItems(const std::vector<ComboItem>& items);
    void AddItem(const ComboItem& item);
    void ClearItems();
    int GetSelection() const;
    void SetSelection(int index, bool notify = false);
    std::size_t GetCount() const;
    std::wstring GetText() const;
    ComboItem GetItem(int index) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND comboHwnd_ = nullptr;
    HWND popupHost_ = nullptr;
    HWND popupList_ = nullptr;
    int controlId_ = 0;
    Theme theme_{};
};

}  // namespace darkui
