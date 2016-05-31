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

game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error){
    std::ifstream input_file(file_name);
    if(!input_file.good())
        throw parse_error("Couldn't open input file");
    parser p(&input_file);
    std::string game_name;
    p.expect_whitespace();
    if(!p.expect_string("<<"))
        throw parse_error(p.get_line_number(), p.get_char_in_line_number(), "Input should begin with game name enclosed with \'<< >>\'");
    while(!p.expect_string(">>") && !p.expect_end_of_file())
        game_name.push_back(p.expect_plain_char());
    p.expect_whitespace();
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

void game::write_steps_logic(std::ofstream& out, const options& o)const{
    out<<digit<<'\n';
    out<<succ("stepSucc",turns_limit+1, o.logarithmic_counter())<<'\n';
}

void game::write_arithmetic(std::ofstream& out, uint less_than_number)const{
    for(uint i=0;i<less_than_number;++i)
        out<<"(arithmeticSucc "<<i<<' '<<i+1<<")\n";
    out<<"\n(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?x ?y))\n";
    out<<"(<= (sum ?x 0 ?x)\n\t(arithmeticSucc ?y ?x))\n";
    out<<"(<= (sum ?x ?y ?z)\n\t(arithmeticSucc ?x ?succx)\n\t(arithmeticSucc ?prevy ?y)\n\t(sum ?succx ?prevy ?z))\n";
    out<<"\n(<= (sub ?x ?y ?z)\n\t(sum ?z ?y ?x))\n";
    out<<"\n(<= (equal ?x ?x)\n\t(sum ?x 0 ?x))\n"; // just to bound x
    if(uppercase_player_goals.has_any_capture_goal() || lowercase_player_goals.has_any_capture_goal()){
        out<<"\n(<= (distinctCells ?x1 ?y1 ?x2 ?y2)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y1)\n\t(rank ?y2)\n\t(distinct ?x1 ?x2))\n";
        out<<"(<= (distinctCells ?x1 ?y1 ?x2 ?y2)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y1)\n\t(rank ?y2)\n\t(distinct ?y1 ?y2))\n\n";
        out<<"(<= (less ?x1 ?y ?x2 ?y)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y)\n\t(distinct ?x1 ?x2)\n\t(sum ?x1 ?a ?x2))\n";
        out<<"(<= (less ?x1 ?y1 ?x2 ?y2)\n\t(file ?x1)\n\t(file ?x2)\n\t(rank ?y1)\n\t(rank ?y2)\n\t(distinct ?y1 ?y2)\n\t(sum ?y1 ?a ?y2))\n";
    }
}

void game::write_base(std::ofstream& out, const options& o)const{
    out<<"(base (step"<<number(1,turns_limit+1, o.logarithmic_counter())<<"))\n";
    out<<"(<= (base (step"<<variable("next",turns_limit+1, o.logarithmic_counter())<<"))";
    out<<"\n\t(stepSucc"<<variable("s",turns_limit+1, o.logarithmic_counter())<<variable("next",turns_limit+1, o.logarithmic_counter())<<"))\n\n";
    out<<"(<= (base (cell ?x ?y ?piece))\n\t(file ?x)\n\t(rank ?y)\n\t(pieceType ?piece))\n";
}

void game::write_input(std::ofstream& out)const{
    out<<"(<= (input ?player noop)\n\t(role ?player))\n";
    out<<"(<= (input ?player (move ?x1 ?y1 ?x2 ?y2))\n\t(file ?x1)\n\t(rank ?y1)\n\t(file ?x2)\n\t(rank ?y2)\n\t(role ?player))\n";
}

void game::write_initial_state(std::ofstream& out, const options& o)const{
    brd.write_initial_state(out);
    out<<"\n(init (step"<<number(1, turns_limit+1, o.logarithmic_counter())<<"))\n";
}

void game::write_pieces_definition(std::ofstream& out, const options& o)const{
    if(o.logarithmic_counter()){
        out<<"(<= (control "<<player_name(true)<<")\n\t(true (step"<<variable("x", (turns_limit+1)/2, o.logarithmic_counter())<<" 0)))\n";
        out<<"(<= (control "<<player_name(false)<<")\n\t(true (step"<<variable("x", (turns_limit+1)/2, o.logarithmic_counter())<<" 1)))\n";
    }
    else{
        out<<"(<= (control "<<player_name(true)<<")\n\t(true (step ?x))\n\t(not (stepSuccEven ?x)))\n";
        out<<"(<= (control "<<player_name(false)<<")\n\t(true (step ?x))\n\t(stepSuccEven ?x))\n";
    }
    out<<"(<= (pieceType ?piece)\n\t(uppercasePieceType ?piece))\n";
    out<<"(<= (pieceType ?piece)\n\t(lowercasePieceType ?piece))\n\n";
    for(const auto& el: piece_moves)
        out<<"(uppercasePieceType "<<piece_name(el.get_symbol(), true)<<")\n";
    out<<'\n';
    for(const auto& el: piece_moves)
        out<<"(lowercasePieceType "<<piece_name(el.get_symbol(), false)<<")\n";
    out<<'\n';
    out<<"(<= (legal ?player noop)\n\t(role ?player)\n\t(not (control ?player)))\n\n";
    reuse_tool known_uppercase_moves;
    reuse_tool known_lowercase_moves;
    if(o.share_concatenations()){
        for(const auto& el: piece_moves){
            el.scan_for_concatenations(known_uppercase_moves);
            el.scan_for_concatenations(known_lowercase_moves);
        }
        known_uppercase_moves.delete_singletons();
        known_lowercase_moves.delete_singletons();
    }
    if(o.share_sums()){
        auto almost_end = piece_moves.end();
        --almost_end;
        for(auto i=piece_moves.begin();i!=almost_end;++i)
            for(auto j=i+1;j!=piece_moves.end();++j){
                i->scan_for_subsums(*j, known_uppercase_moves, true);
                i->scan_for_subsums(*j, known_lowercase_moves, false);
            }
        known_uppercase_moves.write_all_subsums(out, true, o);
        known_lowercase_moves.write_all_subsums(out, false, o);
    }
    for(const auto& el: piece_moves){
        el.write_as_gdl(out,true, known_uppercase_moves, o);
        el.write_as_gdl(out,false, known_lowercase_moves, o);
    }
}

void game::write_next_state_logic(std::ofstream& out, const options& o)const{
    //out<<"(<= (next (control "<<player_name(true)<<"))\n\t(true (control "<<player_name(false)<<")))\n";
    //out<<"(<= (next (control "<<player_name(false)<<"))\n\t(true (control "<<player_name(true)<<")))\n\n";
    out<<"(<= (next (step"<<variable("n",turns_limit+1, o.logarithmic_counter())<<"))";
    out<<"\n\t(true (step"<<variable("prevn",turns_limit+1, o.logarithmic_counter())<<"))";
    out<<"\n\t(stepSucc"<<variable("prevn",turns_limit+1,o.logarithmic_counter())<<variable("n",turns_limit+1, o.logarithmic_counter())<<"))\n\n";
    out<<"(<= (next (cell ?x ?y ?piece))\n\t(true (cell ?x ?y ?piece))\n\t(not (affected ?x ?y)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?x ?y ?x2 ?y2)))\n";
    out<<"(<= (affected ?x ?y)\n\t(does ?player (move ?x1 ?y1 ?x ?y)))\n";
    out<<"(<= (next (cell ?x ?y ?piece))\n\t(does ?player (move ?x1 ?y1 ?x ?y))\n\t(true (cell ?x1 ?y1 ?piece)))\n\n";
}

void game::write_terminal_state(std::ofstream& out, const options& o)const{
    out<<"(<= terminal\n\t(true (step"<<number(turns_limit+1, turns_limit+1, o.logarithmic_counter())<<")))\n";
    std::unordered_set<uint> needed_at_least_predicates;
    lowercase_player_goals.write_capture_detection(out, false, needed_at_least_predicates);
    uppercase_player_goals.write_capture_detection(out, true, needed_at_least_predicates);
    for(const auto& el: needed_at_least_predicates)
        out<<exist_at_least(el);
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
    if(uppercase_player_goals.has_any_breakthrough_goal()){
        out<<"\n(<= terminal\n\tuppercaseBrokeThrough)\n";
        uppercase_player_goals.write_breakthrough_detection(out, true);
    }
    if(lowercase_player_goals.has_any_breakthrough_goal()){
        out<<"\n(<= terminal\n\tlowercaseBrokeThrough)\n";
        lowercase_player_goals.write_breakthrough_detection(out, false);
    }
}

void game::write_goals(std::ofstream& out, const options& o)const{
    out<<"(<= (goal "<<player_name(true)<<" 100)\n\tlowerHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(false)<<" 100)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(true)<<" 0)\n\tupperHasNoPieces)\n";
    out<<"(<= (goal "<<player_name(false)<<" 0)\n\tlowerHasNoPieces)\n\n";
    out<<"(<= (goal "<<player_name(true)<<" 100)\n\t(not lowerHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(false)<<" 100)\n\t(not upperHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(true)<<" 0)\n\t(not upperHasLegalMove))\n";
    out<<"(<= (goal "<<player_name(false)<<" 0)\n\t(not lowerHasLegalMove))\n\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(true)<<" 100)\n\tuppercaseBrokeThrough)\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(false)<<" 100)\n\tlowercaseBrokeThrough)\n";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(true)<<" 0)\n\tlowercaseBrokeThrough)\n";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"(<= (goal "<<player_name(false)<<" 0)\n\tuppercaseBrokeThrough)\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(true)<<" 100)\n\t(capturedEnoughToWin "<<player_name(true)<<"))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(false)<<" 100)\n\t(capturedEnoughToWin "<<player_name(false)<<"))\n";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(true)<<" 0)\n\t(capturedEnoughToWin "<<player_name(false)<<"))\n";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"(<= (goal "<<player_name(false)<<" 0)\n\t(capturedEnoughToWin "<<player_name(true)<<"))\n";
    out<<"\n(<= (goal "<<player_name(false)<<" 50)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    out<<"\n\t(true (step"<<number(turns_limit+1, turns_limit+1, o.logarithmic_counter())<<"))";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not lowercaseBrokeThrough)";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not uppercaseBrokeThrough)";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin lowercasePlayer))";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin uppercasePlayer))";
    out<<")\n";
    out<<"\n(<= (goal "<<player_name(true)<<" 50)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    out<<"\n\t(true (step"<<number(turns_limit+1, turns_limit+1, o.logarithmic_counter())<<"))";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not lowercaseBrokeThrough)";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not uppercaseBrokeThrough)";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin lowercasePlayer))";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin uppercasePlayer))";
    out<<")\n";
    out<<"\n(<= (goal "<<player_name(false)<<" 0)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    out<<"\n\t(not (true (step"<<number(turns_limit+1, turns_limit+1, o.logarithmic_counter())<<")))";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not lowercaseBrokeThrough)";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not uppercaseBrokeThrough)";
    if(lowercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin lowercasePlayer))";
    if(uppercase_player_goals.has_any_capture_goal())
        out<<"\n\t(not (capturedEnoughToWin uppercasePlayer))";
    out<<")\n";
    out<<"\n(<= (goal "<<player_name(true)<<" 0)\n\tlowerHasSomePiece\n\tupperHasSomePiece\n\tlowerHasLegalMove\n\tupperHasLegalMove";
    out<<"\n\t(not (true (step"<<number(turns_limit+1, turns_limit+1, o.logarithmic_counter())<<")))";
    if(lowercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not lowercaseBrokeThrough)";
    if(uppercase_player_goals.has_any_breakthrough_goal())
        out<<"\n\t(not uppercaseBrokeThrough)";
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
        out<<"; no move use repetition pattern\n";
}

uint game::max_number_of_repetitions(void)const{
    uint current_max = 0;
    for(const auto& el: piece_moves)
        current_max = std::max(current_max, el.max_number_of_repetitions(brd.get_width()*brd.get_height()));
    return current_max;
}

void game::write_as_gdl(const std::string& output_file_name, const options& o){
    std::ofstream out(output_file_name);
    if(!o.skip_comments()){
        out<<separator;
        out<<";; "<<name<<'\n';
        out<<separator<<'\n';
        out<<subsection("Roles");
    }
    out<<"(role "<<player_name(true)<<")\n";
    out<<"(role "<<player_name(false)<<")\n";
    if(!o.skip_base()){
        if(!o.skip_comments())
            out<<subsection("Base");
        write_base(out, o);
    }
    if(!o.skip_input()){
        if(!o.skip_comments())
            out<<subsection("Input");
        write_input(out);
    }
    if(!o.skip_comments())
        out<<subsection("Initial state");
    write_initial_state(out, o);
    if(!o.skip_comments())
        out<<subsection("Arithmetics");
    write_arithmetic(out,std::max(brd.get_height(),brd.get_width()));
    if(!o.skip_comments())
        out<<subsection("Board definition");
    brd.write_files_logic(out);
    out<<'\n';
    brd.write_ranks_logic(out);
    out<<"(<= (occupied ?x ?y)\n\t(file ?x)\n\t(rank ?y)\n\t(true (cell ?x ?y ?piece)))\n";
    out<<"(<= (empty ?x ?y)\n\t(file ?x)\n\t(rank ?y)\n\t(not (occupied ?x ?y)))\n";
    if(!o.skip_comments())
        out<<subsection("Pieces definition");
    write_pieces_definition(out, o);
    if(!o.skip_comments())
        out<<subsection("Next state logic");
    write_next_state_logic(out, o);
    if(!o.skip_comments())
        out<<subsection("Terminal states");
    write_terminal_state(out, o);
    if(!o.skip_comments())
        out<<subsection("Goals");
    write_goals(out, o);
    if(!o.skip_comments())
        out<<subsection("Moves repetition counter");
    write_moves_succ(out);
    if(!o.skip_comments())
        out<<subsection("Steps counter");
    write_steps_logic(out, o);
}
