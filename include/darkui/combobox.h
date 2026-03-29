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
    COLORREF buttonHover = RGB(72, 80, 92);
    COLORREF buttonHot = RGB(78, 86, 98);
    COLORREF buttonDisabled = RGB(50, 54, 60);
    COLORREF buttonDisabledText = RGB(130, 136, 144);
    COLORREF border = RGB(61, 66, 74);
    COLORREF text = RGB(224, 227, 232);
    COLORREF mutedText = RGB(156, 164, 174);
    COLORREF arrow = RGB(156, 164, 174);
    COLORREF popupItem = RGB(44, 47, 52);
    COLORREF popupItemHot = RGB(65, 72, 82);
    COLORREF popupAccentItem = RGB(27, 47, 83);
    COLORREF popupAccentItemHot = RGB(39, 66, 116);
    COLORREF tableBackground = RGB(34, 36, 40);
    COLORREF tableText = RGB(224, 227, 232);
    COLORREF tableHeaderBackground = RGB(44, 47, 52);
    COLORREF tableHeaderText = RGB(224, 227, 232);
    COLORREF tableGrid = RGB(61, 66, 74);
    COLORREF sliderBackground = RGB(34, 36, 40);
    COLORREF sliderTrack = RGB(52, 56, 62);
    COLORREF sliderFill = RGB(78, 120, 184);
    COLORREF sliderThumb = RGB(224, 227, 232);
    COLORREF sliderThumbHot = RGB(245, 247, 250);
    COLORREF sliderTick = RGB(92, 100, 112);
    FontSpec uiFont{};
    int itemHeight = 24;
    int arrowWidth = 12;
    int arrowRightPadding = 10;
    int textPadding = 8;
    int popupBorder = 1;
    int popupOffsetY = 2;
    int tableRowHeight = 28;
    int tableHeaderHeight = 30;
    int sliderTrackHeight = 6;
    int sliderThumbRadius = 9;
};

struct ComboItem {
    std::wstring text;
    std::uintptr_t userData = 0;
    bool accent = false;
};

HFONT CreateFont(const FontSpec& spec);

class Button {
public:
    Button();
    ~Button();

    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;

    bool Create(HWND parent, int controlId, const std::wstring& text, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return buttonHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }
    int corner_radius() const { return cornerRadius_; }

    void SetTheme(const Theme& theme);
    void SetText(const std::wstring& text);
    std::wstring GetText() const;
    void SetCornerRadius(int radius);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND buttonHwnd_ = nullptr;
    int controlId_ = 0;
    int cornerRadius_ = 8;
    Theme theme_{};
};

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
