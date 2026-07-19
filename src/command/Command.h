#ifndef VOICECONTROL_COMMAND_H
#define VOICECONTROL_COMMAND_H

#include <string>

using namespace std;

enum CommandType {
    CMD_NONE,
    CMD_OPEN_CHROME,
    CMD_VOLUME_UP,
    CMD_VOLUME_DOWN,
    CMD_MUTE_TOGGLE,
    CMD_NEXT_TRACK,
    CMD_PREV_TRACK,
    CMD_PLAY_PAUSE,
};

struct Command {
    int type = CMD_NONE;
    string arg;          // extra data for the action, e.g. a path
    string confirmation; // what the assistant says back after executing
};

#endif
