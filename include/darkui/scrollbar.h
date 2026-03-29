#pragma once

#include "darkui/combobox.h"

#include <memory>

namespace darkui {

class ScrollBar {
public:
    struct Impl;

    ScrollBar();
    ~ScrollBar();

    ScrollBar(const ScrollBar&) = delete;
    ScrollBar& operator=(const ScrollBar&) = delete;

    bool Create(HWND parent, int controlId, bool vertical, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return scrollBarHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }
    bool vertical() const { return vertical_; }

    void SetTheme(const Theme& theme);
    void SetRange(int minimum, int maximum);
    void SetPageSize(int pageSize);
    void SetValue(int value, bool notify = false);

    int GetValue() const { return value_; }
    int GetMinimum() const { return minimum_; }
    int GetMaximum() const { return maximum_; }
    int GetPageSize() const { return pageSize_; }

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND scrollBarHwnd_ = nullptr;
    int controlId_ = 0;
    int minimum_ = 0;
    int maximum_ = 100;
    int pageSize_ = 10;
    int value_ = 0;
    bool vertical_ = true;
    Theme theme_{};
};

}  // namespace darkui
