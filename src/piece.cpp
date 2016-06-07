#include"piece.hpp"
#include"gdl_constants.hpp"

piece_parse_error::piece_parse_error(void):
parse_error("Piece parse error"){
}

piece_parse_error::piece_parse_error(uint line, uint character, const std::string& source):
parse_error(line, character, source){
}

piece_parse_error::~piece_parse_error(void){
}

const char* piece_parse_error::what(void)const noexcept{
    return ("Piece parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
}

std::string piece_parse_error::to_string(void)const{
    return "Piece parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description;
}

piece::piece(char s, move&& pattern):
symbol(s),
move_pattern(std::move(pattern)){
}

char piece::get_symbol(void)const{
    return symbol;
}

std::vector<piece> parse_pieces(
    parser& p,
    std::vector<warning>& warnings_list,
    const std::unordered_set<char>& declared_pieces,
    const board& brd)throw(piece_parse_error){
    std::vector<piece> result;
    std::unordered_set<char> parsed_pieces;
    if(!p.expect_string("<PIECES>"))
        throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Pieces segment must begin with \'<PIECES>\' string");
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
        uint piece_line = p.get_line_number();
        uint piece_char = p.get_char_in_line_number();
        parser_result<move> move_pattern_result = p.expect_move_pattern();
        if(!move_pattern_result)
            throw piece_parse_error(move_pattern_result.info.line_number, move_pattern_result.info.char_number, move_pattern_result.info.human_readable_info.c_str());
        p.expect_whitespace();
        if(p.expect_plain_char()!='&')
            throw piece_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Move pattern must be terminated with \'&\'");
        p.expect_whitespace();
        uint cutting_result = move_pattern_result.result.cut_unnecessary_moves(brd.get_width(),brd.get_height());
        if(cutting_result==2){
            warnings_list.push_back(warning(piece_line,piece_char, "Every move matching this pattern will not fit in the board, ignoring this definition"));
            ignore = true;
        }
        else if(cutting_result==1)
            warnings_list.push_back(warning(piece_line,piece_char, "This pattern contains too big atomic moves"));
        if(!ignore){
            result.push_back(piece(next_char, std::move(move_pattern_result.result)));
            parsed_pieces.insert(next_char);
        }
    }
    p.go_back();
    if(result.empty())
        warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "There is no piece with any legal move"));
    else if(declared_pieces.size() > parsed_pieces.size())
        warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "There are some pieces without definitions remaining at the end of pieces segment"));
    return result;
}

uint piece::max_number_of_repetitions(uint treat_star_as)const{
    return move_pattern.max_number_of_repetitions(treat_star_as);
}

void piece::scan_for_concatenations(reuse_tool& known)const{
    move_pattern.scan_for_concatenations(known);
}

void piece::scan_for_subsums(const piece& second, reuse_tool& known, bool uppercase)const{
    move_pattern.scan_for_subsums(second.move_pattern, known, uppercase);
}

void piece::write_as_gdl(std::ofstream& out, bool uppercase, reuse_tool& known_moves, const options& o)const{
    moves_concatenation move_on_enemy;
    moves_concatenation move_on_empty;
    move_on_enemy.append(bracketed_move(single_move(0,0,on::enemy)));
    move_on_empty.append(bracketed_move(single_move(0,0,on::empty)));
    move empty_move1;
    move empty_move2;
    empty_move1.append(std::move(move_on_empty));
    empty_move2.append(std::move(move_on_enemy));
    if(move_pattern==empty_move1 || move_pattern==empty_move2)
        return;
    std::vector<std::pair<uint, move>> additional_moves;
    std::vector<std::pair<uint, bracketed_move>> additional_bracketed_moves;
    additional_moves.push_back(std::make_pair(0,move_pattern));
    const std::string legal_move_name = std::string("legal")+piece_name(symbol, uppercase)+"Move";
    out<<"(<= (legal "<<player_name(uppercase)<<" (move ?xin ?yin ?xout ?yout))";
    out<<"\n\t(control "<<player_name(uppercase)<<")";
    out<<"\n\t(true (cell ?xin ?yin "<<piece_name(symbol, uppercase)<<"))";
    out<<"\n\t("<<legal_move_name<<"0 ?xin ?yin ?xout ?yout))\n\n";
    uint next_free_id = 1;
    while(!additional_moves.empty() || !additional_bracketed_moves.empty() || known_moves.there_are_new_concatenations()){
        while(!additional_moves.empty()){
            std::pair<uint, move> move_to_write = additional_moves.back();
            additional_moves.pop_back();
            move_to_write.second.write_as_gdl(
                out,
                additional_moves,
                additional_bracketed_moves,
                known_moves,
                legal_move_name,
                move_to_write.first,
                uppercase,
                next_free_id,
                o);
        }
        while(!additional_bracketed_moves.empty()){
            std::pair<uint, bracketed_move> bracketed_move_to_write = additional_bracketed_moves.back();
            additional_bracketed_moves.pop_back();
            bracketed_move_to_write.second.write_freestanding_predicate(
                out,
                additional_moves,
                known_moves,
                legal_move_name,
                bracketed_move_to_write.first,
                uppercase,
                next_free_id,
                o);
        }
        known_moves.write_all_concatenations(
            out,
            additional_moves,
            additional_bracketed_moves,
            uppercase,
            legal_move_name,
            next_free_id,
            o);
    }
}
