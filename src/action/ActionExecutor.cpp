// windows.h first, before headers that use "using namespace std;".
#include <windows.h>
#include <shellapi.h>

#include "action/ActionExecutor.h"

using namespace std;

static bool openChrome() {
    // Windows resolves "chrome.exe" through the App Paths registry key,
    // so this works no matter where Chrome is installed.
    HINSTANCE result = ShellExecuteA(nullptr, "open", "chrome.exe",
                                     nullptr, nullptr, SW_SHOWNORMAL);
    // ShellExecute returns a value > 32 on success (old API convention).
    return (INT_PTR)result > 32;
}

// Taps a key once (press + release). Media keys like VK_MEDIA_NEXT_TRACK
// are handled system-wide, so this controls whatever player is running.
static bool tapKey(WORD vk) {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vk;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(2, inputs, sizeof(INPUT)) == 2;
}

bool ActionExecutor::execute(const Command& cmd) {
    switch (cmd.type) {
    case CMD_OPEN_CHROME:
        return openChrome();
    case CMD_NEXT_TRACK:
        return tapKey(VK_MEDIA_NEXT_TRACK);
    case CMD_PREV_TRACK:
        return tapKey(VK_MEDIA_PREV_TRACK);
    case CMD_PLAY_PAUSE:
        return tapKey(VK_MEDIA_PLAY_PAUSE);
    default:
        return false; // the other commands come in the next steps
    }
}
