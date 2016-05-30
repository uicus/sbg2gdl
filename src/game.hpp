#ifndef GAME
#define GAME

#include<vector>
#include<string>
#include<fstream>

#include"board.hpp"
#include"piece.hpp"
#include"goals.hpp"
#include"types.hpp"
#include"parser_events.hpp"
#include"options.hpp"

class game{
        std::string name;
        board brd;
        std::vector<piece> piece_moves;
        uint turns_limit;
        goals uppercase_player_goals;
        goals lowercase_player_goals;
        game(
            std::string&& src_name,
            board&& src_brd,
            std::vector<piece>&& src_piece,
            uint limit,
            goals&& u_goals,
            goals&& l_goals);
        void write_steps_logic(std::ofstream& out, const options& o)const;
        void write_arithmetic(std::ofstream& out, uint max_number)const;
        void write_base(std::ofstream& out, const options& o)const;
        void write_input(std::ofstream& out)const;
        void write_initial_state(std::ofstream& out, const options& o)const;
        void write_pieces_definition(std::ofstream& out, const options& o)const;
        void write_next_state_logic(std::ofstream& out, const options& o)const;
        void write_terminal_state(std::ofstream& out, const options& o)const;
        void write_goals(std::ofstream& out)const;
        void write_moves_succ(std::ofstream& out)const;
        uint max_number_of_repetitions(void)const;
    public:
        game(void)=delete;

        friend game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error);

        void write_as_gdl(const std::string& output_file_name, const options& o);
};

game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error);

#endif
