#pragma once

#include "darkui/combobox.h"

#include <memory>

namespace darkui {

class ProgressBar {
public:
    struct Impl;

    ProgressBar();
    ~ProgressBar();

    ProgressBar(const ProgressBar&) = delete;
    ProgressBar& operator=(const ProgressBar&) = delete;

    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return progressHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }

    void SetTheme(const Theme& theme);
    void SetRange(int minimum, int maximum);
    void SetValue(int value);
    void SetShowPercentage(bool enabled);

    int GetValue() const { return value_; }
    int GetMinimum() const { return minimum_; }
    int GetMaximum() const { return maximum_; }
    bool show_percentage() const { return showPercentage_; }

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND progressHwnd_ = nullptr;
    int controlId_ = 0;
    int minimum_ = 0;
    int maximum_ = 100;
    int value_ = 0;
    bool showPercentage_ = true;
    Theme theme_{};
};

}  // namespace darkui
