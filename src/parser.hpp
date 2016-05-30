#ifndef PARSER
#define PARSER

#include<istream>
#include<string>
#include<stack>

#include"types.hpp"
#include"move.hpp"

struct parser_info{
    uint line_number;
    uint char_number;
    std::string human_readable_info;
};

template<typename T>
struct parser_result{
    bool found : 1;
    bool recoverable : 1; // if !found tells whether parser should return this error or go on with parsing
    //union{
        T result;
        parser_info info;
    //};
    ~parser_result(void){} // to surpress warning about not inlining
    operator bool(void){ return found; }
};

class parser{
        struct fallback_point{
            uint line_number;
            uint char_in_line_number;
        };
        std::istream* input;
        std::string buffer;
        uint number_of_chars_in_line;
        std::stack<uint> line_starts;
        fallback_point save_fallback_point(void)const;
        parser_info dump_parser_info(std::string&& info)const;
        void fall_back(const fallback_point& fbp);
        char pointed_char(void)const;
        bool end_of_buffer(void)const;
        bool end_of_line(void)const;
        bool whitespace(void)const;
        char get_next_char(void); // EOF == \0
        bool seek_end_of_multicomment(void); // tells whether EOF (false) came first or "*/" (true)
        void seek_end_of_line_comment(void);
        bool expect_multicomment(void);
        bool expect_line_comment(void);
        parser_result<single_move> expect_single_move(void);
        parser_result<bracketed_move> expect_bracketed_move(void);
        parser_result<moves_concatenation> expect_moves_concatenation(void); // cannot be empty concatenation
    public:
        parser(std::istream* in);
        parser(void)=delete;
        parser(const parser&)=delete;
        parser& operator=(const parser&)=delete;

        uint get_line_number(void)const;
        uint get_char_in_line_number(void)const;

        void go_back(void);

        // following methods falls_back when failed to find expected pattern
        bool expect_string(const std::string& str);
        bool expect_plain_char(char c);
        char expect_plain_char(void);
        bool expect_whitespace(void); // merges ' ', '\t', '\n' and comments into one whitespace
        bool expect_end_of_file(void);
        parser_result<int> expect_int(void); // not overflow safe
        parser_result<move> expect_move_pattern(void); // i.e. moves_sum; cannot be empty sum
};

#endif
