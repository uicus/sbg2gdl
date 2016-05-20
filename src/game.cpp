#include<unordered_set>

#include"game.hpp"
#include"parser.hpp"
#include"gdl_constants.hpp"

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
    out<<digit<<'\n';
    out<<succ("stepSucc",turns_limit+1)<<'\n';
}

void game::write_arithmetic(std::ofstream& out, uint less_than_number)const{
    for(uint i=0;i<less_than_number;++i)
        out<<"(arithmeticSucc "<<i<<' '<<i+1<<")\n";
    out<<"\n(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?x ?y))\n";
    out<<"(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?y ?x))\n";
    out<<"(<= (sum ?x ?y ?z)\n\t(arithmeticSucc ?x ?succx)\n\t(arithmeticSucc ?prevy ?y)\n\t(sum ?succx ?prevy ?z))\n";
    out<<"\n(<= (sub ?x ?y ?z)\n\t(sum ?z ?y ?x))\n";
    out<<"\n(<= (equal ?x ?x)\n\t(sum ?x 0 ?x))\n"; // just to bound x
    out<<"\n(<= (distinctCells ?x1 ?y1 ?x2 ?y2)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y1)\n\t(rank ?y2)\n\t(distinct ?x1 ?x2))\n";
    out<<"(<= (distinctCells ?x1 ?y1 ?x2 ?y2)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y1)\n\t(rank ?y2)\n\t(distinct ?y1 ?y2))\n";
}

void game::write_base(std::ofstream& out)const{
    out<<"(<= (base (control ?p))\n\t(role ?p))\n\n";
    out<<"(base (step "<<number(1,turns_limit+1)<<"))\n";
    out<<"(<= (base (step ?next))\n\t(stepSucc ?s ?next))\n\n";
    out<<"(<= (base (cell ?x ?y ?piece))\n\t(file ?x)\n\t(rank ?y)\n\t(pieceType ?piece))\n";
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal()){
        out<<"\n(<= (base (captureCounter ?piece 0))\n\t(captureCounterStep ?piece ?n ?succn))\n";
        out<<"(<= (base (captureCounter ?piece ?n))\n\t(captureCounterStep ?piece ?prevn ?n))\n";
    }
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(base uppercaseBrokeThrough)\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(base lowercaseBrokeThrough)\n";
}

void game::write_input(std::ofstream& out)const{
    out<<"(<= (input ?player noop)\n\t(role ?player))\n";
    out<<"(<= (input ?player (move ?x1 ?y1 ?x2 ?y2))\n\t(file ?x1)\n\t(rank ?y1)\n\t(file ?x2)\n\t(rank ?y2)\n\t(role ?player))\n";
}

void game::write_initial_state(std::ofstream& out)const{
    brd.write_initial_state(out);
    out<<"\n(init (step "<<number(1, turns_limit+1)<<"))\n";
    out<<"\n(init (control "<<player_name(true)<<"))\n\n";
    if(lowercase_player_goals.has_any_capture_goal()){
        lowercase_player_goals.write_initial_capture_states(out, false, turns_limit);
        out<<'\n';
    }
    if(uppercase_player_goals.has_any_capture_goal())
        uppercase_player_goals.write_initial_capture_states(out, true, turns_limit);
}

void game::write_pieces_definition(std::ofstream& out, const options& o)const{
    out<<"(<= (pieceType ?piece)\n\t(uppercasePieceType ?piece))\n";
    out<<"(<= (pieceType ?piece)\n\t(lowercasePieceType ?piece))\n\n";
    for(const auto& el: piece_moves)
        out<<"(uppercasePieceType "<<piece_name(el.get_symbol(), true)<<")\n";
    out<<'\n';
    for(const auto& el: piece_moves)
        out<<"(lowercasePieceType "<<piece_name(el.get_symbol(), false)<<")\n";
    out<<'\n';
    out<<"(<= (legal ?player noop)\n\t(role ?player)\n\t(not (true (control ?player))))\n\n";
    reuse_tool known_uppercase_moves;
    reuse_tool known_lowercase_moves;
    if(o.optimising() > 0){
        for(const auto& el: piece_moves){
            el.scan(known_uppercase_moves);
            el.scan(known_lowercase_moves);
        }
        known_uppercase_moves.delete_singletons();
        known_lowercase_moves.delete_singletons();
    }
    for(const auto& el: piece_moves){
        el.write_as_gdl(out,true, known_uppercase_moves, o);
        el.write_as_gdl(out,false, known_lowercase_moves, o);
    }
}

void game::write_next_state_logic(std::ofstream& out)const{
    out<<"(<= (next (control "<<player_name(true)<<"))\n\t(true (control "<<player_name(false)<<")))\n";
    out<<"(<= (next (control "<<player_name(false)<<"))\n\t(true (control "<<player_name(true)<<")))\n\n";
    out<<"(<= (next (step ?n))\n\t(true (step ?prevn))\n\t(stepSucc ?prevn ?n))\n\n";
    out<<"(<= (next (cell ?x ?y ?piece))\n\t(true (cell ?x ?y ?piece))\n\t(not (affected ?x ?y)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?x ?y ?x2 ?y2)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?x1 ?y1 ?x ?y)))\n";
    out<<"(<= (next (cell ?x ?y ?piece))\n\t(does ?player (move ?x1 ?y1 ?x ?y))\n\t(true (cell ?x1 ?y1 ?piece)))\n\n";
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal()){
        out<<"(<= (next (captureCounter ?piece ?n))\n\t(does ?player (move ?x1 ?y1 ?x ?y))\n\t(true (cell ?x ?y ?piece))\n\t(stepSucc ?prevn ?n)\n\t(distinctCells ?x1 ?y1 ?x ?y)\n\t(true (captureCounter ?piece ?prevn)))\n";
        out<<"(<= (next (captureCounter ?piece ?n))\n\t(does ?player (move ?x1 ?y1 ?x ?y))\n\t(true (captureCounter ?piece ?n))\n\t(not (true (cell ?x ?y ?piece))))\n";
        out<<"(<= (next (captureCounter ?piece ?n))\n\t(does ?player (move ?x1 ?y1 ?x1 ?y1))\n\t(true (captureCounter ?piece ?n)))\n";
    }
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= (next uppercaseBrokeThrough)\n\t(true uppercaseBrokeThrough))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= (next lowercaseBrokeThrough)\n\t(true lowercaseBrokeThrough))\n";
    uppercase_player_goals.write_breakthrough_detection(out, true);
    lowercase_player_goals.write_breakthrough_detection(out, false);
}

void game::write_terminal_state(std::ofstream& out)const{
    out<<"(<= terminal\n\t(true (step "<<number(turns_limit+1, turns_limit+1)<<")))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n(<= (capturedEnoughToWin "<<player_name(false)<<")\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n))\n\t(uppercasePieceType ?piece))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (capturedEnoughToWin "<<player_name(true)<<")\n\t(captureToWin ?piece ?n)\n\t(true (captureCounter ?piece ?n))\n\t(lowercasePieceType ?piece))\n";
    if(lowercase_player_goals.has_any_capture_goal() || uppercase_player_goals.has_any_capture_goal())
        out<<"(<= terminal\n\t(capturedEnoughToWin ?player))\n";
    out<<"\n(<= upperHasSomePiece\n\t(uppercasePieceType ?piece)\n\t(true (cell ?x ?y ?piece)))\n";
    out<<"(<= upperHasNoPieces\n\t(not upperHasSomePiece))\n";
    out<<"(<= terminal\n\tupperHasNoPieces)\n";
    out<<"\n(<= lowerHasSomePiece\n\t(lowercasePieceType ?piece)\n\t(true (cell ?x ?y ?piece)))\n";
    out<<"(<= lowerHasNoPieces\n\t(not lowerHasSomePiece))\n";
    out<<"(<= terminal\n\tlowerHasNoPieces)\n";
    out<<"\n(<= lowerHasLegalMove\n\t(legal "<<player_name(false)<<" ?move))\n";
    out<<"(<= upperHasLegalMove\n\t(legal "<<player_name(true)<<" ?move))\n";
    out<<"(<= terminal\n\t(not lowerHasLegalMove))\n";
    out<<"(<= terminal\n\t(not upperHasLegalMove))\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= terminal\n\t(true uppercaseBrokeThrough))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n(<= terminal\n\t(true lowercaseBrokeThrough))\n";
}

void game::write_goals(std::ofstream& out)const{
    out<<"(<= (goal "<<player_name(true)<<" 100)\n\tlowerHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(false)<<" 100)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(true)<<" 0)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(false)<<" 0)\n\tlowerHasNoPieces)\n\n";
    out<<"(<= (goal "<<player_name(true)<<" 100)\n\t(not lowerHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(false)<<" 100)\n\t(not upperHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(true)<<" 0)\n\t(not upperHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(false)<<" 0)\n\t(not lowerHasLegalMove))\n\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(true)<<" 100)\n\t(true uppercaseBrokeThrough))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(false)<<" 100)\n\t(true lowercaseBrokeThrough))\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(true)<<" 0)\n\t(true lowercaseBrokeThrough))\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(false)<<" 0)\n\t(true uppercaseBrokeThrough))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(true)<<" 100)\n\t(capturedEnoughToWin "<<player_name(true)<<"))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(false)<<" 100)\n\t(capturedEnoughToWin "<<player_name(false)<<"))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(true)<<" 0)\n\t(capturedEnoughToWin "<<player_name(false)<<"))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(false)<<" 0)\n\t(capturedEnoughToWin "<<player_name(true)<<"))\n";
    out<<"\n(<= (goal "<<player_name(false)<<" 50)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not (true lowercaseBrokeThrough))";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not (true uppercaseBrokeThrough))";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin lowercasePlayer))";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin uppercasePlayer))";
    out<<")\n";
    out<<"\n(<= (goal "<<player_name(true)<<" 50)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not (true lowercaseBrokeThrough))";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not (true uppercaseBrokeThrough))";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin lowercasePlayer))";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin uppercasePlayer))";
    out<<")\n";
}

void game::write_moves_succ(std::ofstream& out)const{
    uint max_number = max_number_of_repetitions();
    if(max_number > 1)
        for(uint i=0;i<max_number;++i)
            out<<"(movesSucc "<<i<<' '<<i+1<<")\n";
    else
        out<<"no move use repetition pattern\n";
}

uint game::max_number_of_repetitions(void)const{
    uint current_max = 0;
    for(const auto& el: piece_moves)
        current_max = std::max(current_max, el.max_number_of_repetitions(brd.get_width()*brd.get_height()));
    return current_max;
}

void game::write_as_gdl(const std::string& output_file_name, const options& o){
    std::ofstream out(output_file_name);
    out<<separator;
    out<<";; "<<name<<'\n';
    out<<separator<<'\n';
    out<<separator;
    out<<";; Roles\n";
    out<<separator<<'\n';
    out<<"(role "<<player_name(true)<<")\n";
    out<<"(role "<<player_name(false)<<")\n";
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
    out<<"(<= (occupied ?x ?y)\n\t(file ?x)\n\t(rank ?y)\n\t(true (cell ?x ?y ?piece)))\n";
    out<<"(<= (empty ?x ?y)\n\t(file ?x)\n\t(rank ?y)\n\t(not (occupied ?x ?y)))\n";
    out<<'\n'<<separator;
    out<<";; Pieces definition\n";
    out<<separator<<'\n';
    write_pieces_definition(out, o);
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
        lowercase_player_goals.write_piece_capture_counter(out,false, turns_limit);
        out<<'\n';
        uppercase_player_goals.write_piece_capture_counter(out,true, turns_limit);
        out<<"\n(<= (captureCounterStep ?piece ?n ?succn)";
        out<<"\n\t(stepSucc ?n ?succn)";
        out<<"\n\t(stepSucc ?prevn ?n)";
        out<<"\n\t(captureCounterStep ?piece ?prevn ?n)";
        out<<"\n\t(not (captureToWin ?piece ?n)))\n";
    }
    out<<'\n'<<separator;
    out<<";; Moves repetition counter\n";
    out<<separator<<'\n';
    write_moves_succ(out);
    out<<'\n'<<separator;
    out<<";; Steps counter\n";
    out<<separator<<'\n';
    write_steps_logic(out);
}
