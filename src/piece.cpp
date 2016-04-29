#include"piece.hpp"
#include"gdl_constants.hpp"

piece_parse_error::piece_parse_error(void):
parse_error("Piece parse error"){
}

piece_parse_error::piece_parse_error(uint line, uint character, const std::string& source):
parse_error(line, character, source){
}

piece_parse_error::piece_parse_error(const piece_parse_error& source):
parse_error(source){
}

piece_parse_error& piece_parse_error::operator=(const piece_parse_error& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = source.description;
    return *this;
}

piece_parse_error::piece_parse_error(piece_parse_error&& source):
parse_error(std::move(source)){
}

piece_parse_error& piece_parse_error::operator=(piece_parse_error&& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = std::move(source.description);
    return *this;
}

piece_parse_error::~piece_parse_error(void){
}

const char* piece_parse_error::what(void)const noexcept{
    return ("Piece parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
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

char piece::get_symbol(void)const{
    return symbol;
}

std::vector<piece> parse_pieces(
    parser& p,
    std::vector<warning>& warnings_list,
    const std::unordered_set<char>& declared_pieces)throw(piece_parse_error){
    std::vector<piece> result;
    std::unordered_set<char> parsed_pieces;
    if(!p.expect_string("<--PIECES-->"))
        throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Pieces segment must begin with \'<--PIECES-->\' string");
    p.expect_whitespace();
    char next_char;
    bool ignore;
    while(isupper(next_char = p.expect_plain_char())){
        ignore = false;
        //next_char = p.expect_plain_char();
        //if(!isupper(next_char))
        //    throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected uppercase character (there are still declared but not described pieces)");
        if(parsed_pieces.count(next_char)){
            ignore = true;
            warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "Piece already defined, ignoring this definition"));
        }
        if(!declared_pieces.count(next_char)){
            ignore = true;
            warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "Undeclared piece (piece doesn't appear in the previous board declaration), ignoring this definition"));
        }
        p.expect_whitespace();
        parser_result<move> move_pattern_result = p.expect_move_pattern();
        if(!move_pattern_result)
            throw piece_parse_error(move_pattern_result.info.line_number, move_pattern_result.info.char_number, move_pattern_result.info.human_readable_info.c_str());
        p.expect_whitespace();
        if(p.expect_plain_char()!='&')
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Move pattern must be terminated with \'&\'");
        p.expect_whitespace();
        if(!ignore){
            result.push_back(piece(next_char, std::move(move_pattern_result.result)));
            parsed_pieces.insert(next_char);
        }
    }
    p.go_back();
    if(declared_pieces.size() > parsed_pieces.size())
        warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "There are some pieces without definitions remaining at the end of pieces segment"));
    return result;
}

uint piece::max_number_of_repetitions(uint treat_star_as)const{
    return move_pattern.max_number_of_repetitions(treat_star_as);
}

void piece::write_as_gdl(std::ofstream& out, bool uppercase)const{
    std::vector<std::pair<uint, const move*>> additional_moves;
    std::vector<std::pair<uint, const bracketed_move*>> additional_bracketed_moves;
    additional_moves.push_back(std::make_pair(0,&move_pattern));
    const std::string legal_move_name = std::string("legal")+piece_name(symbol, uppercase)+"Move";
    out<<"(<= (legal "<<player_name(uppercase)<<" (move ?xin ?yin ?xout ?yout))";
    out<<"\n\t(true (control "<<player_name(uppercase)<<"))";
    out<<"\n\t(true (cell ?xin ?yin "<<piece_name(symbol, uppercase)<<"))";
    out<<"\n\t("<<legal_move_name<<"0 ?xin ?yin ?xout ?yout))\n\n";
    std::pair<uint, const move*> move_to_write;
    std::pair<uint, const bracketed_move*> bracketed_move_to_write;
    uint next_free_id = 1;
    while(!additional_moves.empty() || !additional_bracketed_moves.empty()){
        while(!additional_moves.empty()){
            move_to_write = additional_moves.back();
            additional_moves.pop_back();
            move_to_write.second->write_as_gdl(out,additional_moves,additional_bracketed_moves,legal_move_name,move_to_write.first,uppercase,next_free_id);
        }
        while(!additional_bracketed_moves.empty()){
            bracketed_move_to_write = additional_bracketed_moves.back();
            additional_bracketed_moves.pop_back();
            bracketed_move_to_write.second->write_freestanding_predicate(out,additional_moves,legal_move_name,bracketed_move_to_write.first,uppercase,next_free_id);
        }
    }
}
