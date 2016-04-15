#include<unordered_set>

#include"game.hpp"
#include"parser.hpp"

game::game(
std::string&& src_name,
board&& src_brd,
std::vector<piece>&& src_piece,
uint limit,
goals&& u_goals,
goals&& l_goals):
name(std::move(src_name)),
brd(std::move(src_brd)),
piece_moves(std::move(src_piece)),
turns_limit(limit),
uppercase_player_goals(std::move(u_goals)),
lowercase_player_goals(std::move(l_goals)){
}

game::game(const game& src):
name(src.name),
brd(src.brd),
piece_moves(src.piece_moves),
turns_limit(src.turns_limit),
uppercase_player_goals(src.uppercase_player_goals),
lowercase_player_goals(src.lowercase_player_goals){
}

game& game::operator=(const game& src){
    if(this == &src)
        return *this;
    name = src.name;
    brd = src.brd;
    piece_moves = src.piece_moves;
    turns_limit = src.turns_limit;
    uppercase_player_goals = src.uppercase_player_goals;
    lowercase_player_goals = src.lowercase_player_goals;
    return *this;
}

game::game(game&& src):
name(std::move(src.name)),
brd(std::move(src.brd)),
piece_moves(std::move(src.piece_moves)),
turns_limit(src.turns_limit),
uppercase_player_goals(std::move(src.uppercase_player_goals)),
lowercase_player_goals(std::move(src.lowercase_player_goals)){
}

game& game::operator=(game&& src){
    if(this == &src)
        return *this;
    name = std::move(src.name);
    brd = std::move(src.brd);
    piece_moves = std::move(src.piece_moves);
    turns_limit = src.turns_limit;
    uppercase_player_goals = std::move(src.uppercase_player_goals);
    lowercase_player_goals = std::move(src.lowercase_player_goals);
    return *this;
}

game::~game(void){
}

game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error){
    std::ifstream input_file(file_name);
    if(!input_file.good())
        throw parse_error("Couldn't open input file");
    parser p(&input_file);
    std::string game_name;
    while(!p.expect_whitespace())
        game_name.push_back(p.expect_plain_char());
    if(game_name == "")
        throw parse_error(p.get_line_number(), p.get_char_in_line_number(), "Input should begin with game name");
    std::pair<board, std::unordered_set<char>> board_result = parse_board(p, warnings_list);
    std::vector<piece> piece_moves = parse_pieces(p, warnings_list, board_result.second);
    std::pair<uint, std::pair<goals, goals>> g = parse_goals(p, warnings_list, board_result.second, board_result.first);
    p.expect_whitespace();
    if(!p.expect_end_of_file())
        throw parse_error(p.get_line_number(), p.get_char_in_line_number(), "Unexpected characters at the end of input");
    return game(
        std::move(game_name),
        std::move(board_result.first),
        std::move(piece_moves),
        g.first,
        std::move(g.second.first),
        std::move(g.second.second));
}

void game::write_steps_logic(std::ofstream& out)const{
    for(uint i=1;i<=turns_limit;++i)
        out<<"(succ "<<i<<' '<<i+1<<")\n";
}

void game::write_arithmetic(std::ofstream& out, uint less_than_number)const{
    for(uint i=0;i<less_than_number;++i)
        out<<"(arithmeticSucc "<<i<<' '<<i+1<<")\n";
    out<<"\n(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?x ?y))\n";
    out<<"(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?y ?x))\n";
    out<<"(<= (sum ?x ?y ?z)\n\t(arithmeticSucc ?x ?succx)\n\t(arithmeticSucc ?prevy ?y)\n\t(sum ?succx ?prevy ?z))\n";
    out<<"\n(<= (sub ?x ?y ?z)\n\t(sum ?z ?y ?x))\n";
}

void game::write_base(std::ofstream& out)const{
    out<<"(<= (base (control ?p))\n\t(role ?p))\n\n";
    out<<"(base (step 1))\n";
    out<<"(<= (base (step ?next))\n\t(succ ?s ?next))\n\n";
    out<<"(<= (base (cell ?x ?y ?piece))\n\t(file ?x)\n\t(rank ?y)\n\t(pieceType ?piece))\n\n";
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal()){
        out<<"(<= (base (captureCounter ?piece 0))\n\t(captureCounterStep ?piece ?n ?succn))\n";
        out<<"(<= (base (captureCounter ?piece ?n))\n\t(captureCounterStep ?piece ?prevn ?n))\n";
    }
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(base (uppercaseBrokeThrough T))\n(base (uppercaseBrokeThrough F))\n\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(base (lowercaseBrokeThrough T))\n(base (lowercaseBrokeThrough F))\n\n";
}

void game::write_input(std::ofstream& out)const{
    out<<"(<= (input ?player noop)\n\t(role ?player))\n";
    out<<"(<= (input uppercasePlayer (move ?piece ?x1 ?y1 ?x2 ?y2))\n\t(uppercasePieceType ?piece)\n\t(file ?x1)\n\t(rank ?y1)\n\t(file ?x2)\n\t(rank ?y2))\n";
    out<<"(<= (input lowercasePlayer (move ?piece ?x1 ?y1 ?x2 ?y2))\n\t(lowercasePieceType ?piece)\n\t(file ?x1)\n\t(rank ?y1)\n\t(file ?x2)\n\t(rank ?y2))\n";
}

void game::write_initial_state(std::ofstream& out)const{
    brd.write_initial_state(out);
    out<<"\n(init (step 1))\n";
    out<<"\n(init (control uppercasePlayer))\n\n";
    if(lowercase_player_goals.has_any_capture_goal()){
        lowercase_player_goals.write_initial_capture_states(out, false);
        out<<'\n';
    }
    if(uppercase_player_goals.has_any_capture_goal())
        uppercase_player_goals.write_initial_capture_states(out, true);
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(init (uppercaseBrokeThrough F))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(init (lowercaseBrokeThrough F))\n";
}

void game::write_pieces_definition(std::ofstream& out)const{
    out<<"(<= (pieceType ?piece)\n\t(uppercasePieceType ?piece))\n";
    out<<"(<= (pieceType ?piece)\n\t(lowercasePieceType ?piece))\n\n";
    for(const auto& el: piece_moves)
        out<<"(uppercasePieceType "<<el.get_symbol()<<")\n";
    out<<'\n';
    for(const auto& el: piece_moves)
        out<<"(lowercasePieceType "<<char(tolower(el.get_symbol()))<<")\n";
}

void game::write_next_state_logic(std::ofstream& out)const{
    out<<"(<= (next (control uppercasePlayer))\n\t(true (control lowercasePlayer)))\n";
    out<<"(<= (next (control lowercasePlayer))\n\t(true (control uppercasePlayer)))\n\n";
    out<<"(<= (next (step ?n))\n\t(true (step ?prevn))\n\t(succ ?prevn ?n))\n\n";
    out<<"(<= (next (cell ?x ?y ?player ?piece))\n\t(true (cell ?x ?y ?player ?piece))\n\t(not (affected ?x ?y)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?piece ?x ?y ?x2 ?y2)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?piece ?x1 ?y1 ?x ?y)))\n";
    out<<"(<= (next (cell ?x ?y ?player ?piece))\n\t(does ?player (move ?piece ?x1 ?y1 ?x ?y)))\n\n";
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (next (captureCounter ?piece ?n))\n\t(does ?player (move ?movingPiece ?x1 ?y1 ?x ?y))\n\t(true (cell ?x ?y ?piece))\n\t(succ ?prevn ?n)\n\t(captureCounter ?piece ?prevn))\n";
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (next (captureCounter ?piece ?n))\n\t(true (captureCounter ?piece ?n)))\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= (next (uppercaseBrokeThrough ?b))\n\t(true (uppercaseBrokeThrough ?b)))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= (next (lowercaseBrokeThrough ?b))\n\t(true (lowercaseBrokeThrough ?b)))\n";
    uppercase_player_goals.write_breakthrough_detection(out, true);
    lowercase_player_goals.write_breakthrough_detection(out, false);
}

void game::write_terminal_state(std::ofstream& out)const{
    out<<"(<= terminal\n\t(step "<<turns_limit+1<<"))\n\n";
    if(lowercase_player_goals.has_any_capture_goal() || uppercase_player_goals.has_any_capture_goal())
        out<<"(<= terminal\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n)))\n";
    out<<"\n(<= upperHasNoPieces\n\t(uppercasePieceType ?piece)\n\t(not (true (cell ?x ?y ?piece))))\n";
    out<<"(<= terminal\n\tupperHasNoPieces)\n";
    out<<"\n(<= lowerHasNoPieces\n\t(lowercasePieceType ?piece)\n\t(not (true (cell ?x ?y ?piece))))\n";
    out<<"(<= terminal\n\tlowerHasNoPieces)\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= terminal\n\t(true (uppercaseBrokeThrough T)))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= terminal\n\t(true (lowercaseBrokeThrough T)))\n";
}

void game::write_goals(std::ofstream& out)const{
    out<<"(<= (goal uppercasePlayer 100)\n\tlowerHasNoPieces)\n";
    out<<"(<= (goal lowercasePlayer 100)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal uppercasePlayer 0)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal lowercasePlayer 0)\n\tlowerHasNoPieces)\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal uppercasePlayer 100)\n\t(true (upperBrokeThrough T)))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal lowercasePlayer 100)\n\t(true (lowerBrokeThrough T)))\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal uppercasePlayer 0)\n\t(true (lowerBrokeThrough T)))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal lowercasePlayer 0)\n\t(true (upperBrokeThrough T)))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal uppercase 100)\n\t(lowercasePieceType ?piece)\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n)))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal lowercase 100)\n\t(uppercasePieceType ?piece)\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n)))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal uppercase 0)\n\t(uppercasePieceType ?piece)\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n)))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal lowercase 0)\n\t(lowercasePieceType ?piece)\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n)))\n";
}

std::string separator = ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";

void game::write_as_gdl(const std::string& output_file_name){
    std::ofstream out(output_file_name);
    out<<separator;
    out<<";; "<<name<<'\n';
    out<<separator<<'\n';
    out<<separator;
    out<<";; Roles\n";
    out<<separator<<'\n';
    out<<"(role uppercasePlayer)\n";
    out<<"(role lowercasePlayer)\n";
    out<<'\n'<<separator;
    out<<";; Base\n";
    out<<separator<<'\n';
    write_base(out);
    out<<'\n'<<separator;
    out<<";; Input\n";
    out<<separator<<'\n';
    write_input(out);
    out<<'\n'<<separator;
    out<<";; Initial state\n";
    out<<separator<<'\n';
    write_initial_state(out);
    out<<'\n'<<separator;
    out<<";; Artithmetic\n";
    out<<separator<<'\n';
    write_arithmetic(out,std::max(brd.get_height(),brd.get_width()));
    out<<'\n'<<separator;
    out<<";; Board definition\n";
    out<<separator<<'\n';
    brd.write_files_logic(out);
    out<<'\n';
    brd.write_ranks_logic(out);
    out<<'\n'<<separator;
    out<<";; Pieces definition\n";
    out<<separator<<'\n';
    write_pieces_definition(out);
    out<<'\n'<<separator;
    out<<";; Next state logic\n";
    out<<separator<<'\n';
    write_next_state_logic(out);
    out<<'\n'<<separator;
    out<<";; Terminal states\n";
    out<<separator<<'\n';
    write_terminal_state(out);
    out<<'\n'<<separator;
    out<<";; Goals\n";
    out<<separator<<'\n';
    write_goals(out);
    if(lowercase_player_goals.has_any_capture_goal() || uppercase_player_goals.has_any_capture_goal()){
        out<<'\n'<<separator;
        out<<";; Captures counter\n";
        out<<separator<<'\n';
        lowercase_player_goals.write_piece_capture_counter(out,false);
        out<<'\n';
        uppercase_player_goals.write_piece_capture_counter(out,true);
        out<<"\n(<= (captureCounterStep ?piece ?n ?succn)\n\t(succ ?n ?succn)\n\t(captureCounterStep ?piece ?prevn ?n)\n\t(not (captureToWin ?piece ?n)))\n";
    }
    out<<'\n'<<separator;
    out<<";; Steps counter\n";
    out<<separator<<'\n';
    write_steps_logic(out);
}
