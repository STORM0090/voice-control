#ifndef VOICECONTROL_COMMAND_H
#define VOICECONTROL_COMMAND_H

#include <string>

using namespace std;

enum CommandType {
    CMD_NONE,
    CMD_OPEN_APP,
    CMD_VOLUME_UP,
    CMD_VOLUME_DOWN,
    CMD_MUTE_TOGGLE,
    CMD_NEXT_TRACK,
    CMD_PREV_TRACK,
    CMD_PLAY_PAUSE,
};

struct Command {
    int type = CMD_NONE;
    string arg;          // what to open, for CMD_OPEN_APP
    string confirmation; // spoken back after the command runs
};

#endif
