#ifndef VOICECONTROL_COMMAND_PARSER_H
#define VOICECONTROL_COMMAND_PARSER_H

#include <string>
#include <vector>
#include "command/Command.h"

using namespace std;

// One rule: if the spoken text contains any of these phrases,
// it becomes this command. "arg" is extra data for the action,
// for example which program to start.
struct CommandRule {
    vector<string> phrases;
    int type;
    string confirmation;
    string arg;
};

class CommandParser {
public:
    CommandParser();
    Command parse(const string& rawText);

    // All trigger phrases in one string, to hint the speech recognizer.
    string vocabulary() const;

private:
    vector<CommandRule> rules;
};

#endif
