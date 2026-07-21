#ifndef VOICECONTROL_ACTION_EXECUTOR_H
#define VOICECONTROL_ACTION_EXECUTOR_H

#include "command/Command.h"

using namespace std;

// Carries out a parsed command through the Windows API.
class ActionExecutor {
public:
    bool execute(const Command& cmd); // false if it did not run
};

#endif
