// windows.h first, before headers that use "using namespace std;".
#include <windows.h>
#include <shellapi.h>

#include "action/ActionExecutor.h"

using namespace std;

// "what" is an exe name (found via App Paths or PATH) or a URI like "spotify:"
static bool openApp(const string& what) {
    HINSTANCE result = ShellExecuteA(nullptr, "open", what.c_str(),
                                     nullptr, nullptr, SW_SHOWNORMAL);
    return (INT_PTR)result > 32; // old API convention: > 32 means success
}

// press + release; Windows routes media keys to whatever player is running
static bool tapKey(WORD vk) {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vk;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(2, inputs, sizeof(INPUT)) == 2;
}

// one press moves the volume by ~2%, too little to hear
static bool tapKeyTimes(WORD vk, int times) {
    for (int i = 0; i < times; i++) {
        if (!tapKey(vk)) {
            return false;
        }
    }
    return true;
}

bool ActionExecutor::execute(const Command& cmd) {
    switch (cmd.type) {
    case CMD_OPEN_APP:
        return openApp(cmd.arg);
    case CMD_NEXT_TRACK:
        return tapKey(VK_MEDIA_NEXT_TRACK);
    case CMD_PREV_TRACK:
        return tapKey(VK_MEDIA_PREV_TRACK);
    case CMD_PLAY_PAUSE:
        return tapKey(VK_MEDIA_PLAY_PAUSE);
    case CMD_VOLUME_UP:
        return tapKeyTimes(VK_VOLUME_UP, 5);
    case CMD_VOLUME_DOWN:
        return tapKeyTimes(VK_VOLUME_DOWN, 5);
    case CMD_MUTE_TOGGLE:
        return tapKey(VK_VOLUME_MUTE);
    default:
        return false;
    }
}
