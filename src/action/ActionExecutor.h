#ifndef VOICECONTROL_ACTION_EXECUTOR_H
#define VOICECONTROL_ACTION_EXECUTOR_H

#include "command/Command.h"

using namespace std;

// Carries out a parsed command using the Windows API.
class ActionExecutor {
public:
    // Returns false if the command is not implemented or failed.
    bool execute(const Command& cmd);
};

#endif
