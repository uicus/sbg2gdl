#ifndef BOARD
#define BOARD

#include<unordered_set>
#include<vector>

#include"types.hpp"
#include"parser.hpp"
#include"parser_events.hpp"

class board_parse_error : public parse_error{
    public:
        board_parse_error(void);
        board_parse_error(uint line, uint character, const char* description);
        board_parse_error(const board_parse_error& source);
        board_parse_error& operator=(const board_parse_error& source);
        board_parse_error(board_parse_error&& source);
        board_parse_error& operator=(board_parse_error&& source);
        virtual ~board_parse_error(void);

        virtual const char* what(void)const noexcept;
};

class board{
        uint width;
        std::vector<std::vector<std::pair<bool, char>>> fields; // false -> "lowercase" player, true -> "uppercase" player
        board(uint w, uint h);
    public:
        board(const board& src);
        board& operator=(const board& src);
        board(board&& src);
        board& operator=(board&& src);
        ~board(void);

        bool inside(uint x, uint y)const;
        uint get_width(void)const;
        uint get_height(void)const;
        friend std::pair<board, std::unordered_set<char>> parse_board(parser& p, std::vector<warning>& warnigns_list)throw(board_parse_error);
};

std::pair<board, std::unordered_set<char>> parse_board(
    parser& p,
    std::vector<warning>& warnigns_list)throw(board_parse_error);

#endif
