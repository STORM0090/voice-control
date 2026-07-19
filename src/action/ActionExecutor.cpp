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

bool ActionExecutor::execute(const Command& cmd) {
    switch (cmd.type) {
    case CMD_OPEN_CHROME:
        return openChrome();
    default:
        return false; // the other commands come in the next steps
    }
}
