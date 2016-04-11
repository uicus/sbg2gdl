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
    out<<"(<= (base (control ?p))\n\t(role ?p))\n\n";
    out<<"(base (step 1))\n";
    out<<"(<= (base (step ?next))\n\t(succ ?s ?next))\n\n";
    out<<"(<= (base (cell ?x ?y ?piece))\n\t(file ?x)\n\t(rank ?y)\n\t(pieceType ?piece))\n\n";
    out<<separator;
    out<<";; Input\n";
    out<<separator<<'\n';
    out<<"(<= (input ?player noop)\n\t(role ?player))\n";
    for(const auto& el: piece_moves)
        el.write_possible_input(out);
    out<<'\n'<<separator;
    out<<";; Initial state\n";
    out<<separator<<'\n';
    brd.write_initial_state(out);
    out<<"\n(init (step 1))\n";
    out<<"\n(init (control uppercasePlayer))\n";
    out<<'\n'<<separator;
    out<<";; Artithmetic\n";
    out<<separator<<'\n';
    write_arithmetic(out,std::max(brd.get_height(),brd.get_width()));
    out<<'\n'<<separator;
    out<<";; Board definition\n";
    out<<separator<<'\n';
    brd.write_files_logic(out);
    brd.write_ranks_logic(out);
    out<<'\n'<<separator;
    out<<";; Steps counter\n";
    out<<separator<<'\n';
    write_steps_logic(out);
}
