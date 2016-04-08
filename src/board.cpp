#include"board.hpp"

board_parse_error::board_parse_error(void):
parse_error("Board parse error"){
}

board_parse_error::board_parse_error(uint line, uint character, const char* source):
parse_error(line, character, source){
}

board_parse_error::board_parse_error(const board_parse_error& source):
parse_error(source){
}

board_parse_error& board_parse_error::operator=(const board_parse_error& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = source.description;
    return *this;
}

board_parse_error::board_parse_error(board_parse_error&& source):
parse_error(std::move(source)){
}

board_parse_error& board_parse_error::operator=(board_parse_error&& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = std::move(source.description);
    return *this;
}

board_parse_error::~board_parse_error(void){
}

const char* board_parse_error::what(void)const noexcept{
    return ("Board parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
}

board::board(uint w, uint h):
width(w),
fields(h, std::vector<std::pair<bool, char>>(w, std::make_pair(false,'.'))){
}

board::board(const board& src):
width(src.width),
fields(src.fields){
}

board& board::operator=(const board& src){
    if(this == &src)
        return *this;
    width = src.width;
    fields = src.fields;
    return *this;
}

board::board(board&& src):
width(src.width),
fields(std::move(src.fields)){
}

board& board::operator=(board&& src){
    if(this == &src)
        return *this;
    width = src.width;
    fields = std::move(src.fields);
    return *this;
}

board::~board(void){
}

bool board::inside(uint x, uint y)const{
    return x<get_width() && y<get_height();
}

uint board::get_width(void)const{
    return width;
}
uint board::get_height(void)const{
    return fields.size();
}

std::pair<board, std::unordered_set<char>> parse_board(
    parser& p,
    std::vector<warning>& warnings_list)throw(board_parse_error){
    std::unordered_set<char> pieces_set;
    if(!p.expect_string("<--BOARD-->"))
        throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Board segment must begin with \'<--BOARD-->\' string");
    p.expect_whitespace();
    parser_result<int> width_result = p.expect_int();
    if(!width_result)
        throw board_parse_error(width_result.info.line_number, width_result.info.char_number, width_result.info.human_readable_info.c_str());
    if(width_result.result < 1)
        throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Board width must be positive");
    if(!p.expect_whitespace())
        throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected whitespace after board width declaration");
    parser_result<int> height_result = p.expect_int();
    if(!height_result)
        throw board_parse_error(height_result.info.line_number, height_result.info.char_number, height_result.info.human_readable_info.c_str());
    if(height_result.result < 1)
        throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Board height must be positive");
    p.expect_whitespace();
    board result(width_result.result, height_result.result);
    std::vector<uint> expected_fields_alignment(result.width);
    char next_char;
    uint line_number = p.get_line_number();
    for(uint i=0;i<result.width;++i){
        next_char = p.expect_plain_char();
        if(next_char!='.'){
            if(isupper(next_char))
                result.fields[0][i] = std::make_pair(true, next_char);
            else if(islower(next_char))
                result.fields[0][i] = std::make_pair(false, toupper(next_char));
            else
                throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected \'.\' or letter");
            pieces_set.insert(toupper(next_char));
        }
        if(p.get_line_number()!=line_number)
            warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "This character is in wrong line"));
        expected_fields_alignment[i] = p.get_char_in_line_number();
        p.expect_whitespace();
    }
    for(uint i=1;i<result.fields.size();++i){
        line_number = p.get_line_number();
        for(uint j=0;j<result.width;++j){
            next_char = p.expect_plain_char();
            if(next_char!='.'){
                if(isupper(next_char))
                    result.fields[0][i] = std::make_pair(true, next_char);
                else if(islower(next_char))
                    result.fields[0][i] = std::make_pair(false, toupper(next_char));
                else
                    throw board_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected \'.\' or letter");
                pieces_set.insert(toupper(next_char));
            }
            if(p.get_line_number()!=line_number)
                 warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "This character is in wrong line"));
            if(p.get_char_in_line_number()!=expected_fields_alignment[j])
                 warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "This character is in wrong column"));
            p.expect_whitespace();
        }
    }
    return std::make_pair(std::move(result), std::move(pieces_set));
}
