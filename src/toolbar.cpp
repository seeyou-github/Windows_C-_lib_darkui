#include "darkui/toolbar.h"

#include <windowsx.h>

#include <algorithm>

namespace darkui {
namespace {

constexpr wchar_t kToolbarClassName[] = L"DarkUiToolbarControl";

ATOM EnsureToolbarClassRegistered(HINSTANCE instance);
LRESULT CALLBACK ToolbarWindowProcThunk(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

}  // namespace

struct Toolbar::Impl {
    Toolbar* owner = nullptr;
    HINSTANCE instance = nullptr;
    HBRUSH brushBackground = nullptr;
    HBRUSH brushItem = nullptr;
    HBRUSH brushItemHot = nullptr;
    HBRUSH brushItemActive = nullptr;
    HPEN penSeparator = nullptr;
    HFONT font = nullptr;
    std::vector<ToolbarItem> items;
    std::vector<RECT> itemRects;
    int hotIndex = -1;
    int pressedIndex = -1;
    bool trackingMouseLeave = false;
    bool layoutDirty = true;

    explicit Impl(Toolbar* toolbar) : owner(toolbar) {}

    ~Impl() {
        if (brushBackground) DeleteObject(brushBackground);
        if (brushItem) DeleteObject(brushItem);
        if (brushItemHot) DeleteObject(brushItemHot);
        if (brushItemActive) DeleteObject(brushItemActive);
        if (penSeparator) DeleteObject(penSeparator);
        if (font) DeleteObject(font);
    }

    bool UpdateThemeResources() {
        HBRUSH newBrushBackground = CreateSolidBrush(owner->theme_.toolbarBackground);
        HBRUSH newBrushItem = CreateSolidBrush(owner->theme_.toolbarItem);
        HBRUSH newBrushItemHot = CreateSolidBrush(owner->theme_.toolbarItemHot);
        HBRUSH newBrushItemActive = CreateSolidBrush(owner->theme_.toolbarItemActive);
        HPEN newPenSeparator = CreatePen(PS_SOLID, 1, owner->theme_.toolbarSeparator);
        HFONT newFont = CreateFont(owner->theme_.uiFont);

        if (!newBrushBackground || !newBrushItem || !newBrushItemHot || !newBrushItemActive || !newPenSeparator || !newFont) {
            if (newBrushBackground) DeleteObject(newBrushBackground);
            if (newBrushItem) DeleteObject(newBrushItem);
            if (newBrushItemHot) DeleteObject(newBrushItemHot);
            if (newBrushItemActive) DeleteObject(newBrushItemActive);
            if (newPenSeparator) DeleteObject(newPenSeparator);
            if (newFont) DeleteObject(newFont);
            return false;
        }

        if (brushBackground) DeleteObject(brushBackground);
        if (brushItem) DeleteObject(brushItem);
        if (brushItemHot) DeleteObject(brushItemHot);
        if (brushItemActive) DeleteObject(brushItemActive);
        if (penSeparator) DeleteObject(penSeparator);
        if (font) DeleteObject(font);

        brushBackground = newBrushBackground;
        brushItem = newBrushItem;
        brushItemHot = newBrushItemHot;
        brushItemActive = newBrushItemActive;
        penSeparator = newPenSeparator;
        font = newFont;
        layoutDirty = true;

        if (owner->toolbarHwnd_) {
            InvalidateRect(owner->toolbarHwnd_, nullptr, TRUE);
        }
        return true;
    }

    RECT ClientRect() const {
        RECT rect{};
        GetClientRect(owner->toolbarHwnd_, &rect);
        return rect;
    }

    int ItemWidth(HDC dc, const ToolbarItem& item) const {
        if (item.separator) {
            return 14;
        }
        const int base = 44;
        SIZE size{};
        GetTextExtentPoint32W(dc, item.text.c_str(), static_cast<int>(item.text.size()), &size);
        const int textWidth = size.cx;
        return std::max(base, textWidth + owner->theme_.textPadding * 2 + 10);
    }

    void RebuildItemRects() {
        itemRects.clear();
        itemRects.resize(items.size());
        if (!owner->toolbarHwnd_) {
            layoutDirty = false;
            return;
        }

        HDC dc = GetDC(owner->toolbarHwnd_);
        if (!dc) {
            layoutDirty = false;
            return;
        }
        HFONT drawFont = font ? font : reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        HFONT oldFont = drawFont ? reinterpret_cast<HFONT>(SelectObject(dc, drawFont)) : nullptr;

        RECT client = ClientRect();
        RECT rc{client.left + 8, client.top + 6, client.left + 8, client.bottom - 6};
        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            const int width = ItemWidth(dc, items[i]);
            rc.right = rc.left + width;
            itemRects[i] = rc;
            rc.left = rc.right + 6;
        }

        if (oldFont) {
            SelectObject(dc, oldFont);
        }
        ReleaseDC(owner->toolbarHwnd_, dc);
        layoutDirty = false;
    }

    void EnsureLayout() {
        if (layoutDirty) {
            RebuildItemRects();
        }
    }

    RECT ItemRect(int index) {
        EnsureLayout();
        if (index < 0 || index >= static_cast<int>(itemRects.size())) {
            return RECT{0, 0, 0, 0};
        }
        return itemRects[index];
    }

    int HitTest(POINT point) {
        EnsureLayout();
        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            if (items[i].separator || items[i].disabled) {
                continue;
            }
            RECT rc = itemRects[i];
            if (PtInRect(&rc, point)) {
                return i;
            }
        }
        return -1;
    }

    void NotifyClick(int index) {
        if (!owner->parentHwnd_ || !owner->toolbarHwnd_ || index < 0 || index >= static_cast<int>(items.size())) {
            return;
        }
        const ToolbarItem& item = items[index];
        SendMessageW(owner->parentHwnd_,
                     WM_COMMAND,
                     MAKEWPARAM(static_cast<UINT>(item.commandId), 0),
                     reinterpret_cast<LPARAM>(owner->toolbarHwnd_));
    }

    void StartMouseLeaveTracking() {
        if (!owner->toolbarHwnd_ || trackingMouseLeave) {
            return;
        }
        TRACKMOUSEEVENT track{};
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = owner->toolbarHwnd_;
        if (TrackMouseEvent(&track)) {
            trackingMouseLeave = true;
        }
    }

    void Paint(HDC dc) {
        EnsureLayout();
        RECT client = ClientRect();
        FillRect(dc, &client, brushBackground ? brushBackground : reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        HFONT drawFont = font ? font : reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        HFONT oldFont = drawFont ? reinterpret_cast<HFONT>(SelectObject(dc, drawFont)) : nullptr;
        SetBkMode(dc, TRANSPARENT);

        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            const ToolbarItem& item = items[i];
            RECT rc = itemRects[i];

            if (item.separator) {
                HPEN drawPen = penSeparator ? penSeparator : reinterpret_cast<HPEN>(GetStockObject(DC_PEN));
                HPEN oldPen = drawPen ? reinterpret_cast<HPEN>(SelectObject(dc, drawPen)) : nullptr;
                const int x = (rc.left + rc.right) / 2;
                MoveToEx(dc, x, rc.top + 4, nullptr);
                LineTo(dc, x, rc.bottom - 4);
                if (oldPen) {
                    SelectObject(dc, oldPen);
                }
                continue;
            }

            HBRUSH fill = brushItem;
            COLORREF textColor = owner->theme_.toolbarText;
            if (item.checked || i == pressedIndex) {
                fill = brushItemActive;
                textColor = owner->theme_.toolbarTextActive;
            } else if (i == hotIndex) {
                fill = brushItemHot;
            }

            if (item.disabled) {
                fill = brushItem;
                textColor = owner->theme_.buttonDisabledText;
            }

            HGDIOBJ oldBrush = SelectObject(dc, fill ? fill : reinterpret_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH)));
            HPEN oldPen = reinterpret_cast<HPEN>(SelectObject(dc, GetStockObject(NULL_PEN)));
            RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, 12, 12);
            SelectObject(dc, oldPen);
            SelectObject(dc, oldBrush);

            RECT textRect = rc;
            textRect.left += owner->theme_.textPadding;
            textRect.right -= owner->theme_.textPadding;
            SetTextColor(dc, textColor);
            DrawTextW(dc, item.text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
        }

        if (oldFont) {
            SelectObject(dc, oldFont);
        }
    }

    static LRESULT CALLBACK ToolbarWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        auto* self = reinterpret_cast<Impl*>(GetWindowLongPtrW(window, GWLP_USERDATA));

        if (message == WM_NCCREATE) {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = reinterpret_cast<Impl*>(create->lpCreateParams);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            return TRUE;
        }

        if (!self) {
            return DefWindowProcW(window, message, wParam, lParam);
        }

        switch (message) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(window, &ps);
            self->Paint(dc);
            EndPaint(window, &ps);
            return 0;
        }
        case WM_SIZE:
            self->layoutDirty = true;
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_MOUSEMOVE: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const int index = self->HitTest(pt);
            if (self->hotIndex != index) {
                self->hotIndex = index;
                InvalidateRect(window, nullptr, FALSE);
            }
            self->StartMouseLeaveTracking();
            return 0;
        }
        case WM_MOUSELEAVE:
            self->trackingMouseLeave = false;
            self->hotIndex = -1;
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_LBUTTONDOWN: {
            SetFocus(window);
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            self->pressedIndex = self->HitTest(pt);
            if (self->pressedIndex >= 0) {
                SetCapture(window);
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const int releasedIndex = self->HitTest(pt);
            const int pressedIndex = self->pressedIndex;
            self->pressedIndex = -1;
            if (GetCapture() == window) {
                ReleaseCapture();
            }
            InvalidateRect(window, nullptr, FALSE);
            if (pressedIndex >= 0 && pressedIndex == releasedIndex) {
                self->NotifyClick(pressedIndex);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            self->pressedIndex = -1;
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE || wParam == VK_RETURN) {
                const int index = self->hotIndex >= 0 ? self->hotIndex : 0;
                if (index >= 0 && index < static_cast<int>(self->items.size()) && !self->items[index].separator && !self->items[index].disabled) {
                    self->NotifyClick(index);
                    return 0;
                }
            }
            break;
        default:
            break;
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }
};

namespace {

ATOM EnsureToolbarClassRegistered(HINSTANCE instance) {
    static ATOM atom = 0;
    if (atom != 0) {
        return atom;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ToolbarWindowProcThunk;
    wc.hInstance = instance;
    wc.lpszClassName = kToolbarClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    atom = RegisterClassExW(&wc);
    return atom;
}

LRESULT CALLBACK ToolbarWindowProcThunk(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return Toolbar::Impl::ToolbarWindowProc(window, message, wParam, lParam);
}

}  // namespace

Toolbar::Toolbar() : impl_(std::make_unique<Impl>(this)) {}

Toolbar::~Toolbar() {
    Destroy();
}

bool Toolbar::Create(HWND parent, int controlId, const Theme& theme, DWORD style, DWORD exStyle) {
    Destroy();
    parentHwnd_ = parent;
    controlId_ = controlId;
    theme_ = theme;
    impl_->instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(parent, GWLP_HINSTANCE));
    if (!impl_->instance) {
        impl_->instance = GetModuleHandleW(nullptr);
    }

    if (!EnsureToolbarClassRegistered(impl_->instance)) {
        Destroy();
        return false;
    }

    toolbarHwnd_ = CreateWindowExW(exStyle,
                                   kToolbarClassName,
                                   L"",
                                   style | WS_CLIPSIBLINGS,
                                   0,
                                   0,
                                   0,
                                   0,
                                   parent,
                                   reinterpret_cast<HMENU>(static_cast<INT_PTR>(controlId)),
                                   impl_->instance,
                                   impl_.get());
    if (!toolbarHwnd_) {
        Destroy();
        return false;
    }

    impl_->UpdateThemeResources();
    if (!impl_->brushBackground || !impl_->brushItem || !impl_->brushItemHot || !impl_->brushItemActive || !impl_->penSeparator || !impl_->font) {
        Destroy();
        return false;
    }
    return true;
}

void Toolbar::Destroy() {
    if (toolbarHwnd_) {
        if (GetCapture() == toolbarHwnd_) {
            ReleaseCapture();
        }
        DestroyWindow(toolbarHwnd_);
        toolbarHwnd_ = nullptr;
    }
    parentHwnd_ = nullptr;
    controlId_ = 0;
    impl_->items.clear();
    impl_->itemRects.clear();
    impl_->hotIndex = -1;
    impl_->pressedIndex = -1;
    impl_->layoutDirty = true;
}

void Toolbar::SetTheme(const Theme& theme) {
    const Theme previous = theme_;
    theme_ = theme;
    if (!impl_->UpdateThemeResources()) {
        theme_ = previous;
        impl_->UpdateThemeResources();
    }
}

void Toolbar::SetItems(const std::vector<ToolbarItem>& items) {
    impl_->items = items;
    impl_->layoutDirty = true;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, TRUE);
    }
}

void Toolbar::AddItem(const ToolbarItem& item) {
    impl_->items.push_back(item);
    impl_->layoutDirty = true;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, TRUE);
    }
}

void Toolbar::ClearItems() {
    impl_->items.clear();
    impl_->itemRects.clear();
    impl_->layoutDirty = true;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, TRUE);
    }
}

void Toolbar::SetItem(int index, const ToolbarItem& item) {
    if (index < 0 || index >= static_cast<int>(impl_->items.size())) {
        return;
    }
    impl_->items[index] = item;
    impl_->layoutDirty = true;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, FALSE);
    }
}

void Toolbar::SetChecked(int index, bool checked) {
    if (index < 0 || index >= static_cast<int>(impl_->items.size())) {
        return;
    }
    impl_->items[index].checked = checked;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, FALSE);
    }
}

void Toolbar::SetDisabled(int index, bool disabled) {
    if (index < 0 || index >= static_cast<int>(impl_->items.size())) {
        return;
    }
    impl_->items[index].disabled = disabled;
    if (toolbarHwnd_) {
        InvalidateRect(toolbarHwnd_, nullptr, FALSE);
    }
}

std::size_t Toolbar::GetCount() const {
    return impl_->items.size();
}

ToolbarItem Toolbar::GetItem(int index) const {
    if (index < 0 || index >= static_cast<int>(impl_->items.size())) {
        return {};
    }
    return impl_->items[index];
}

}  // namespace darkui
