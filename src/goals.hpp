#ifndef GOALS
#define GOALS

#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<fstream>

#include"parser.hpp"
#include"board.hpp"
#include"parser_events.hpp"

class goals_parse_error : public parse_error{
    public:
        goals_parse_error(void);
        goals_parse_error(uint line, uint character, const std::string& description);
        virtual ~goals_parse_error(void);

        virtual const char* what(void)const noexcept;
        virtual std::string to_string(void)const;
};

class goals{
        std::unordered_map<char, std::vector<std::pair<uint, uint>>> piece_placement;
        std::unordered_map<char, uint> piece_capture;
    public:
        goals(void);

        void add_piece_placement_goal(char symbol, uint x, uint y);
        void add_piece_capture_goal(char symbol, uint number_of_pieces);

        bool has_any_capture_goal(void)const;
        void write_capture_detection(std::ofstream& out, bool capturing_lower_pieces, std::unordered_set<uint>& needed_at_least_predicates)const;

        bool has_any_breakthrough_goal(void)const;
        void write_breakthrough_detection(std::ofstream& out, bool uppercase)const;
};

std::pair<uint, std::pair<goals, goals>> parse_goals(
    parser& p,
    std::vector<warning>& warnings_list,
    const std::unordered_set<char>& declared_pieces,
    const board& declared_board)throw(goals_parse_error,parse_error);

#endif
