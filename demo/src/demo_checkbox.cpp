#include <windows.h>
#include <commctrl.h>

#include "darkui/darkui.h"

namespace {

constexpr wchar_t kDemoClassName[] = L"DarkUiCheckBoxDemoWindow";
constexpr wchar_t kDemoTitle[] = L"lib_darkui checkbox demo";

enum ControlId {
    ID_CHECK_A = 7101,
    ID_CHECK_B = 7102,
    ID_CHECK_C = 7103
};

struct DemoState {
    darkui::Theme theme;
    darkui::ThemeManager themeManager;
    darkui::CheckBox checkA;
    darkui::CheckBox checkB;
    darkui::CheckBox checkC;
    HBRUSH brushBackground = nullptr;
    HFONT titleFont = nullptr;
    HFONT textFont = nullptr;
    std::wstring status = L"Ready";
};

void CleanupState(DemoState* state) {
    if (!state) return;
    if (state->brushBackground) DeleteObject(state->brushBackground);
    if (state->titleFont) DeleteObject(state->titleFont);
    if (state->textFont) DeleteObject(state->textFont);
    delete state;
}

darkui::Theme MakeTheme() {
    darkui::Theme theme;
    theme.background = RGB(18, 20, 24);
    theme.panel = RGB(30, 33, 38);
    theme.text = RGB(236, 239, 244);
    theme.mutedText = RGB(150, 156, 166);
    theme.checkBackground = RGB(43, 47, 54);
    theme.checkBackgroundHot = RGB(55, 60, 68);
    theme.checkAccent = RGB(82, 132, 204);
    theme.checkBorder = RGB(64, 72, 84);
    theme.checkText = RGB(236, 239, 244);
    theme.checkDisabledText = RGB(128, 136, 146);
    theme.uiFont.family = L"Segoe UI";
    theme.uiFont.height = -20;
    return theme;
}

void Layout(HWND, DemoState* state) {
    MoveWindow(state->checkA.hwnd(), 32, 126, 320, 28, TRUE);
    MoveWindow(state->checkB.hwnd(), 32, 166, 320, 28, TRUE);
    MoveWindow(state->checkC.hwnd(), 32, 206, 320, 28, TRUE);
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
        if (!created->brushBackground || !created->titleFont || !created->textFont) {
            CleanupState(created);
            return -1;
        }

        darkui::CheckBox::Options checkAOptions;
        checkAOptions.text = L"Enable automatic thumbnail generation";
        checkAOptions.checked = true;
        checkAOptions.surfaceRole = darkui::SurfaceRole::Background;
        darkui::CheckBox::Options checkBOptions;
        checkBOptions.text = L"Pin inspector on startup";
        checkBOptions.surfaceRole = darkui::SurfaceRole::Background;
        darkui::CheckBox::Options checkCOptions;
        checkCOptions.text = L"Download proxy media in background";
        checkCOptions.surfaceRole = darkui::SurfaceRole::Background;

        if (!created->checkA.Create(window, ID_CHECK_A, created->theme, checkAOptions) ||
            !created->checkB.Create(window, ID_CHECK_B, created->theme, checkBOptions) ||
            !created->checkC.Create(window, ID_CHECK_C, created->theme, checkCOptions)) {
            CleanupState(created);
            return -1;
        }
        created->themeManager.SetTheme(created->theme);
        created->themeManager.Bind(created->checkA, created->checkB, created->checkC);
        EnableWindow(created->checkC.hwnd(), FALSE);

        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(created));
        Layout(window, created);
        return 0;
    }
    case WM_SIZE:
        if (state) Layout(window, state);
        return 0;
    case WM_COMMAND:
        if (state && HIWORD(wParam) == BN_CLICKED) {
            state->status = L"Checked states: A=" +
                            std::wstring(state->checkA.GetChecked() ? L"1" : L"0") +
                            L" B=" +
                            std::wstring(state->checkB.GetChecked() ? L"1" : L"0") +
                            L" C=" +
                            std::wstring(state->checkC.GetChecked() ? L"1" : L"0");
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }
        break;
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
            RECT descRect{32, 60, client.right - 32, 94};
            RECT noteRect{32, 252, client.right - 32, 330};
            RECT statusRect{32, client.bottom - 56, client.right - 32, client.bottom - 24};

            DrawLine(dc, state->titleFont, state->theme.text, titleRect, L"Dark CheckBox Demo", DT_LEFT | DT_TOP | DT_SINGLELINE);
            DrawLine(dc, state->textFont, state->theme.mutedText, descRect, L"Hover, checked, unchecked, and disabled states use dark owner-drawn rendering while preserving BN_CLICKED notifications.", DT_LEFT | DT_TOP | DT_WORDBREAK);
            DrawTextW(dc,
                      L"The first checkbox starts checked. The third is disabled to show muted text behavior.\nEach click updates the status line below through normal WM_COMMAND handling.",
                      -1,
                      &noteRect,
                      DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
            DrawLine(dc, state->textFont, state->theme.text, statusRect, state->status.c_str(), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

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
