#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>

#include <algorithm>
#include <string>
#include <vector>

#include "darkui/darkui.h"

namespace {

constexpr wchar_t kDemoClassName[] = L"DarkUiShowcaseWindow";
constexpr wchar_t kPageClassName[] = L"DarkUiShowcasePage";
constexpr wchar_t kDemoTitle[] = L"lib_darkui showcase";

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif
#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif

enum ControlId {
    ID_THEME_GRAPHITE = 9001,
    ID_THEME_EMBER,
    ID_THEME_GLACIER,
    ID_THEME_MOSS,
    ID_THEME_MONO,
    ID_MAIN_TAB,

    ID_OVERVIEW_PROFILE = 9101,
    ID_OVERVIEW_PRIMARY,
    ID_OVERVIEW_SECONDARY,
    ID_OVERVIEW_STATUS,
    ID_OVERVIEW_SYNC,
    ID_OVERVIEW_CPU,

    ID_CONTROLS_EXPOSURE = 9201,
    ID_CONTROLS_BALANCE,
    ID_CONTROLS_SCROLL_H,
    ID_CONTROLS_SCROLL_V,
    ID_CONTROLS_SEARCH,
    ID_CONTROLS_NOTES,

    ID_DATA_FILTER = 9301,
    ID_DATA_REFRESH,
    ID_DATA_TABLE,
    ID_DATA_QUEUE,

    ID_EXPANDED_STATIC_TITLE = 9401,
    ID_EXPANDED_STATIC_HELPER,
    ID_EXPANDED_CHECK_AUTOSAVE,
    ID_EXPANDED_CHECK_COMPACT,
    ID_EXPANDED_RADIO_GRID,
    ID_EXPANDED_RADIO_FOCUS,
    ID_EXPANDED_RADIO_FLOW,
    ID_EXPANDED_DIALOG,
    ID_EXPANDED_RESULT,

    ID_PAGE_OVERVIEW = 9501,
    ID_PAGE_CONTROLS,
    ID_PAGE_DATA,
    ID_PAGE_EXPANDED
};

enum class PageKind { Overview, Controls, Data, Expanded };

struct OverviewPanel {
    darkui::ComboBox profile;
    darkui::Button primary;
    darkui::Button secondary;
    darkui::Static status;
    darkui::ProgressBar sync;
    darkui::ProgressBar cpu;
};

struct ControlsPanel {
    darkui::Slider exposure;
    darkui::Slider balance;
    darkui::ScrollBar timeline;
    darkui::ScrollBar navigator;
    darkui::Edit search;
    darkui::Edit notes;
};

struct DataPanel {
    darkui::ComboBox filter;
    darkui::Button refresh;
    darkui::Table table;
    darkui::ListBox queue;
};

struct ExpandedPanel {
    darkui::Static headline;
    darkui::Static helper;
    darkui::CheckBox autoSave;
    darkui::CheckBox compact;
    darkui::RadioButton grid;
    darkui::RadioButton focus;
    darkui::RadioButton flow;
    darkui::Button dialogButton;
    darkui::Static result;
};

struct AppState {
    darkui::Theme theme;
    int themeIndex = 0;

    darkui::Tab tab;
    darkui::Button themeGraphite;
    darkui::Button themeEmber;
    darkui::Button themeGlacier;
    darkui::Button themeMoss;
    darkui::Button themeMono;

    HWND overviewPage = nullptr;
    HWND controlsPage = nullptr;
    HWND dataPage = nullptr;
    HWND expandedPage = nullptr;

    OverviewPanel overview;
    ControlsPanel controls;
    DataPanel data;
    ExpandedPanel expanded;

    HBRUSH windowBrush = nullptr;
    HFONT titleFont = nullptr;
    HFONT subtitleFont = nullptr;
    HFONT sectionFont = nullptr;
    HFONT bodyFont = nullptr;
};

struct PageState {
    AppState* app = nullptr;
    PageKind kind = PageKind::Overview;
};

void ApplyThemeToControls(AppState* state);

RECT InsetRectCopy(const RECT& rect, int dx, int dy) {
    RECT rc = rect;
    rc.left += dx; rc.top += dy; rc.right -= dx; rc.bottom -= dy;
    return rc;
}

RECT SliceTop(const RECT& rect, int height) {
    RECT rc = rect;
    rc.bottom = std::min(rect.bottom, rect.top + height);
    return rc;
}

RECT SliceLeft(const RECT& rect, int width) {
    RECT rc = rect;
    rc.right = std::min(rect.right, rect.left + width);
    return rc;
}

void DrawLabel(HDC dc, HFONT font, COLORREF color, const RECT& rect, const wchar_t* text, UINT format) {
    HFONT oldFont = font ? reinterpret_cast<HFONT>(SelectObject(dc, font)) : nullptr;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, const_cast<RECT*>(&rect), format | DT_NOPREFIX);
    if (oldFont) SelectObject(dc, oldFont);
}

void FillRoundedRect(HDC dc, const RECT& rect, int radius, COLORREF fill, COLORREF border) {
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);
    DeleteObject(pen);
}

darkui::Theme MakeThemeByIndex(int index) {
    switch (index) {
    case 1: return darkui::MakePresetTheme(darkui::ThemePreset::Ember);
    case 2: return darkui::MakePresetTheme(darkui::ThemePreset::Glacier);
    case 3: return darkui::MakePresetTheme(darkui::ThemePreset::Moss);
    case 4: return darkui::MakePresetTheme(darkui::ThemePreset::Mono);
    default: return darkui::MakePresetTheme(darkui::ThemePreset::Graphite);
    }
}

const wchar_t* ThemeName(int index) {
    switch (index) {
    case 1: return L"Ember";
    case 2: return L"Glacier";
    case 3: return L"Moss";
    case 4: return L"Mono";
    default: return L"Graphite";
    }
}

void ApplyWindowCaptionTheme(HWND window) {
    const BOOL immersive = TRUE;
    const COLORREF black = RGB(0, 0, 0);
    const COLORREF white = RGB(255, 255, 255);
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &immersive, sizeof(immersive));
    DwmSetWindowAttribute(window, DWMWA_CAPTION_COLOR, &black, sizeof(black));
    DwmSetWindowAttribute(window, DWMWA_TEXT_COLOR, &white, sizeof(white));
}

void RecreateWindowBrush(AppState* state) {
    if (state->windowBrush) DeleteObject(state->windowBrush);
    state->windowBrush = CreateSolidBrush(state->theme.background);
}

bool RecreateFonts(AppState* state) {
    darkui::FontSpec titleSpec = state->theme.uiFont;
    titleSpec.height -= 12;
    titleSpec.weight = FW_SEMIBOLD;

    darkui::FontSpec subtitleSpec = state->theme.uiFont;
    subtitleSpec.height += 2;

    darkui::FontSpec sectionSpec = state->theme.uiFont;
    sectionSpec.height -= 2;
    sectionSpec.weight = FW_SEMIBOLD;

    HFONT newTitleFont = darkui::CreateFont(titleSpec);
    HFONT newSubtitleFont = darkui::CreateFont(subtitleSpec);
    HFONT newSectionFont = darkui::CreateFont(sectionSpec);
    HFONT newBodyFont = darkui::CreateFont(state->theme.uiFont);
    if (!newTitleFont || !newSubtitleFont || !newSectionFont || !newBodyFont) {
        if (newTitleFont) DeleteObject(newTitleFont);
        if (newSubtitleFont) DeleteObject(newSubtitleFont);
        if (newSectionFont) DeleteObject(newSectionFont);
        if (newBodyFont) DeleteObject(newBodyFont);
        return false;
    }

    if (state->titleFont) DeleteObject(state->titleFont);
    if (state->subtitleFont) DeleteObject(state->subtitleFont);
    if (state->sectionFont) DeleteObject(state->sectionFont);
    if (state->bodyFont) DeleteObject(state->bodyFont);

    state->titleFont = newTitleFont;
    state->subtitleFont = newSubtitleFont;
    state->sectionFont = newSectionFont;
    state->bodyFont = newBodyFont;
    return true;
}

void UpdateExpandedSummary(AppState* state) {
    std::wstring text = L"Theme: ";
    text += ThemeName(state->themeIndex);
    text += L"\nLayout: ";
    if (state->expanded.grid.GetChecked()) text += L"Grid";
    else if (state->expanded.focus.GetChecked()) text += L"Focus";
    else text += L"Flow";
    text += L"\nAutosave: ";
    text += state->expanded.autoSave.GetChecked() ? L"On" : L"Off";
    text += L"\nCompact Rails: ";
    text += state->expanded.compact.GetChecked() ? L"On" : L"Off";
    state->expanded.result.SetText(text);
}

void RefreshDataRows(AppState* state) {
    const int filter = state->data.filter.GetSelection();
    std::vector<darkui::TableRow> rows{
        {L"Campaign Boards", L"Cloud", L"Ready", L"4.2 GB", L"12:44"},
        {L"Studio Takes", L"Local", L"Queued", L"2.1 GB", L"11:02"},
        {L"Audio Stems", L"NAS", L"Ready", L"918 MB", L"09:31"},
        {L"Launch Review", L"Archive", L"Locked", L"690 MB", L"Yesterday"},
        {L"Proxy Drafts", L"Cloud", L"Syncing", L"6.8 GB", L"Now"},
        {L"Final Captures", L"SSD", L"Ready", L"1.7 GB", L"14:08"}
    };
    if (filter == 1) rows.erase(std::remove_if(rows.begin(), rows.end(), [](const darkui::TableRow& r) { return r[2] != L"Ready"; }), rows.end());
    if (filter == 2) rows.erase(std::remove_if(rows.begin(), rows.end(), [](const darkui::TableRow& r) { return r[1] != L"Cloud"; }), rows.end());
    state->data.table.SetRows(rows);
}

void ShowExpandedDialog(HWND ownerWindow, AppState* state) {
    const auto result = darkui::ShowConfirmDialog(ownerWindow,
                                                  9701,
                                                  state->theme,
                                                  L"Publish Session",
                                                  L"Apply the current expanded controls profile to every editing station in the workspace?",
                                                  L"Publish",
                                                  L"Cancel");
    state->expanded.result.SetText(result == darkui::Dialog::Result::Confirm ? L"Dialog result: Published" : L"Dialog result: Cancelled");
}

void CleanupAppState(AppState* state) {
    if (!state) return;
    if (state->windowBrush) DeleteObject(state->windowBrush);
    if (state->titleFont) DeleteObject(state->titleFont);
    if (state->subtitleFont) DeleteObject(state->subtitleFont);
    if (state->sectionFont) DeleteObject(state->sectionFont);
    if (state->bodyFont) DeleteObject(state->bodyFont);
    delete state;
}

void ApplyTheme(AppState* state, HWND window, int themeIndex) {
    state->themeIndex = themeIndex;
    state->theme = MakeThemeByIndex(themeIndex);
    RecreateWindowBrush(state);
    if (!RecreateFonts(state)) return;
    ApplyWindowCaptionTheme(window);
    ApplyThemeToControls(state);
    InvalidateRect(window, nullptr, TRUE);
    if (state->overviewPage) InvalidateRect(state->overviewPage, nullptr, TRUE);
    if (state->controlsPage) InvalidateRect(state->controlsPage, nullptr, TRUE);
    if (state->dataPage) InvalidateRect(state->dataPage, nullptr, TRUE);
    if (state->expandedPage) InvalidateRect(state->expandedPage, nullptr, TRUE);
}

void ApplyThemeToControls(AppState* state) {
    state->tab.SetTheme(state->theme);
    state->themeGraphite.SetTheme(state->theme); state->themeGraphite.SetSurfaceColor(state->themeIndex == 0 ? state->theme.panel : state->theme.background);
    state->themeEmber.SetTheme(state->theme); state->themeEmber.SetSurfaceColor(state->themeIndex == 1 ? state->theme.panel : state->theme.background);
    state->themeGlacier.SetTheme(state->theme); state->themeGlacier.SetSurfaceColor(state->themeIndex == 2 ? state->theme.panel : state->theme.background);
    state->themeMoss.SetTheme(state->theme); state->themeMoss.SetSurfaceColor(state->themeIndex == 3 ? state->theme.panel : state->theme.background);
    state->themeMono.SetTheme(state->theme); state->themeMono.SetSurfaceColor(state->themeIndex == 4 ? state->theme.panel : state->theme.background);

    state->overview.profile.SetTheme(state->theme);
    state->overview.primary.SetTheme(state->theme); state->overview.primary.SetSurfaceColor(state->theme.panel);
    state->overview.secondary.SetTheme(state->theme); state->overview.secondary.SetSurfaceColor(state->theme.panel);
    state->overview.status.SetTheme(state->theme); state->overview.status.SetBackgroundColor(state->theme.panel);
    state->overview.sync.SetTheme(state->theme); state->overview.sync.SetSurfaceColor(state->theme.panel);
    state->overview.cpu.SetTheme(state->theme); state->overview.cpu.SetSurfaceColor(state->theme.panel);

    state->controls.exposure.SetTheme(state->theme);
    state->controls.balance.SetTheme(state->theme);
    state->controls.timeline.SetTheme(state->theme);
    state->controls.navigator.SetTheme(state->theme);
    state->controls.search.SetTheme(state->theme);
    state->controls.notes.SetTheme(state->theme);

    state->data.filter.SetTheme(state->theme);
    state->data.refresh.SetTheme(state->theme); state->data.refresh.SetSurfaceColor(state->theme.background);
    state->data.table.SetTheme(state->theme);
    state->data.queue.SetTheme(state->theme);

    state->expanded.headline.SetTheme(state->theme); state->expanded.headline.SetBackgroundColor(state->theme.panel);
    state->expanded.helper.SetTheme(state->theme); state->expanded.helper.SetBackgroundColor(state->theme.panel);
    state->expanded.autoSave.SetTheme(state->theme); state->expanded.autoSave.SetSurfaceColor(state->theme.panel);
    state->expanded.compact.SetTheme(state->theme); state->expanded.compact.SetSurfaceColor(state->theme.panel);
    state->expanded.grid.SetTheme(state->theme); state->expanded.grid.SetSurfaceColor(state->theme.panel);
    state->expanded.focus.SetTheme(state->theme); state->expanded.focus.SetSurfaceColor(state->theme.panel);
    state->expanded.flow.SetTheme(state->theme); state->expanded.flow.SetSurfaceColor(state->theme.panel);
    state->expanded.dialogButton.SetTheme(state->theme); state->expanded.dialogButton.SetSurfaceColor(state->theme.panel);
    state->expanded.result.SetTheme(state->theme); state->expanded.result.SetBackgroundColor(state->theme.panel);
    UpdateExpandedSummary(state);
}

void LayoutMainWindow(HWND window, AppState* state) {
    RECT client{};
    GetClientRect(window, &client);
    const int outer = 28;
    const int headerHeight = 104;
    const int buttonWidth = 102;
    const int buttonHeight = 36;
    const int gap = 10;
    const int right = client.right - outer;
    const int top = outer + 10;

    MoveWindow(state->themeMono.hwnd(), right - buttonWidth, top, buttonWidth, buttonHeight, TRUE);
    MoveWindow(state->themeMoss.hwnd(), right - buttonWidth * 2 - gap, top, buttonWidth, buttonHeight, TRUE);
    MoveWindow(state->themeGlacier.hwnd(), right - buttonWidth * 3 - gap * 2, top, buttonWidth, buttonHeight, TRUE);
    MoveWindow(state->themeEmber.hwnd(), right - buttonWidth * 4 - gap * 3, top, buttonWidth, buttonHeight, TRUE);
    MoveWindow(state->themeGraphite.hwnd(), right - buttonWidth * 5 - gap * 4, top, buttonWidth, buttonHeight, TRUE);

    MoveWindow(state->tab.hwnd(),
               outer,
               outer + headerHeight,
               std::max<LONG>(460, client.right - outer * 2),
               std::max<LONG>(460, client.bottom - outer * 2 - headerHeight),
               TRUE);
}

void LayoutOverviewPage(HWND page, AppState* state) {
    RECT client{}; GetClientRect(page, &client);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT top = SliceTop(content, 258);
    RECT bottom{content.left, top.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(top, ((top.right - top.left) - 20) / 2);
    RECT right{left.right + 20, top.top, top.right, top.bottom};
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);
    RECT bi = InsetRectCopy(bottom, 24, 24);

    MoveWindow(state->overview.profile.hwnd(), li.left, li.top + 54, li.right - li.left, 34, TRUE);
    MoveWindow(state->overview.primary.hwnd(), ri.left, ri.top + 58, 168, 38, TRUE);
    MoveWindow(state->overview.secondary.hwnd(), ri.left + 184, ri.top + 58, 168, 38, TRUE);
    MoveWindow(state->overview.status.hwnd(), ri.left, ri.top + 120, ri.right - ri.left, 48, TRUE);
    MoveWindow(state->overview.sync.hwnd(), bi.left, bi.top + 56, bi.right - bi.left, 34, TRUE);
    MoveWindow(state->overview.cpu.hwnd(), bi.left, bi.top + 128, bi.right - bi.left, 34, TRUE);
}

void LayoutControlsPage(HWND page, AppState* state) {
    RECT client{}; GetClientRect(page, &client);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT top = SliceTop(content, 260);
    RECT bottom{content.left, top.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(bottom, ((bottom.right - bottom.left) - 20) / 2);
    RECT right{left.right + 20, bottom.top, bottom.right, bottom.bottom};
    RECT ti = InsetRectCopy(top, 24, 24);
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);

    MoveWindow(state->controls.exposure.hwnd(), ti.left, ti.top + 62, ti.right - ti.left, 42, TRUE);
    MoveWindow(state->controls.balance.hwnd(), ti.left, ti.top + 156, ti.right - ti.left, 42, TRUE);
    MoveWindow(state->controls.timeline.hwnd(), li.left, li.top + 78, li.right - li.left, state->theme.scrollBarThickness + 12, TRUE);
    MoveWindow(state->controls.navigator.hwnd(), li.right - 22, li.top + 126, 18, std::max(100L, li.bottom - li.top - 144), TRUE);
    MoveWindow(state->controls.search.hwnd(), ri.left, ri.top + 62, ri.right - ri.left, 40, TRUE);
    MoveWindow(state->controls.notes.hwnd(), ri.left, ri.top + 122, ri.right - ri.left, std::max(118L, ri.bottom - ri.top - 136), TRUE);
}

void LayoutDataPage(HWND page, AppState* state) {
    RECT client{}; GetClientRect(page, &client);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT bar = SliceTop(content, 42);
    RECT bottom{content.left, bar.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(bottom, ((bottom.right - bottom.left) - 20) / 2);
    RECT right{left.right + 20, bottom.top, bottom.right, bottom.bottom};

    MoveWindow(state->data.filter.hwnd(), bar.left, bar.top, 240, 42, TRUE);
    MoveWindow(state->data.refresh.hwnd(), bar.right - 146, bar.top, 146, 42, TRUE);
    MoveWindow(state->data.table.hwnd(), left.left, left.top, left.right - left.left, left.bottom - left.top, TRUE);
    MoveWindow(state->data.queue.hwnd(), right.left, right.top, right.right - right.left, right.bottom - right.top, TRUE);
}

void LayoutExpandedPage(HWND page, AppState* state) {
    RECT client{}; GetClientRect(page, &client);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT left = SliceLeft(content, ((content.right - content.left) - 20) / 2);
    RECT right{left.right + 20, content.top, content.right, content.bottom};
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);

    MoveWindow(state->expanded.headline.hwnd(), li.left, li.top + 12, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.helper.hwnd(), li.left, li.top + 52, li.right - li.left, 62, TRUE);
    MoveWindow(state->expanded.autoSave.hwnd(), li.left, li.top + 138, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.compact.hwnd(), li.left, li.top + 176, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.grid.hwnd(), li.left, li.top + 232, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.focus.hwnd(), li.left, li.top + 268, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.flow.hwnd(), li.left, li.top + 304, li.right - li.left, 28, TRUE);
    MoveWindow(state->expanded.dialogButton.hwnd(), ri.left, ri.top + 56, 196, 40, TRUE);
    MoveWindow(state->expanded.result.hwnd(), ri.left, ri.top + 124, ri.right - ri.left, std::max(120L, ri.bottom - ri.top - 140), TRUE);
}

void LayoutPage(HWND page, AppState* state, PageKind kind) {
    switch (kind) {
    case PageKind::Overview: LayoutOverviewPage(page, state); break;
    case PageKind::Controls: LayoutControlsPage(page, state); break;
    case PageKind::Data: LayoutDataPage(page, state); break;
    case PageKind::Expanded: LayoutExpandedPage(page, state); break;
    }
}

void PaintMainWindow(HWND window, HDC dc, AppState* state) {
    RECT client{}; GetClientRect(window, &client);
    FillRect(dc, &client, state->windowBrush);
    DrawLabel(dc, state->titleFont, state->theme.text, RECT{28, 24, client.right - 28, 58}, L"DarkUI Semantic Showcase", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc,
              state->subtitleFont,
              state->theme.mutedText,
              RECT{28, 62, client.right - 560, 92},
              L"Five semantic palettes drive every control from one theme entry. The new controls live in the Expanded page, and the window title bar stays black.",
              DT_LEFT | DT_TOP | DT_WORDBREAK);
}

void PaintOverviewPage(HWND page, HDC dc, PageState* state) {
    RECT client{}; GetClientRect(page, &client); FillRect(dc, &client, state->app->windowBrush);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT top = SliceTop(content, 258);
    RECT bottom{content.left, top.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(top, ((top.right - top.left) - 20) / 2);
    RECT right{left.right + 20, top.top, top.right, top.bottom};
    FillRoundedRect(dc, left, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, right, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, bottom, 24, state->app->theme.panel, state->app->theme.border);
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);
    RECT bi = InsetRectCopy(bottom, 24, 24);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(li, 26), L"Theme Inputs", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{li.left, li.top + 30, li.right, li.top + 52}, L"Latest semantic theme entry through one shared Theme object.", DT_LEFT | DT_TOP | DT_WORDBREAK);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(ri, 26), L"Actions And Status", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{ri.left, ri.top + 30, ri.right, ri.top + 52}, L"Buttons, static text, and progress cards all follow the same palette.", DT_LEFT | DT_TOP | DT_WORDBREAK);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(bi, 26), L"Live Progress", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{bi.left, bi.top + 32, bi.right, bi.top + 50}, L"Background sync", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{bi.left, bi.top + 104, bi.right, bi.top + 122}, L"Render capacity", DT_LEFT | DT_TOP | DT_SINGLELINE);
}

void PaintControlsPage(HWND page, HDC dc, PageState* state) {
    RECT client{}; GetClientRect(page, &client); FillRect(dc, &client, state->app->windowBrush);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT top = SliceTop(content, 260);
    RECT bottom{content.left, top.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(bottom, ((bottom.right - bottom.left) - 20) / 2);
    RECT right{left.right + 20, bottom.top, bottom.right, bottom.bottom};
    FillRoundedRect(dc, top, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, left, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, right, 24, state->app->theme.panel, state->app->theme.border);
    RECT ti = InsetRectCopy(top, 24, 24);
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(ti, 26), L"Sliders And Motion", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{ti.left, ti.top + 34, ti.right, ti.top + 52}, L"Exposure", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{ti.left, ti.top + 128, ti.right, ti.top + 146}, L"Balance", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(li, 26), L"Custom Scrollbars", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{li.left, li.top + 34, li.right, li.top + 56}, L"Horizontal timeline and vertical navigation rail.", DT_LEFT | DT_TOP | DT_WORDBREAK);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(ri, 26), L"Dark Edit Surfaces", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{ri.left, ri.top + 34, ri.right, ri.top + 56}, L"Single-line search plus multiline notes.", DT_LEFT | DT_TOP | DT_WORDBREAK);
}

void PaintDataPage(HWND page, HDC dc, PageState* state) {
    RECT client{}; GetClientRect(page, &client); FillRect(dc, &client, state->app->windowBrush);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT bar = SliceTop(content, 42);
    RECT bottom{content.left, bar.bottom + 20, content.right, content.bottom};
    RECT left = SliceLeft(bottom, ((bottom.right - bottom.left) - 20) / 2);
    RECT right{left.right + 20, bottom.top, bottom.right, bottom.bottom};
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, RECT{content.left, content.top - 4, content.right, content.top + 22}, L"Data Density", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{content.left, content.top + 24, content.right, content.top + 46}, L"Custom table and list box using the same semantic palette and font system.", DT_LEFT | DT_TOP | DT_WORDBREAK);
    FillRoundedRect(dc, left, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, right, 24, state->app->theme.panel, state->app->theme.border);
}

void PaintExpandedPage(HWND page, HDC dc, PageState* state) {
    RECT client{}; GetClientRect(page, &client); FillRect(dc, &client, state->app->windowBrush);
    RECT content = InsetRectCopy(client, 28, 28);
    RECT left = SliceLeft(content, ((content.right - content.left) - 20) / 2);
    RECT right{left.right + 20, content.top, content.right, content.bottom};
    FillRoundedRect(dc, left, 24, state->app->theme.panel, state->app->theme.border);
    FillRoundedRect(dc, right, 24, state->app->theme.panel, state->app->theme.border);
    RECT li = InsetRectCopy(left, 24, 24);
    RECT ri = InsetRectCopy(right, 24, 24);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(li, 26), L"New Controls", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{li.left, li.top + 34, li.right, li.top + 56}, L"Static, CheckBox, RadioButton, and a semantic-state summary.", DT_LEFT | DT_TOP | DT_WORDBREAK);
    DrawLabel(dc, state->app->sectionFont, state->app->theme.text, SliceTop(ri, 26), L"Dialog Trigger", DT_LEFT | DT_TOP | DT_SINGLELINE);
    DrawLabel(dc, state->app->bodyFont, state->app->theme.mutedText, RECT{ri.left, ri.top + 34, ri.right, ri.top + 56}, L"Open the custom dark dialog without leaving the showcase.", DT_LEFT | DT_TOP | DT_WORDBREAK);
}

void PaintPage(HWND page, HDC dc, PageState* state) {
    switch (state->kind) {
    case PageKind::Overview: PaintOverviewPage(page, dc, state); break;
    case PageKind::Controls: PaintControlsPage(page, dc, state); break;
    case PageKind::Data: PaintDataPage(page, dc, state); break;
    case PageKind::Expanded: PaintExpandedPage(page, dc, state); break;
    }
}

bool CreatePageWindow(HWND parent, int controlId, PageKind kind, AppState* app, HWND* outPage) {
    auto* pageState = new PageState();
    pageState->app = app;
    pageState->kind = kind;
    HWND page = CreateWindowExW(0, kPageClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(controlId)), reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(parent, GWLP_HINSTANCE)), pageState);
    if (!page) { delete pageState; return false; }
    *outPage = page;
    return true;
}

bool CreateOverviewControls(AppState* app) {
    if (!app->overview.profile.Create(app->overviewPage, ID_OVERVIEW_PROFILE, app->theme)) return false;
    if (!app->overview.primary.Create(app->overviewPage, ID_OVERVIEW_PRIMARY, L"Sync Cluster", app->theme)) return false;
    if (!app->overview.secondary.Create(app->overviewPage, ID_OVERVIEW_SECONDARY, L"Share Preview", app->theme)) return false;
    if (!app->overview.status.Create(app->overviewPage, ID_OVERVIEW_STATUS, L"Semantic palette active across every page", app->theme, WS_CHILD | WS_VISIBLE | SS_LEFT)) return false;
    if (!app->overview.sync.Create(app->overviewPage, ID_OVERVIEW_SYNC, app->theme)) return false;
    if (!app->overview.cpu.Create(app->overviewPage, ID_OVERVIEW_CPU, app->theme)) return false;
    app->overview.primary.SetCornerRadius(14); app->overview.primary.SetSurfaceColor(app->theme.panel);
    app->overview.secondary.SetCornerRadius(14); app->overview.secondary.SetSurfaceColor(app->theme.panel);
    app->overview.status.SetBackgroundColor(app->theme.panel); app->overview.status.SetTextFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    app->overview.profile.SetItems({{L"Studio Executive", 1, true}, {L"Asset Review", 2, false}, {L"Broadcast Board", 3, false}});
    app->overview.profile.SetSelection(0);
    app->overview.sync.SetRange(0, 100); app->overview.sync.SetValue(74); app->overview.sync.SetSurfaceColor(app->theme.panel);
    app->overview.cpu.SetRange(0, 100); app->overview.cpu.SetValue(61); app->overview.cpu.SetSurfaceColor(app->theme.panel); app->overview.cpu.SetShowPercentage(false);
    return true;
}

bool CreateControlsPanel(AppState* app) {
    if (!app->controls.exposure.Create(app->controlsPage, ID_CONTROLS_EXPOSURE, app->theme)) return false;
    if (!app->controls.balance.Create(app->controlsPage, ID_CONTROLS_BALANCE, app->theme)) return false;
    if (!app->controls.timeline.Create(app->controlsPage, ID_CONTROLS_SCROLL_H, false, app->theme)) return false;
    if (!app->controls.navigator.Create(app->controlsPage, ID_CONTROLS_SCROLL_V, true, app->theme)) return false;
    if (!app->controls.search.Create(app->controlsPage, ID_CONTROLS_SEARCH, L"", app->theme)) return false;
    if (!app->controls.notes.Create(app->controlsPage, ID_CONTROLS_NOTES, L"Dark multiline edit\nwith the custom dark vertical scrollbar\nlinked to the shared theme.", app->theme, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL)) return false;
    app->controls.exposure.SetRange(0, 100); app->controls.exposure.SetValue(68); app->controls.exposure.SetShowTicks(true); app->controls.exposure.SetTickCount(9);
    app->controls.balance.SetRange(0, 100); app->controls.balance.SetValue(42); app->controls.balance.SetShowTicks(true); app->controls.balance.SetTickCount(7);
    app->controls.timeline.SetRange(0, 100); app->controls.timeline.SetPageSize(20); app->controls.timeline.SetValue(34);
    app->controls.navigator.SetRange(0, 100); app->controls.navigator.SetPageSize(18); app->controls.navigator.SetValue(26);
    app->controls.search.SetCueBanner(L"Search sessions"); app->controls.search.SetCornerRadius(14);
    app->controls.notes.SetCueBanner(L"Write notes here"); app->controls.notes.SetCornerRadius(16);
    return true;
}

bool CreateDataPanel(AppState* app) {
    if (!app->data.filter.Create(app->dataPage, ID_DATA_FILTER, app->theme)) return false;
    if (!app->data.refresh.Create(app->dataPage, ID_DATA_REFRESH, L"Refresh", app->theme)) return false;
    if (!app->data.table.Create(app->dataPage, ID_DATA_TABLE, app->theme)) return false;
    if (!app->data.queue.Create(app->dataPage, ID_DATA_QUEUE, app->theme)) return false;
    app->data.refresh.SetCornerRadius(14); app->data.refresh.SetSurfaceColor(app->theme.background);
    app->data.filter.SetItems({{L"All transfers", 0, true}, {L"Ready only", 1, false}, {L"Cloud only", 2, false}}); app->data.filter.SetSelection(0);
    app->data.table.SetColumns({{L"Collection", 220, LVCFMT_LEFT}, {L"Source", 100, LVCFMT_LEFT}, {L"State", 110, LVCFMT_LEFT}, {L"Size", 100, LVCFMT_RIGHT}, {L"Updated", 110, LVCFMT_LEFT}});
    app->data.table.SetDrawEmptyGrid(true);
    app->data.queue.SetCornerRadius(18);
    app->data.queue.SetItems({{L"Queued color pass", 1}, {L"Archive sync", 2}, {L"Proxy rebuild", 3}, {L"Review package", 4}, {L"Audio conform", 5}});
    app->data.queue.SetSelection(0);
    RefreshDataRows(app);
    return true;
}

bool CreateExpandedPanel(AppState* app) {
    if (!app->expanded.headline.Create(app->expandedPage, ID_EXPANDED_STATIC_TITLE, L"Extended Components", app->theme, WS_CHILD | WS_VISIBLE | SS_LEFT)) return false;
    if (!app->expanded.helper.Create(app->expandedPage, ID_EXPANDED_STATIC_HELPER, L"These controls were added after the original showcase and now participate in the same semantic theme system.", app->theme, WS_CHILD | WS_VISIBLE | SS_LEFT)) return false;
    if (!app->expanded.autoSave.Create(app->expandedPage, ID_EXPANDED_CHECK_AUTOSAVE, L"Enable automatic recovery snapshots", app->theme)) return false;
    if (!app->expanded.compact.Create(app->expandedPage, ID_EXPANDED_CHECK_COMPACT, L"Compact side rails", app->theme)) return false;
    if (!app->expanded.grid.Create(app->expandedPage, ID_EXPANDED_RADIO_GRID, L"Grid layout", app->theme, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP)) return false;
    if (!app->expanded.focus.Create(app->expandedPage, ID_EXPANDED_RADIO_FOCUS, L"Focus layout", app->theme)) return false;
    if (!app->expanded.flow.Create(app->expandedPage, ID_EXPANDED_RADIO_FLOW, L"Flow layout", app->theme)) return false;
    if (!app->expanded.dialogButton.Create(app->expandedPage, ID_EXPANDED_DIALOG, L"Open Dialog", app->theme)) return false;
    if (!app->expanded.result.Create(app->expandedPage, ID_EXPANDED_RESULT, L"", app->theme, WS_CHILD | WS_VISIBLE | SS_LEFT)) return false;
    app->expanded.headline.SetBackgroundColor(app->theme.panel); app->expanded.headline.SetTextFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    app->expanded.helper.SetBackgroundColor(app->theme.panel); app->expanded.helper.SetTextFormat(DT_LEFT | DT_TOP);
    app->expanded.autoSave.SetSurfaceColor(app->theme.panel); app->expanded.compact.SetSurfaceColor(app->theme.panel);
    app->expanded.grid.SetSurfaceColor(app->theme.panel); app->expanded.focus.SetSurfaceColor(app->theme.panel); app->expanded.flow.SetSurfaceColor(app->theme.panel);
    app->expanded.dialogButton.SetCornerRadius(14); app->expanded.dialogButton.SetSurfaceColor(app->theme.panel);
    app->expanded.result.SetBackgroundColor(app->theme.panel); app->expanded.result.SetTextFormat(DT_LEFT | DT_WORDBREAK);
    app->expanded.autoSave.SetChecked(true); app->expanded.grid.SetChecked(true); UpdateExpandedSummary(app);
    return true;
}

bool CreateShowcase(AppState* state, HWND window) {
    if (!state->themeGraphite.Create(window, ID_THEME_GRAPHITE, L"Graphite", state->theme)) return false;
    if (!state->themeEmber.Create(window, ID_THEME_EMBER, L"Ember", state->theme)) return false;
    if (!state->themeGlacier.Create(window, ID_THEME_GLACIER, L"Glacier", state->theme)) return false;
    if (!state->themeMoss.Create(window, ID_THEME_MOSS, L"Moss", state->theme)) return false;
    if (!state->themeMono.Create(window, ID_THEME_MONO, L"Mono", state->theme)) return false;
    if (!state->tab.Create(window, ID_MAIN_TAB, state->theme)) return false;

    state->themeGraphite.SetCornerRadius(14);
    state->themeEmber.SetCornerRadius(14);
    state->themeGlacier.SetCornerRadius(14);
    state->themeMoss.SetCornerRadius(14);
    state->themeMono.SetCornerRadius(14);

    state->tab.SetItems({{L"Overview", 1}, {L"Controls", 2}, {L"Data", 3}, {L"Expanded", 4}});

    if (!CreatePageWindow(state->tab.hwnd(), ID_PAGE_OVERVIEW, PageKind::Overview, state, &state->overviewPage)) return false;
    if (!CreatePageWindow(state->tab.hwnd(), ID_PAGE_CONTROLS, PageKind::Controls, state, &state->controlsPage)) return false;
    if (!CreatePageWindow(state->tab.hwnd(), ID_PAGE_DATA, PageKind::Data, state, &state->dataPage)) return false;
    if (!CreatePageWindow(state->tab.hwnd(), ID_PAGE_EXPANDED, PageKind::Expanded, state, &state->expandedPage)) return false;

    state->tab.AttachPage(0, state->overviewPage);
    state->tab.AttachPage(1, state->controlsPage);
    state->tab.AttachPage(2, state->dataPage);
    state->tab.AttachPage(3, state->expandedPage);
    state->tab.SetSelection(0, false);

    if (!CreateOverviewControls(state)) return false;
    if (!CreateControlsPanel(state)) return false;
    if (!CreateDataPanel(state)) return false;
    if (!CreateExpandedPanel(state)) return false;

    ApplyThemeToControls(state);
    LayoutMainWindow(window, state);
    return true;
}

LRESULT CALLBACK ShowcasePageProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<PageState*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        auto* created = reinterpret_cast<PageState*>(create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(created));
        return 0;
    }
    case WM_SIZE:
        if (state && state->app) LayoutPage(window, state->app, state->kind);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        if (state && state->app) {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(window, &ps);
            PaintPage(window, dc, state);
            EndPaint(window, &ps);
            return 0;
        }
        break;
    case WM_COMMAND:
        if (state && state->app && state->app->tab.parent()) {
            return SendMessageW(state->app->tab.parent(), WM_COMMAND, wParam, lParam);
        }
        break;
    case WM_NOTIFY:
        if (state && state->app && state->app->tab.parent()) {
            return SendMessageW(state->app->tab.parent(), WM_NOTIFY, wParam, lParam);
        }
        break;
    case WM_NCDESTROY:
        delete state;
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

LRESULT CALLBACK ShowcaseWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<AppState*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        auto* created = new AppState();
        created->theme = MakeThemeByIndex(0);
        created->themeIndex = 0;
        RecreateWindowBrush(created);
        if (!created->windowBrush || !RecreateFonts(created)) {
            CleanupAppState(created);
            return -1;
        }
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(created));
        ApplyWindowCaptionTheme(window);
        if (!CreateShowcase(created, window)) {
            CleanupAppState(created);
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            return -1;
        }
        return 0;
    }
    case WM_SIZE:
        if (state) LayoutMainWindow(window, state);
        return 0;
    case WM_GETMINMAXINFO: {
        auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
        if (info) {
            info->ptMinTrackSize.x = 1180;
            info->ptMinTrackSize.y = 760;
            return 0;
        }
        break;
    }
    case WM_COMMAND:
        if (!state) break;
        if (HIWORD(wParam) == BN_CLICKED) {
            switch (LOWORD(wParam)) {
            case ID_THEME_GRAPHITE: ApplyTheme(state, window, 0); return 0;
            case ID_THEME_EMBER: ApplyTheme(state, window, 1); return 0;
            case ID_THEME_GLACIER: ApplyTheme(state, window, 2); return 0;
            case ID_THEME_MOSS: ApplyTheme(state, window, 3); return 0;
            case ID_THEME_MONO: ApplyTheme(state, window, 4); return 0;
            case ID_DATA_REFRESH: RefreshDataRows(state); return 0;
            case ID_EXPANDED_CHECK_AUTOSAVE:
            case ID_EXPANDED_CHECK_COMPACT:
            case ID_EXPANDED_RADIO_GRID:
            case ID_EXPANDED_RADIO_FOCUS:
            case ID_EXPANDED_RADIO_FLOW:
                UpdateExpandedSummary(state);
                return 0;
            case ID_EXPANDED_DIALOG:
                ShowExpandedDialog(window, state);
                return 0;
            default:
                break;
            }
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_DATA_FILTER) {
            RefreshDataRows(state);
            return 0;
        }
        break;
    case WM_NOTIFY:
        if (state) {
            auto* hdr = reinterpret_cast<NMHDR*>(lParam);
            if (hdr && hdr->hwndFrom == state->tab.hwnd() && hdr->code == TCN_SELCHANGE) {
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }
        }
        break;
    case WM_ERASEBKGND:
        if (state) {
            RECT rect{};
            GetClientRect(window, &rect);
            FillRect(reinterpret_cast<HDC>(wParam), &rect, state->windowBrush);
            return 1;
        }
        break;
    case WM_PAINT:
        if (state) {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(window, &ps);
            PaintMainWindow(window, dc, state);
            EndPaint(window, &ps);
            return 0;
        }
        break;
    case WM_DESTROY:
        CleanupAppState(state);
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
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&icc);

    WNDCLASSEXW pageClass{};
    pageClass.cbSize = sizeof(pageClass);
    pageClass.lpfnWndProc = ShowcasePageProc;
    pageClass.hInstance = instance;
    pageClass.lpszClassName = kPageClassName;
    pageClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    pageClass.hbrBackground = nullptr;
    if (!RegisterClassExW(&pageClass)) return 0;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ShowcaseWindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = kDemoClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    if (!RegisterClassExW(&wc)) return 0;

    HWND window = CreateWindowExW(0,
                                  kDemoClassName,
                                  kDemoTitle,
                                  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  1320,
                                  860,
                                  nullptr,
                                  nullptr,
                                  instance,
                                  nullptr);
    if (!window) return 0;

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
