#pragma once

#include "darkui/button.h"
#include "darkui/static.h"

#include <memory>
#include <string>

namespace darkui {

// Modal dark dialog with a custom title bar, body area, and confirm/cancel buttons.
// Usage:
// - Create the dialog with Create().
// - Use SetMessage() for a simple centered message body, or create your own child
//   controls inside content_hwnd() for custom layouts.
// - Call ShowModal() to display the popup and run a local modal message loop.
// Notes:
// - Unknown WM_COMMAND / WM_NOTIFY messages are forwarded to the owner window.
// - The built-in message label can be hidden when you want a fully custom body.
class Dialog {
public:
    enum class Result {
        None = 0,
        Confirm = 1,
        Cancel = 2
    };

    struct Impl;

    Dialog();
    ~Dialog();

    Dialog(const Dialog&) = delete;
    Dialog& operator=(const Dialog&) = delete;

    // Creates the dialog window and its built-in controls.
    // Parameters:
    // - owner: Owner window disabled during modal display.
    // - controlId: Logical identifier stored by the wrapper.
    // - title: Initial title shown in the custom title bar.
    // - theme: Visual theme used for the dialog surface and buttons.
    // - width: Initial dialog width in pixels.
    // - height: Initial dialog height in pixels.
    // Returns:
    // - true on success.
    // - false on failure.
    bool Create(HWND owner, int controlId, const std::wstring& title, const Theme& theme = Theme{}, int width = 480, int height = 280);
    // Destroys the popup window and all built-in child controls.
    void Destroy();

    // Returns the popup window handle.
    HWND hwnd() const { return dialogHwnd_; }
    // Returns the owner window handle passed to Create().
    HWND owner() const { return ownerHwnd_; }
    // Returns the central content host window.
    // Create custom child controls inside this host when you do not want to use
    // the built-in message label.
    HWND content_hwnd() const { return contentHwnd_; }
    // Returns the built-in confirm button handle.
    HWND confirm_button() const { return confirmButton_.hwnd(); }
    // Returns the built-in cancel button handle.
    HWND cancel_button() const { return cancelButton_.hwnd(); }
    // Returns the built-in message label handle.
    HWND message_hwnd() const { return messageLabel_.hwnd(); }
    // Returns the current modal result.
    Result modal_result() const { return modalResult_; }
    // Returns the active theme.
    const Theme& theme() const { return theme_; }

    // Replaces the current theme and repaints the dialog.
    void SetTheme(const Theme& theme);
    // Updates the title-bar text.
    void SetTitle(const std::wstring& title);
    // Updates the built-in centered message body and makes it visible.
    void SetMessage(const std::wstring& text);
    // Returns the current built-in message text.
    std::wstring GetMessage() const;
    // Updates the confirm button text.
    void SetConfirmText(const std::wstring& text);
    // Updates the cancel button text.
    void SetCancelText(const std::wstring& text);
    // Shows or hides the built-in message label.
    void SetMessageVisible(bool visible);
    // Shows or hides the cancel button.
    void SetCancelVisible(bool visible);
    // Sets the modal result and closes the dialog.
    void EndDialog(Result result);
    // Shows the dialog modally and returns the final modal result.
    Result ShowModal();

private:
    std::unique_ptr<Impl> impl_;
    HWND ownerHwnd_ = nullptr;
    HWND dialogHwnd_ = nullptr;
    HWND contentHwnd_ = nullptr;
    int controlId_ = 0;
    int width_ = 480;
    int height_ = 280;
    bool messageVisible_ = true;
    bool cancelVisible_ = true;
    bool modalRunning_ = false;
    Result modalResult_ = Result::None;
    Theme theme_{};
    Button confirmButton_{};
    Button cancelButton_{};
    Static titleLabel_{};
    Static messageLabel_{};
};

}  // namespace darkui
