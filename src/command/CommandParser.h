#ifndef VOICECONTROL_COMMAND_PARSER_H
#define VOICECONTROL_COMMAND_PARSER_H

#include <string>
#include <vector>
#include "command/Command.h"

using namespace std;

// One rule: if the spoken text contains any of these phrases,
// it becomes this command.
struct CommandRule {
    vector<string> phrases;
    int type;
    string confirmation;
};

class CommandParser {
public:
    CommandParser();
    Command parse(const string& rawText);

private:
    vector<CommandRule> rules;
};

#endif
