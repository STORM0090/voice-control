#include "command/CommandParser.h"

#include <cctype>

using namespace std;

// whisper output looks like " Open Chrome. " - capitals, punctuation, spaces.
// Turn it into " open chrome " so phrases are easy to find. The surrounding
// spaces let us match whole words only ("track" does not match "soundtrack").
static string normalize(const string& raw) {
    string out = " ";
    for (const auto& ch : raw) {
        unsigned char u = (unsigned char)ch;
        if (isalnum(u)) {
            out += (char)tolower(u);
        } else if (out.back() != ' ') {
            out += ' ';
        }
    }
    if (out.back() != ' ') {
        out += ' ';
    }
    return out;
}

CommandParser::CommandParser() {
    // Adding a command = one line here (plus a case in the executor).
    // "open home" is here on purpose: whisper often hears "chrome" as "home".
    rules.push_back({{"open chrome", "launch chrome", "start chrome", "open home"},
                     CMD_OPEN_CHROME, "Opening Chrome"});
    // "plus"/"minus" are short and easy to pronounce, so whisper hears them
    // reliably - unlike "louder"/"quieter", which it often mangles.
    rules.push_back({{"volume up", "louder", "turn it up", "plus"},
                     CMD_VOLUME_UP, "Volume up"});
    rules.push_back({{"volume down", "quieter", "turn it down", "minus"},
                     CMD_VOLUME_DOWN, "Volume down"});
    rules.push_back({{"mute", "unmute", "sound off", "sound on"},
                     CMD_MUTE_TOGGLE, "Mute toggled"});
    rules.push_back({{"next track", "next song", "skip"},
                     CMD_NEXT_TRACK, "Next track"});
    rules.push_back({{"previous track", "previous song", "go back"},
                     CMD_PREV_TRACK, "Previous track"});
    rules.push_back({{"play", "pause", "resume", "stop music"},
                     CMD_PLAY_PAUSE, "Playback toggled"});
}

Command CommandParser::parse(const string& rawText) {
    string text = normalize(rawText);

    for (const auto& rule : rules) {
        for (const auto& phrase : rule.phrases) {
            if (text.find(" " + phrase + " ") != string::npos) {
                Command cmd;
                cmd.type = rule.type;
                cmd.confirmation = rule.confirmation;
                return cmd;
            }
        }
    }

    return Command(); // type stays CMD_NONE
}
