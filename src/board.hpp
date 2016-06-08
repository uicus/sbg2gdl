#ifndef BOARD
#define BOARD

#include<unordered_set>
#include<vector>
#include<fstream>

#include"types.hpp"
#include"parser.hpp"
#include"parser_events.hpp"

class board_parse_error : public parse_error{
    public:
        board_parse_error(void);
        board_parse_error(uint line, uint character, const std::string& description);
        virtual ~board_parse_error(void);

        virtual const char* what(void)const noexcept;
        virtual std::string to_string(void)const;
};

class board{
        uint width;
        std::vector<std::vector<std::pair<bool, char>>> fields; // false -> "lowercase" player, true -> "uppercase" player
        board(uint w, uint h);
    public:
        board(void)=delete;

        bool inside(uint x, uint y)const;
        uint get_width(void)const;
        uint get_height(void)const;
        friend std::pair<board, std::unordered_set<char>> parse_board(parser& p, std::vector<warning>& warnigns_list)throw(board_parse_error,parse_error);
        void write_initial_state(std::ofstream& out)const;
        void write_ranks_logic(std::ofstream& out)const;
        void write_files_logic(std::ofstream& out)const;
};

std::pair<board, std::unordered_set<char>> parse_board(
    parser& p,
    std::vector<warning>& warnigns_list)throw(board_parse_error,parse_error);

#endif
