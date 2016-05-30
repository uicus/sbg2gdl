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
        virtual ~parse_error(void);

        virtual const char* what(void)const noexcept;
        virtual std::string to_string(void)const;
};

class warning{
        uint line_number;
        uint char_number;
        std::string description;
    public:
        warning(uint line, uint column, const std::string& src);

        std::string to_string(bool as_error)const;
};

#endif
