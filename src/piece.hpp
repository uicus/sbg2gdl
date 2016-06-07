#ifndef PIECE
#define PIECE

#include<unordered_set>
#include<vector>
#include<fstream>

#include"move.hpp"
#include"parser.hpp"
#include"parser_events.hpp"
#include"options.hpp"
#include"board.hpp"

class piece_parse_error : public parse_error{
    public:
        piece_parse_error(void);
        piece_parse_error(uint line, uint character, const std::string& description);
        virtual ~piece_parse_error(void);

        virtual const char* what(void)const noexcept;
        virtual std::string to_string(void)const;
};

class piece{
        char symbol; // uppercase
        move move_pattern;
    public:
        piece(char s, move&& pattern);

        char get_symbol(void)const;

        uint max_number_of_repetitions(uint treat_star_as)const;
        void scan_for_concatenations(reuse_tool& known)const;
        void scan_for_subsums(const piece& second, reuse_tool& known, bool uppercase)const;

        void write_as_gdl(std::ofstream& out, bool uppercase, reuse_tool& known_moves, const options& o)const;
};

std::vector<piece> parse_pieces(
    parser& p,
    std::vector<warning>& warnigns_list,
    const std::unordered_set<char>& declared_pieces,
    const board& brd)throw(piece_parse_error);

#endif
