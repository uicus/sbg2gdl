#ifndef PARSER_EVENTS
#define PARSER_EVENTS

#include<exception>
#include<string>
#include<vector>

#include"types.hpp"

class parse_error : public std::exception{
    protected:
        uint line_number;
        uint char_number;
        std::string description;
    public:
        //parse_error(void);
        parse_error(const std::string& source);
        parse_error(uint line, uint character, const std::string& description);
        parse_error(const parse_error& source);
        parse_error& operator=(const parse_error& source);
        parse_error(parse_error&& source);
        parse_error& operator=(parse_error&& source);
        virtual ~parse_error(void);

        virtual const char* what(void)const noexcept;
};

class warning{
        uint line_number;
        uint char_number;
        std::string description;
    public:
        warning(uint line, uint column, const std::string& src);
        warning(const warning& src);
        warning& operator=(const warning& src);
        warning(warning&& src);
        warning& operator=(warning&& src);
        ~warning(void);

        std::string to_string(bool as_error)const;
};

#endif
