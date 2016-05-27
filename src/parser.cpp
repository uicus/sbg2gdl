#include"parser.hpp"

parser::parser(std::istream* in):
input(in),
buffer(""),
number_of_chars_in_line(0),
line_starts(){
    std::string temp;
    std::getline(*input, temp);
    buffer += temp + '\n';
    line_starts.push(0);
    number_of_chars_in_line = 0;
}

parser::parser(parser&& src):
input(src.input),
buffer(std::move(src.buffer)),
number_of_chars_in_line(src.number_of_chars_in_line),
line_starts(std::move(src.line_starts)){
}

parser& parser::operator=(parser&& src){
    if(this == &src)
        return *this;
    input = src.input;
    buffer = std::move(src.buffer);
    number_of_chars_in_line = src.number_of_chars_in_line;
    line_starts = std::move(src.line_starts);
    return *this;
}

parser::~parser(void){
}

parser::fallback_point parser::save_fallback_point(void)const{
    fallback_point fbp;
    fbp.line_number = get_line_number();
    fbp.char_in_line_number = get_char_in_line_number();
    return fbp;
}

parser_info parser::dump_parser_info(std::string&& info)const{
    parser_info pi{get_line_number(),get_char_in_line_number(), std::move(info)};
    return pi;
}

void parser::fall_back(const fallback_point& fbp){
    while(get_line_number() > fbp.line_number)line_starts.pop();
    number_of_chars_in_line = fbp.char_in_line_number;
}

void parser::go_back(void){
    if(number_of_chars_in_line == 0){
        if(get_line_number() > 1){
            number_of_chars_in_line = line_starts.top() - 1;
            line_starts.pop();
            number_of_chars_in_line -= line_starts.top();
        }
    }
    else
        --number_of_chars_in_line;
}

uint parser::get_line_number(void)const{
    return line_starts.size();
}

uint parser::get_char_in_line_number(void)const{
    return number_of_chars_in_line;
}

char parser::pointed_char(void)const{
    return line_starts.top() + number_of_chars_in_line >= buffer.size() ? '\0' : buffer[line_starts.top() + number_of_chars_in_line];
}

bool parser::end_of_buffer(void)const{
    return line_starts.empty() || number_of_chars_in_line + line_starts.top() >= buffer.size();
}

bool parser::end_of_line(void)const{
    return pointed_char() == '\n' || end_of_buffer();
}

bool parser::whitespace(void)const{
    return isspace(pointed_char());//pointed_char() == '\r' || pointed_char() == ' ' || pointed_char() == '\t' || end_of_line();
}

char parser::get_next_char(void){
    char result;
    if(end_of_buffer() && input->eof())
        result = '\0';
    else if(end_of_line()){
        result = pointed_char();
        line_starts.push(line_starts.top() + number_of_chars_in_line + 1);
        number_of_chars_in_line = 0;
        if(end_of_buffer() && !input->eof()){
            std::string temp;
            std::getline(*input, temp);
            buffer += temp + '\n';
        }
    }
    else{
        result = pointed_char();
        ++number_of_chars_in_line;
    }
    return result;
}

bool parser::seek_end_of_multicomment(void){
    char prev_char = '\0';
    char current_char = '\0';
    while((current_char = get_next_char()) != '\0'){
        if(current_char == '*')
            prev_char = '*';
        else{
            if(current_char == '/'){
                if(prev_char == '*')
                    return true;
                else
                    prev_char = '/';
            }
            else
                prev_char = current_char;
        }
    }
    return false;
}

void parser::seek_end_of_line_comment(void){
    while(!end_of_line() && get_next_char() != '\0');
}

bool parser::expect_multicomment(void){
    if(expect_string("/*")){
        seek_end_of_multicomment();
        return true;
    }
    return false;
}

bool parser::expect_line_comment(void){
    if(expect_string("//")){
        seek_end_of_line_comment();
        return true;
    }
    return false;
}

parser_result<single_move> parser::expect_single_move(void){
    parser_result<single_move> result{false,true,single_move(),{0,0,""}};
    fallback_point fbp = save_fallback_point();
    parser_result<int> x_number_parsing_result = expect_int();
    if(!x_number_parsing_result){
        result.info = x_number_parsing_result.info;
        result.recoverable = x_number_parsing_result.recoverable;
        fall_back(fbp);
        return result;
    }
    result.recoverable = false;
    expect_whitespace();
    if(!expect_plain_char(',')){
        result.info = dump_parser_info(std::string("Expected \',\', encountered \'")+pointed_char()+"\'");
        fall_back(fbp);
        return result;
    }
    expect_whitespace();
    parser_result<int> y_number_parsing_result = expect_int();
    if(!y_number_parsing_result){
        result.info = y_number_parsing_result.info;
        fall_back(fbp);
        return result;
    }
    expect_whitespace();
    if(!expect_plain_char(',')){
        result.info = dump_parser_info(std::string("Expected \',\', encountered \'")+pointed_char()+"\'");
        fall_back(fbp);
        return result;
    }
    expect_whitespace();
    char kind_char = expect_plain_char();
    if(kind_char != 'e' && kind_char != 'p' && kind_char != 'w'){
        result.info = dump_parser_info(std::string("Expected \'e\', \'p\' or \'w\', encountered \'")+kind_char+"\'");
        fall_back(fbp);
        return result;
    }
    on kind = kind_char == 'e' ? empty : (kind_char == 'p' ? enemy : own);
    result.result = single_move(x_number_parsing_result.result, y_number_parsing_result.result, kind);
    result.found = true;
    return result;
}

parser_result<bracketed_move> parser::expect_bracketed_move(void){
    parser_result<bracketed_move> result{false,true,bracketed_move(single_move()),{0,0,""}};
    fallback_point fbp = save_fallback_point();
    if(!expect_plain_char('(')){
        result.info = dump_parser_info(std::string("Expected \'(\', encountered \'")+pointed_char()+"\'");
        fall_back(fbp);
        return result;
    }
    result.recoverable = false;
    expect_whitespace();
    parser_result<single_move> single_move_result = expect_single_move();
    if(single_move_result)
        result.result = bracketed_move(std::move(single_move_result.result));
    else{
        if(!single_move_result.recoverable){
            fall_back(fbp);
            result.info = single_move_result.info;
            return result;
        }
        parser_result<moves_sum> moves_sum_result = expect_move_pattern();
        if(moves_sum_result)
            result.result = bracketed_move(std::move(moves_sum_result.result));
        else{
            if(!moves_sum_result.recoverable)
                result.info = moves_sum_result.info;
            else
                result.info = dump_parser_info(std::string("Expected single move pattern or moves sum, encountered \'")+pointed_char()+"\'");
            fall_back(fbp);
            return result;
        }
    }
    expect_whitespace();
    if(!expect_plain_char(')')){
        result.info = dump_parser_info(std::string("Expected \')\', encountered \'")+pointed_char()+"\'");
        fall_back(fbp);
        return result;
    }
    fallback_point after_bracket_fbp = save_fallback_point();
    expect_whitespace();
    if(expect_plain_char('^')){
        expect_whitespace();
        if(expect_plain_char('*'))
            result.result.set_star();
        else{
            parser_result<int> repetition_number_parsing_result = expect_int();
            if(!repetition_number_parsing_result){
                if(repetition_number_parsing_result.recoverable)
                    result.info = dump_parser_info(std::string("Expected \'*\' or positive number after \'^\'"));
                else
                    result.info = repetition_number_parsing_result.info;
                fall_back(fbp);
                return result;
            }
            else if(repetition_number_parsing_result.result < 1){
                result.info = dump_parser_info(std::string("Number after \'^\' have to be positive"));
                fall_back(fbp);
                return result;
            }
            else
                result.result.set_number(repetition_number_parsing_result.result);
        }
    }
    else
        fall_back(after_bracket_fbp);
    result.found = true;
    return result;
}

parser_result<moves_concatenation> parser::expect_moves_concatenation(void){
    parser_result<moves_concatenation> result{false,true,moves_concatenation(),{0,0,""}};
    fallback_point fbp = save_fallback_point();
    parser_result<bracketed_move> bm_parsing_result = expect_bracketed_move();
    if(!bm_parsing_result){
        if(bm_parsing_result.recoverable)
            result.info = dump_parser_info(std::string("Expected at least one bracketed move"));
        else
            result.info = bm_parsing_result.info;
        result.recoverable = bm_parsing_result.recoverable;
        fall_back(fbp);
        return result;
    }
    result.recoverable = false;
    result.result.append(std::move(bm_parsing_result.result));
    fallback_point after_first_fbp;
    while(after_first_fbp = save_fallback_point(), expect_whitespace(), bm_parsing_result = expect_bracketed_move())
        result.result.append(std::move(bm_parsing_result.result));
    if(!bm_parsing_result.recoverable){
        result.info = bm_parsing_result.info;
        fall_back(fbp);
        return result;
    }
    fall_back(after_first_fbp);
    result.found = true;
    return result;
}

bool parser::expect_string(const std::string& str){
    fallback_point fbp = save_fallback_point();
    for(uint i = 0;i<str.length();++i){
        if(get_next_char() != str[i]){
            fall_back(fbp);
            return false;
        }
    }
    return true;
}

bool parser::expect_plain_char(char c){
    fallback_point fbp = save_fallback_point();
    if(get_next_char() != c){
        fall_back(fbp);
        return false;
    }
    return true;
}

char parser::expect_plain_char(void){
    return get_next_char();
}

bool parser::expect_whitespace(void){
    bool whitespace_encountered = false;
    while(true){ // guaranteed to return: in every iteration parser goes forward at least one character or return
        while(whitespace()){
            whitespace_encountered = true;
            if(get_next_char() == '\0')
                return true;
        }
        if(!expect_multicomment() && !expect_line_comment())
            return whitespace_encountered;
        whitespace_encountered = true;
    }
    return true; // ureachable
}

bool parser::expect_end_of_file(void){
    fallback_point fbp = save_fallback_point();
    if(get_next_char() == '\0')
        return true;
    else{
        fall_back(fbp);
        return false;
    }
}

parser_result<int> parser::expect_int(void){
    parser_result<int> result{false,true,0,{0,0,""}};
    //result.found = false;
    //result.result = 0;
    bool sign = true; // true -> +, false -> -
    fallback_point fbp = save_fallback_point();
    char current_char;
    current_char = get_next_char();
    if(current_char == '-'){
        sign = false;
        current_char = get_next_char();
    }
    else if(current_char == '+')
        current_char = get_next_char();
    if(!isdigit(current_char)){
        result.info = dump_parser_info(std::string("Expected digit, encountered \'")+current_char+"\'");
        fall_back(fbp);
        return result;
    }
    else
        result.result = current_char - '0';
    result.recoverable = false;
    while(true){
        current_char = get_next_char();
        if(!isdigit(current_char)){
            go_back();
            result.found = true;
            result.result *= (sign ? 1 : -1);
            return result;
        }
        else{
            result.result *= 10;
            result.result += current_char - '0';
        }
    }
    return result; // unreachable
}
parser_result<move> parser::expect_move_pattern(void){
    parser_result<move> result{false,true,move(),{0,0,""}};
    fallback_point fbp = save_fallback_point();
    parser_result<moves_concatenation> mc_parsing_result = expect_moves_concatenation();
    if(!mc_parsing_result){
        if(mc_parsing_result.recoverable)
            result.info = dump_parser_info(std::string("Expected at least one moves concatenation"));
        else
            result.info = mc_parsing_result.info;
        result.recoverable = mc_parsing_result.recoverable;
        fall_back(fbp);
        return result;
    }
    result.recoverable = false;
    result.result.append(std::move(mc_parsing_result.result));
    fallback_point after_first_fbp;
    while(true){
        after_first_fbp = save_fallback_point();
        expect_whitespace();
        if(!expect_plain_char('+')){
            fall_back(after_first_fbp);
            break;
        }
        expect_whitespace();
        mc_parsing_result = expect_moves_concatenation();
        if(!mc_parsing_result){
            if(!mc_parsing_result.recoverable){
                result.recoverable = false;
                fall_back(fbp);
                result.info = mc_parsing_result.info;
                return result;
            }
            else{
                fall_back(after_first_fbp);
                break;
            }
        }
        result.result.append(std::move(mc_parsing_result.result));
    }
    result.found = true;
    return result;
}
