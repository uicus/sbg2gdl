#ifndef GDL_CONSTANTS
#define GDL_CONSTANTS

#include<string>

const std::string separator = ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";

std::string player_name(bool uppercase);
std::string piece_name(char symbol, bool uppercase);

inline std::string player_name(bool uppercase){
    return uppercase ? "uppercasePlayer" : "lowercasePlayer";
}

inline std::string piece_name(char symbol, bool uppercase){
    return uppercase ? std::string("u")+char(toupper(symbol)) : std::string("l")+char(tolower(symbol));
}

#endif
