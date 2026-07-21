#ifndef VOICECONTROL_COMMAND_PARSER_H
#define VOICECONTROL_COMMAND_PARSER_H

#include <string>
#include <vector>
#include "command/Command.h"

using namespace std;

// If the text contains any of these phrases, it becomes this command.
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

    // all trigger phrases in one string, used to hint whisper
    string vocabulary() const;

private:
    vector<CommandRule> rules;
};

#endif
