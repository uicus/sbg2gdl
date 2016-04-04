#include"piece.hpp"

piece_parse_error::piece_parse_error(void):
std::exception(),
description("Wrong argument error"){
}

piece_parse_error::piece_parse_error(uint line, uint character, const char* source):
std::exception(),
description("Line "+std::to_string(line)+", character "+std::to_string(character)+": "+source){
}

piece_parse_error::piece_parse_error(const piece_parse_error& source):
std::exception(),
description(source.description){
}

piece_parse_error& piece_parse_error::operator=(const piece_parse_error& source){
    description = source.description;
    return *this;
}

piece_parse_error::piece_parse_error(piece_parse_error&& source):
std::exception(),
description(std::move(source.description)){
}

piece_parse_error& piece_parse_error::operator=(piece_parse_error&& source){
    if(this == &source)
        return *this;
    description = std::move(source.description);
    return *this;
}

piece_parse_error::~piece_parse_error(void){
}

const char* piece_parse_error::what(void)const noexcept{
    return description.c_str();
}

piece::piece(char s, move&& pattern):
symbol(s),
move_pattern(std::move(pattern)){
}

piece::piece(const piece& src):
symbol(src.symbol),
move_pattern(src.move_pattern){
}

piece& piece::operator=(const piece& src){
    if(this == &src)
        return *this;
    symbol = src.symbol;
    move_pattern = src.move_pattern;
    return *this;
}

piece::piece(piece&& src):
symbol(src.symbol),
move_pattern(std::move(src.move_pattern)){
}

piece& piece::operator=(piece&& src){
    if(this == &src)
        return *this;
    symbol = src.symbol;
    move_pattern = std::move(src.move_pattern);
    return *this;
}

piece::~piece(void){
}

std::vector<piece> parse_pieces(parser& p, const std::unordered_set<char>& declared_pieces)throw(piece_parse_error){
    std::vector<piece> result;
    std::unordered_set<char> parsed_pieces;
    if(!p.expect_string("<--PIECES-->"))
        throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Pieces declaration must begin with \'<--PIECES-->\' string");
    p.expect_whitespace();
    char next_char;
    while(declared_pieces.size() > parsed_pieces.size()){
        next_char = p.expect_plain_char();
        if(!isupper(next_char))
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected uppercase character (there are still declared but not described pieces)");
        if(parsed_pieces.count(next_char))
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Cannot describe this piece twice");
        if(!declared_pieces.count(next_char))
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Undeclared piece (piece doesn't appear in the previous board declaration)");
        p.expect_whitespace();
        parser_result<move> move_pattern_result = p.expect_move_pattern();
        if(!move_pattern_result)
            throw piece_parse_error(move_pattern_result.info.line_number, move_pattern_result.info.char_number, move_pattern_result.info.human_readable_info.c_str());
        p.expect_whitespace();
        if(p.expect_plain_char()!='&')
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Move pattern must be terminated with \'&\'");
        p.expect_whitespace();
        result.push_back(piece(next_char, std::move(move_pattern_result.result)));
        parsed_pieces.insert(next_char);
    }
    return result;
}
