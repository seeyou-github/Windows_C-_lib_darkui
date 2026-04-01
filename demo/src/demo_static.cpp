#include <windows.h>
#include <commctrl.h>

#include "darkui/darkui.h"

namespace {

constexpr wchar_t kDemoClassName[] = L"DarkUiStaticDemoWindow";
constexpr wchar_t kDemoTitle[] = L"lib_darkui static demo";

enum ControlId {
    ID_STATIC_TITLE = 5101,
    ID_STATIC_ICON = 5102,
    ID_STATIC_BITMAP = 5103
};

struct DemoState {
    darkui::Theme theme;
    darkui::ThemeManager themeManager;
    darkui::Static title;
    darkui::Static iconView;
    darkui::Static bitmapView;
    HBRUSH brushBackground = nullptr;
    HFONT titleFont = nullptr;
    HFONT textFont = nullptr;
    HBITMAP previewBitmap = nullptr;
};

void CleanupState(DemoState* state) {
    if (!state) return;
    if (state->brushBackground) DeleteObject(state->brushBackground);
    if (state->titleFont) DeleteObject(state->titleFont);
    if (state->textFont) DeleteObject(state->textFont);
    if (state->previewBitmap) DeleteObject(state->previewBitmap);
    delete state;
}

darkui::Theme MakeTheme() {
    darkui::Theme theme;
    theme.background = RGB(18, 20, 24);
    theme.panel = RGB(30, 33, 38);
    theme.border = RGB(58, 64, 74);
    theme.staticBackground = theme.panel;
    theme.staticText = RGB(236, 239, 244);
    theme.text = RGB(236, 239, 244);
    theme.mutedText = RGB(150, 156, 166);
    theme.uiFont.family = L"Segoe UI";
    theme.uiFont.height = -20;
    theme.textPadding = 12;
    return theme;
}

HBITMAP CreatePreviewBitmap(HWND window) {
    HDC windowDc = GetDC(window);
    if (!windowDc) return nullptr;
    HDC memoryDc = CreateCompatibleDC(windowDc);
    HBITMAP bitmap = CreateCompatibleBitmap(windowDc, 96, 64);
    if (memoryDc && bitmap) {
        HGDIOBJ oldBitmap = SelectObject(memoryDc, bitmap);
        HBRUSH bg = CreateSolidBrush(RGB(40, 46, 54));
        HBRUSH accent = CreateSolidBrush(RGB(78, 120, 184));
        RECT rect{0, 0, 96, 64};
        FillRect(memoryDc, &rect, bg);
        RECT bar1{10, 14, 86, 24};
        RECT bar2{10, 30, 72, 40};
        RECT bar3{10, 46, 56, 56};
        FillRect(memoryDc, &bar1, accent);
        FillRect(memoryDc, &bar2, accent);
        FillRect(memoryDc, &bar3, accent);
        DeleteObject(bg);
        DeleteObject(accent);
        SelectObject(memoryDc, oldBitmap);
    }
    if (memoryDc) DeleteDC(memoryDc);
    ReleaseDC(window, windowDc);
    return bitmap;
}

void Layout(HWND, DemoState* state) {
    MoveWindow(state->title.hwnd(), 32, 108, 300, 44, TRUE);
    MoveWindow(state->iconView.hwnd(), 32, 176, 96, 96, TRUE);
    MoveWindow(state->bitmapView.hwnd(), 152, 176, 140, 96, TRUE);
}

void DrawLine(HDC dc, HFONT font, COLORREF color, RECT rect, const wchar_t* text, UINT format) {
    HFONT oldFont = reinterpret_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &rect, format | DT_NOPREFIX);
    if (oldFont) SelectObject(dc, oldFont);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<DemoState*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        auto* created = new DemoState();
        created->theme = MakeTheme();
        created->brushBackground = CreateSolidBrush(created->theme.background);

        darkui::FontSpec titleSpec = created->theme.uiFont;
        titleSpec.height = -30;
        titleSpec.weight = FW_SEMIBOLD;
        created->titleFont = darkui::CreateFont(titleSpec);
        created->textFont = darkui::CreateFont(created->theme.uiFont);
        created->previewBitmap = CreatePreviewBitmap(window);
        if (!created->brushBackground || !created->titleFont || !created->textFont || !created->previewBitmap) {
            CleanupState(created);
            return -1;
        }

        darkui::Static::Options titleOptions;
        titleOptions.text = L"Dark static text block";
        titleOptions.surfaceRole = darkui::SurfaceRole::Panel;
        titleOptions.textFormat = DT_LEFT | DT_SINGLELINE;
        darkui::Static::Options iconOptions;
        iconOptions.surfaceRole = darkui::SurfaceRole::Panel;
        iconOptions.icon = LoadIconW(nullptr, IDI_INFORMATION);
        darkui::Static::Options bitmapOptions;
        bitmapOptions.surfaceRole = darkui::SurfaceRole::Panel;
        bitmapOptions.bitmap = created->previewBitmap;

        if (!created->title.Create(window, ID_STATIC_TITLE, created->theme, titleOptions) ||
            !created->iconView.Create(window, ID_STATIC_ICON, created->theme, iconOptions) ||
            !created->bitmapView.Create(window, ID_STATIC_BITMAP, created->theme, bitmapOptions)) {
            CleanupState(created);
            return -1;
        }
        created->themeManager.SetTheme(created->theme);
        created->themeManager.Bind(created->title, created->iconView, created->bitmapView);

        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(created));
        Layout(window, created);
        return 0;
    }
    case WM_SIZE:
        if (state) Layout(window, state);
        return 0;
    case WM_ERASEBKGND:
        if (state) {
            RECT rect{};
            GetClientRect(window, &rect);
            FillRect(reinterpret_cast<HDC>(wParam), &rect, state->brushBackground);
            return 1;
        }
        break;
    case WM_PAINT:
        if (state) {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(window, &ps);
            RECT client{};
            GetClientRect(window, &client);
            FillRect(dc, &client, state->brushBackground);

            RECT titleRect{32, 24, client.right - 32, 58};
            RECT descRect{32, 60, client.right - 32, 92};
            RECT noteRect{32, 290, client.right - 32, 350};

            DrawLine(dc, state->titleFont, state->theme.text, titleRect, L"Dark Static Demo", DT_LEFT | DT_TOP | DT_SINGLELINE);
            DrawLine(dc, state->textFont, state->theme.mutedText, descRect, L"One text label, one icon surface, and one bitmap surface using the same dark static wrapper.", DT_LEFT | DT_TOP | DT_WORDBREAK);
            DrawTextW(dc,
                      L"`darkui::Static` is useful for helper text, status badges, icons, and lightweight preview areas.\nText, icon, and bitmap presentation can be switched at runtime.",
                      -1,
                      &noteRect,
                      DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

            EndPaint(window, &ps);
            return 0;
        }
        break;
    case WM_DESTROY:
        CleanupState(state);
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

}  // namespace

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = kDemoClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    if (!RegisterClassExW(&wc)) {
        return 0;
    }

    HWND window = CreateWindowExW(0,
                                  kDemoClassName,
                                  kDemoTitle,
                                  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  760,
                                  420,
                                  nullptr,
                                  nullptr,
                                  instance,
                                  nullptr);
    if (!window) {
        return 0;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
