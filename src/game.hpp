#ifndef GAME
#define GAME

#include<vector>
#include<string>

#include"board.hpp"
#include"piece.hpp"
#include"goals.hpp"
#include"types.hpp"
#include"parser_events.hpp"

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
    public:
        game(const game& src);
        game& operator=(const game& src);
        game(game&& src);
        game& operator=(game&& src);
        ~game(void);

        friend game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error);

        void write_as_gdl(const std::string& output_file_name);
};

game parse_game(const std::string& file_name, std::vector<warning>& warnings_list)throw(parse_error);

#endif
