#include"goals.hpp"

goals_parse_error::goals_parse_error(void):
parse_error("Goals parse error"){
}

goals_parse_error::goals_parse_error(uint line, uint character, const char* source):
parse_error(line, character, source){
}

goals_parse_error::goals_parse_error(const goals_parse_error& source):
parse_error(source){
}

goals_parse_error& goals_parse_error::operator=(const goals_parse_error& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = source.description;
    return *this;
}

goals_parse_error::goals_parse_error(goals_parse_error&& source):
parse_error(std::move(source)){
}

goals_parse_error& goals_parse_error::operator=(goals_parse_error&& source){
    if(this == &source)
        return *this;
    line_number = source.line_number;
    char_number = source.char_number;
    description = std::move(source.description);
    return *this;
}

goals_parse_error::~goals_parse_error(void){
}

const char* goals_parse_error::what(void)const noexcept{
    return ("Goals parse error, line "+std::to_string(line_number)+", character "+std::to_string(char_number)+": "+description).c_str();
}

goals::goals(void):
piece_placement(),
piece_capture(){
}

goals::goals(const goals& src):
piece_placement(src.piece_placement),
piece_capture(src.piece_capture){
}

goals& goals::operator=(const goals& src){
    if(this == &src)
        return *this;
    piece_placement = src.piece_placement;
    piece_capture = src.piece_capture;
    return *this;
}

goals::goals(goals&& src):
piece_placement(std::move(src.piece_placement)),
piece_capture(std::move(src.piece_capture)){
}

goals& goals::operator=(goals&& src){
    if(this == &src)
        return *this;
    piece_placement = std::move(src.piece_placement);
    piece_capture = std::move(src.piece_capture);
    return *this;
}

goals::~goals(void){
}

void goals::add_piece_placement_goal(char symbol, uint x, uint y){
    if(!piece_placement.count(symbol))
        piece_placement[symbol] = std::vector<std::pair<uint, uint>>();
    piece_placement[symbol].push_back(std::make_pair(x,y));
}

void goals::add_piece_capture_goal(char symbol, uint number_of_pieces){
    piece_capture[symbol] = number_of_pieces;
}

std::pair<uint, std::pair<goals, goals>> parse_goals(
    parser& p,
    std::vector<warning>& warnings_list,
    const std::unordered_set<char>& declared_pieces,
    const board& declared_board)throw(goals_parse_error){
    uint turns_limit;
    goals uppercase_player_goals;
    goals lowercase_player_goals;
    if(!p.expect_string("<--GOALS-->"))
        throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Goals segment must begin with \'<--GOALS-->\' string");
    p.expect_whitespace();
    parser_result<int> turns_limit_result = p.expect_int();
    if(!turns_limit_result)
        throw goals_parse_error(turns_limit_result.info.line_number, turns_limit_result.info.char_number, turns_limit_result.info.human_readable_info.c_str());
    if(turns_limit_result.result < 1)
        throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Turns limit must be positive");
    turns_limit = turns_limit_result.result;
    p.expect_whitespace();
    if(p.expect_plain_char()!='&')
        throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Turns limit must be terminated with \'&\'");
    p.expect_whitespace();
    char next_char = p.expect_plain_char();
    bool ignore;
    while(next_char == '@' || next_char == '#'){
        ignore = false;
        if(next_char == '@'){
            next_char = p.expect_plain_char();
            bool uppercase_player;
            if(isupper(next_char))
                uppercase_player = true;
            else if(islower(next_char)){
                uppercase_player = false;
                next_char = toupper(next_char);
            }
            else
                throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected letter after \'@\'");
            if(!declared_pieces.count(next_char)){
                ignore = true;
                warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "Undeclared piece (piece doesn't appear in the previous board declaration)"));
            }
            char piece_symbol = next_char;
            do{
                p.expect_whitespace();
                parser_result<int> x_result = p.expect_int();
                if(!x_result)
                    throw goals_parse_error(x_result.info.line_number, x_result.info.char_number, x_result.info.human_readable_info.c_str());
                if(x_result.result < 0)
                    throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "x coordinate must be non-negative");
                if(!p.expect_whitespace())
                    throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected whitespace after x coordinate");
                parser_result<int> y_result = p.expect_int();
                if(!y_result)
                    throw goals_parse_error(y_result.info.line_number, y_result.info.char_number, y_result.info.human_readable_info.c_str());
                if(y_result.result < 0)
                    throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "y coordinate must be non-negative");
                if(!declared_board.inside(x_result.result, y_result.result)){
                    ignore = true;
                    warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "Given point is outside the board"));
                }
                if(!ignore){
                    if(uppercase_player)
                        uppercase_player_goals.add_piece_placement_goal(piece_symbol, x_result.result, y_result.result);
                    else
                        lowercase_player_goals.add_piece_placement_goal(piece_symbol, x_result.result, y_result.result);
                }
                p.expect_whitespace();
                next_char = p.expect_plain_char();
                if(next_char != ',' && next_char != '&')
                    throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected \',\' or \'&\'");
            }while(next_char != '&');
        }
        else{
            next_char = p.expect_plain_char();
            bool uppercase_player;
            if(isupper(next_char))
                uppercase_player = false;
            else if(islower(next_char)){
                uppercase_player = true;
                next_char = toupper(next_char);
            }
            else
                throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Expected letter after \'#\'");
            if(!declared_pieces.count(next_char)){
                ignore = true;
                warnings_list.push_back(warning(p.get_line_number(), p.get_char_in_line_number(), "Undeclared piece (piece doesn't appear in the previous board declaration)"));
            }
            p.expect_whitespace();
            parser_result<int> number_of_pieces_result = p.expect_int();
            if(!number_of_pieces_result)
                throw goals_parse_error(number_of_pieces_result.info.line_number, number_of_pieces_result.info.char_number, number_of_pieces_result.info.human_readable_info.c_str());
            if(number_of_pieces_result.result < 1)
                throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Number of piece captures must be positive");
            if(!ignore){
                if(uppercase_player)
                    uppercase_player_goals.add_piece_capture_goal(next_char, number_of_pieces_result.result);
                else
                    lowercase_player_goals.add_piece_capture_goal(next_char, number_of_pieces_result.result);
            }
            p.expect_whitespace();
            if(p.expect_plain_char()!='&')
                throw goals_parse_error(p.get_line_number(), p.get_char_in_line_number(), "Capture goal must be terminated with \'&\'");
        }
        p.expect_whitespace();
        next_char = p.expect_plain_char();
    }
    return std::make_pair(turns_limit, std::make_pair(std::move(uppercase_player_goals), std::move(lowercase_player_goals)));
}

bool goals::has_any_capture_goal(void)const{
    return piece_capture.size() > 0;
}

void goals::write_piece_capture_counter(std::ofstream& out, bool capturing_lower_pieces)const{
    for(const auto& el: piece_capture)
        out<<"(captureCounterStep "<<(capturing_lower_pieces ? char(tolower(el.first)) : el.first)<<" 0 1)\n";
    out<<'\n';
    for(const auto& el: piece_capture)
        out<<"(captureToWin "<<(capturing_lower_pieces ? char(tolower(el.first)) : el.first)<<' '<<el.second<<")\n";
}

void goals::write_initial_capture_states(std::ofstream& out, bool capturing_lower_pieces)const{
    for(const auto& el: piece_capture)
        out<<"(init (captureCounter "<<(capturing_lower_pieces ? char(tolower(el.first)) : el.first)<<" 0))\n";
}

bool goals::has_any_breakthrough_goal(void)const{
    return piece_placement.size() > 0;
}

void goals::write_breakthrough_detection(std::ofstream& out, bool uppercase)const{
    for(const auto& el: piece_placement){
        out<<'\n';
        std::string correct_case = (uppercase ? "uppercase" : "lowercase");
        for(const auto& coords: el.second){
            out<<"(<= (next ("<<correct_case<<"BrokeThrough T))";
            out<<"\n\t(does "<<correct_case<<"Player (move "<<(uppercase ? el.first : char(tolower(el.first)))<<" ?x ?y "<<coords.first<<' '<<coords.second<<")))\n";
        }
    }
}
