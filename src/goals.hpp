#ifndef GOALS
#define GOALS

#include<vector>
#include<unordered_map>
#include<unordered_set>

#include"parser.hpp"
#include"board.hpp"
#include"parser_events.hpp"

class goals_parse_error : public parse_error{
    public:
        goals_parse_error(void);
        goals_parse_error(uint line, uint character, const char* description);
        goals_parse_error(const goals_parse_error& source);
        goals_parse_error& operator=(const goals_parse_error& source);
        goals_parse_error(goals_parse_error&& source);
        goals_parse_error& operator=(goals_parse_error&& source);
        virtual ~goals_parse_error(void);

        virtual const char* what(void)const noexcept;
};

class goals{
        std::unordered_map<char, std::vector<std::pair<uint, uint>>> piece_placement;
        std::unordered_map<char, uint> piece_capture;
    public:
        goals(void);
        goals(const goals& src);
        goals& operator=(const goals& src);
        goals(goals&& src);
        goals& operator=(goals&& src);
        ~goals(void);

        void add_piece_placement_goal(char symbol, uint x, uint y);
        void add_piece_capture_goal(char symbol, uint number_of_pieces);
};

std::pair<uint, std::pair<goals, goals>> parse_goals(
    parser& p,
    std::vector<warning>& warnings_list,
    const std::unordered_set<char>& declared_pieces,
    const board& declared_board)throw(goals_parse_error);

#endif