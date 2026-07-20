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
    // Adding an app is one line here. The last field is passed to
    // ShellExecute, so it can be an exe name or a URI like "spotify:".
    // Matching on the app name alone, without "open", is on purpose:
    // whisper often gets the name right but turns "open" into something
    // else ("and calculator", "loves potify").
    // "open home" is a fallback: whisper often hears "chrome" as "home".
    rules.push_back({{"chrome", "browser", "open home"},
                     CMD_OPEN_APP, "Opening Chrome", "chrome.exe"});
    rules.push_back({{"spotify", "open music"},
                     CMD_OPEN_APP, "Opening Spotify", "spotify:"});
    rules.push_back({{"notepad", "notes"},
                     CMD_OPEN_APP, "Opening Notepad", "notepad.exe"});
    rules.push_back({{"calculator", "calc"},
                     CMD_OPEN_APP, "Opening Calculator", "calc.exe"});
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
                cmd.arg = rule.arg;
                return cmd;
            }
        }
    }

    return Command(); // type stays CMD_NONE
}

string CommandParser::vocabulary() const {
    string words;
    for (const auto& rule : rules) {
        for (const auto& phrase : rule.phrases) {
            if (!words.empty()) {
                words += ", ";
            }
            words += phrase;
        }
    }
    return words;
}
