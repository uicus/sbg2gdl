#include"parser_events.hpp"

parse_error::parse_error(const std::string& source):
std::exception(),
line_number(0),
char_number(0),
description(source){
}

parse_error::parse_error(uint line, uint character, const std::string& source):
std::exception(),
line_number(line),
char_number(character),
description(source){
}

parse_error::~parse_error(void){
}

const char* parse_error::what(void)const noexcept{
    return ("General parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
}

std::string parse_error::to_string(void)const{
    return "General parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description;
}

warning::warning(uint line, uint column, const std::string& src):
line_number(line),
char_number(column),
description(src){
}

std::string warning::to_string(bool as_error)const{
    return (as_error ? "Error (-Werror), line " : "Warning, line ")+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description;
}
