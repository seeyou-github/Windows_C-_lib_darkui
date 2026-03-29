#pragma once

#include "darkui/combobox.h"

#include <memory>

namespace darkui {

class Slider {
public:
    struct Impl;

    Slider();
    ~Slider();

    Slider(const Slider&) = delete;
    Slider& operator=(const Slider&) = delete;

    bool Create(HWND parent, int controlId, const Theme& theme = Theme{}, DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP, DWORD exStyle = 0);
    void Destroy();

    HWND hwnd() const { return sliderHwnd_; }
    HWND parent() const { return parentHwnd_; }
    int control_id() const { return controlId_; }
    const Theme& theme() const { return theme_; }

    void SetTheme(const Theme& theme);
    void SetRange(int minimum, int maximum);
    void SetValue(int value, bool notify = false);
    void SetShowTicks(bool enabled);
    void SetTickCount(int count);
    int GetValue() const { return value_; }
    int GetMinimum() const { return minimum_; }
    int GetMaximum() const { return maximum_; }
    bool show_ticks() const { return showTicks_; }
    int tick_count() const { return tickCount_; }

private:
    std::unique_ptr<Impl> impl_;
    HWND parentHwnd_ = nullptr;
    HWND sliderHwnd_ = nullptr;
    int controlId_ = 0;
    int minimum_ = 0;
    int maximum_ = 100;
    int value_ = 0;
    bool showTicks_ = false;
    int tickCount_ = 0;
    Theme theme_{};
};

}  // namespace darkui
