#include <windows.h>
#include <commctrl.h>

#include <memory>
#include <string>

#include "darkui/darkui.h"

namespace {

constexpr wchar_t kDemoClassName[] = L"DarkUiDialogDemoWindow";
constexpr wchar_t kDemoTitle[] = L"lib_darkui dialog demo";

enum ControlId {
    ID_OPEN_MESSAGE = 9201,
    ID_OPEN_FORM = 9202,
    ID_DIALOG_FILL = 9301,
    ID_DIALOG_TITLE = 9302,
    ID_DIALOG_NOTES = 9303,
    ID_DIALOG_LABEL_TITLE = 9304,
    ID_DIALOG_LABEL_NOTES = 9305
};

struct CustomDialogSession {
    darkui::Dialog dialog;
    darkui::Static titleLabel;
    darkui::Static notesLabel;
    darkui::Edit titleEdit;
    darkui::Button fillButton;
    darkui::Edit notesEdit;
};

struct DemoState {
    darkui::Theme theme;
    darkui::Button messageButton;
    darkui::Button formButton;
    HBRUSH brushBackground = nullptr;
    HFONT titleFont = nullptr;
    HFONT textFont = nullptr;
    std::wstring status = L"Ready";
    std::unique_ptr<CustomDialogSession> activeDialog;
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
    theme.panel = RGB(26, 29, 34);
    theme.border = RGB(58, 64, 74);
    theme.text = RGB(236, 239, 244);
    theme.mutedText = RGB(150, 156, 166);
    theme.staticBackground = theme.panel;
    theme.staticText = theme.text;
    theme.editBackground = RGB(38, 42, 48);
    theme.editText = RGB(236, 239, 244);
    theme.editPlaceholder = RGB(126, 134, 144);
    theme.button = RGB(58, 86, 138);
    theme.buttonHover = RGB(68, 99, 158);
    theme.buttonHot = RGB(82, 114, 180);
    theme.buttonDisabled = RGB(45, 49, 56);
    theme.buttonDisabledText = RGB(120, 126, 136);
    theme.uiFont.family = L"Segoe UI";
    theme.uiFont.height = -20;
    theme.textPadding = 10;
    theme.scrollBarThickness = 14;
    return theme;
}

void Layout(HWND window, DemoState* state) {
    RECT client{};
    GetClientRect(window, &client);
    MoveWindow(state->messageButton.hwnd(), 32, 118, 210, 42, TRUE);
    MoveWindow(state->formButton.hwnd(), 258, 118, 210, 42, TRUE);
}

void DrawLine(HDC dc, HFONT font, COLORREF color, RECT rect, const wchar_t* text, UINT format) {
    HFONT oldFont = reinterpret_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &rect, format | DT_NOPREFIX);
    if (oldFont) SelectObject(dc, oldFont);
}

void ShowMessageDialog(HWND window, DemoState* state) {
    darkui::Dialog dialog;
    if (!dialog.Create(window, 4001, L"Delete Snapshot", state->theme, 460, 230)) {
        state->status = L"Dialog creation failed";
        InvalidateRect(window, nullptr, FALSE);
        return;
    }

    dialog.SetTitle(L"Confirm Delete");
    dialog.SetMessage(L"Delete the selected snapshot permanently?\nThis action cannot be undone.");
    dialog.SetConfirmText(L"Delete");
    dialog.SetCancelText(L"Cancel");

    const darkui::Dialog::Result result = dialog.ShowModal();
    state->status = (result == darkui::Dialog::Result::Confirm) ? L"Message dialog confirmed" : L"Message dialog cancelled";
    InvalidateRect(window, nullptr, FALSE);
}

void ShowFormDialog(HWND window, DemoState* state) {
    state->activeDialog = std::make_unique<CustomDialogSession>();
    auto& session = *state->activeDialog;
    if (!session.dialog.Create(window, 4002, L"Create Dark Note", state->theme, 580, 400)) {
        state->status = L"Form dialog creation failed";
        state->activeDialog.reset();
        InvalidateRect(window, nullptr, FALSE);
        return;
    }

    session.dialog.SetTitle(L"Create Note Form");
    session.dialog.SetMessageVisible(false);
    session.dialog.SetConfirmText(L"Save");
    session.dialog.SetCancelText(L"Cancel");

    if (!session.titleLabel.Create(session.dialog.content_hwnd(), ID_DIALOG_LABEL_TITLE, L"Title", state->theme, WS_CHILD | WS_VISIBLE | SS_LEFT) ||
        !session.notesLabel.Create(session.dialog.content_hwnd(), ID_DIALOG_LABEL_NOTES, L"Notes", state->theme, WS_CHILD | WS_VISIBLE | SS_LEFT) ||
        !session.titleEdit.Create(session.dialog.content_hwnd(), ID_DIALOG_TITLE, L"", state->theme) ||
        !session.fillButton.Create(session.dialog.content_hwnd(), ID_DIALOG_FILL, L"Fill Sample", state->theme) ||
        !session.notesEdit.Create(session.dialog.content_hwnd(),
                                  ID_DIALOG_NOTES,
                                  L"",
                                  state->theme,
                                  WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL)) {
        state->status = L"Form controls creation failed";
        state->activeDialog.reset();
        InvalidateRect(window, nullptr, FALSE);
        return;
    }

    session.titleLabel.SetBackgroundColor(state->theme.panel);
    session.notesLabel.SetBackgroundColor(state->theme.panel);
    session.fillButton.SetCornerRadius(12);
    session.fillButton.SetSurfaceColor(state->theme.panel);
    session.titleEdit.SetCornerRadius(12);
    session.notesEdit.SetCornerRadius(12);
    session.titleEdit.SetCueBanner(L"Enter a title");
    session.notesEdit.SetCueBanner(L"Write your notes here");

    MoveWindow(session.titleLabel.hwnd(), 16, 14, 120, 22, TRUE);
    MoveWindow(session.titleEdit.hwnd(), 16, 40, 390, 38, TRUE);
    MoveWindow(session.fillButton.hwnd(), 422, 40, 120, 38, TRUE);
    MoveWindow(session.notesLabel.hwnd(), 16, 94, 120, 22, TRUE);
    MoveWindow(session.notesEdit.hwnd(), 16, 120, 526, 148, TRUE);

    const darkui::Dialog::Result result = session.dialog.ShowModal();
    if (result == darkui::Dialog::Result::Confirm) {
        state->status = L"Saved note: " + session.titleEdit.GetText() + L" | " + session.notesEdit.GetText();
    } else {
        state->status = L"Form dialog cancelled";
    }
    state->activeDialog.reset();
    InvalidateRect(window, nullptr, FALSE);
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

        if (!created->messageButton.Create(window, ID_OPEN_MESSAGE, L"Open Message Dialog", created->theme) ||
            !created->formButton.Create(window, ID_OPEN_FORM, L"Open Form Dialog", created->theme)) {
            CleanupState(created);
            return -1;
        }
        created->messageButton.SetCornerRadius(14);
        created->formButton.SetCornerRadius(14);
        created->messageButton.SetSurfaceColor(created->theme.background);
        created->formButton.SetSurfaceColor(created->theme.background);

        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(created));
        Layout(window, created);
        return 0;
    }
    case WM_SIZE:
        if (state) Layout(window, state);
        return 0;
    case WM_COMMAND:
        if (!state) break;
        if (HIWORD(wParam) == BN_CLICKED) {
            if (LOWORD(wParam) == ID_OPEN_MESSAGE) {
                ShowMessageDialog(window, state);
                return 0;
            }
            if (LOWORD(wParam) == ID_OPEN_FORM) {
                ShowFormDialog(window, state);
                return 0;
            }
            if (LOWORD(wParam) == ID_DIALOG_FILL && state->activeDialog) {
                state->activeDialog->titleEdit.SetText(L"Evening Review");
                state->activeDialog->notesEdit.SetText(L"1. Validate dark title bar\n2. Check confirm/cancel buttons\n3. Test multiline input in the custom body");
                return 0;
            }
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
            RECT descRect{32, 60, client.right - 32, 96};
            RECT noteRect{32, 184, client.right - 32, 274};
            RECT statusRect{32, client.bottom - 56, client.right - 32, client.bottom - 24};

            DrawLine(dc, state->titleFont, state->theme.text, titleRect, L"Dark Dialog Demo", DT_LEFT | DT_TOP | DT_SINGLELINE);
            DrawLine(dc, state->textFont, state->theme.mutedText, descRect, L"One button opens a simple confirmation popup. The other opens a custom dialog body containing Static, Edit, Button, and multiline Edit controls.", DT_LEFT | DT_TOP | DT_WORDBREAK);
            DrawTextW(dc,
                      L"`darkui::Dialog` provides a dark modal shell with a black title bar, bottom confirm/cancel buttons, and a content host for custom child controls.",
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
