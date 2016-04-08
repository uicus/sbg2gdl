#include"parser_events.hpp"

/*parse_error::parse_error(void):
std::exception(),
description("Unspecified parser error"){
}*/

parse_error::parse_error(const char* source):
std::exception(),
line_number(0),
char_number(0),
description(source){
}

parse_error::parse_error(uint line, uint character, const char* source):
std::exception(),
line_number(line),
char_number(character),
description(source){
}

parse_error::parse_error(const parse_error& source):
std::exception(),
line_number(source.line_number),
char_number(source.char_number),
description(source.description){
}

parse_error& parse_error::operator=(const parse_error& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = source.description;
    return *this;
}

parse_error::parse_error(parse_error&& source):
std::exception(),
line_number(source.line_number),
char_number(source.char_number),
description(std::move(source.description)){
}

parse_error& parse_error::operator=(parse_error&& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = std::move(source.description);
    return *this;
}

parse_error::~parse_error(void){
}

const char* parse_error::what(void)const noexcept{
    return ("General parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
}

warning::warning(uint line, uint column, const char* src):
line_number(line),
char_number(column),
description(src){
}

warning::warning(const warning& src):
line_number(src.line_number),
char_number(src.char_number),
description(src.description){
}

warning& warning::operator=(const warning& src){
    if(this == &src)
        return *this;
    line_number = src.line_number;
    char_number = src.char_number;
    description = src.description;
    return *this;
}
warning::warning(warning&& src):
line_number(src.line_number),
char_number(src.char_number),
description(std::move(src.description)){
}
warning& warning::operator=(warning&& src){
    if(this == &src)
        return *this;
    line_number = src.line_number;
    char_number = src.char_number;
    description = std::move(src.description);
    return *this;
}

warning::~warning(void){
}

std::string warning::to_string(bool as_error)const{
    return (as_error ? "Error (-Werror), line " : "Warning, line ")+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description;
}
