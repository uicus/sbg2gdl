#ifndef PIECE
#define PIECE

#include<unordered_set>
#include<vector>

#include"move.hpp"
#include"parser.hpp"

class piece_parse_error : public std::exception{
        std::string description;
    public:
        piece_parse_error(void);
        piece_parse_error(uint line, uint character, const char* description);
        piece_parse_error(const piece_parse_error& source);
        piece_parse_error& operator=(const piece_parse_error& source);
        piece_parse_error(piece_parse_error&& source);
        piece_parse_error& operator=(piece_parse_error&& source);
        virtual ~piece_parse_error(void);

        virtual const char* what(void)const noexcept;
};

class piece{
        char symbol; // uppercase
        move move_pattern;
    public:
        piece(char s, move&& pattern);
        piece(const piece& src);
        piece& operator=(const piece& src);
        piece(piece&& src);
        piece& operator=(piece&& src);
        ~piece(void);
};

std::vector<piece> parse_pieces(parser& p, const std::unordered_set<char>& declared_pieces)throw(piece_parse_error);

#endif
