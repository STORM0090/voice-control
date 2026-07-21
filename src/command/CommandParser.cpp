#include "command/CommandParser.h"

#include <cctype>

using namespace std;

// " Open Chrome. " -> " open chrome ". The outer spaces make the search
// match whole words only, so "track" does not hit "soundtrack".
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
    // Last field goes to ShellExecute: an exe name or a URI like "spotify:".
    // Matching the app name without "open" is deliberate - whisper usually
    // gets the name right but mangles "open" ("and calculator").
    rules.push_back({{"chrome", "browser", "open home"},
                     CMD_OPEN_APP, "Opening Chrome", "chrome.exe"});
    rules.push_back({{"spotify", "open music"},
                     CMD_OPEN_APP, "Opening Spotify", "spotify:"});
    rules.push_back({{"notepad", "notes"},
                     CMD_OPEN_APP, "Opening Notepad", "notepad.exe"});
    rules.push_back({{"calculator", "calc"},
                     CMD_OPEN_APP, "Opening Calculator", "calc.exe"});
    // "plus"/"minus" are short, whisper hears them far better than "louder"
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
    rules.push_back({{"show desktop", "minimize"},
                     CMD_SHOW_DESKTOP, "Showing desktop"});
    // two words on purpose: locking on a stray "lock" would be annoying
    rules.push_back({{"lock screen", "lock the computer"},
                     CMD_LOCK_SCREEN, "Locking the screen"});
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

    return Command();
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
