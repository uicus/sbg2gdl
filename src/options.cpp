#include<cstring>

#include"options.hpp"

wrong_argument_error::wrong_argument_error(void):
std::exception(),
description("Wrong argument error"){
}

wrong_argument_error::wrong_argument_error(const std::string& source):
std::exception(),
description(source){
}

wrong_argument_error::~wrong_argument_error(void){
}

const char* wrong_argument_error::what(void)const noexcept{
    return description.c_str();
}

options::options(void):
just_verify(false),
optimise_domain(false),
show_warnings(true),
warnings_as_errors(false),
prolog_safe(false),
logarithmic_c(true),
share_c(false),
share_r(false),
share_s(false),
skip_i(false),
skip_b(false),
skip_c(false),
output_name("a.gdl"){
}

options::options(uint number_of_args, const char** args)throw(wrong_argument_error):
just_verify(false),
optimise_domain(false),
show_warnings(true),
warnings_as_errors(false),
prolog_safe(false),
logarithmic_c(true),
share_c(false),
share_r(false),
share_s(false),
skip_i(false),
skip_b(false),
skip_c(false),
output_name("a.gdl"){
    for(uint i=0;i<number_of_args;++i){
        if(args[i][0] != '-')
            throw wrong_argument_error("Every flag should begin with \"-\"");
        else{
            if(!std::strcmp(args[i], "-v"))
                just_verify = true;
            else if(!std::strcmp(args[i], "-o")){
                ++i;
                if(i >= number_of_args)
                    throw wrong_argument_error("Flag \"-o\" should be succeeded by output file name");
                else
                    output_name = args[i];
            }
            else if(!std::strcmp(args[i], "-Odomain"))
                optimise_domain = true;
            else if(!std::strcmp(args[i], "-O")){
                prolog_safe = false;
                logarithmic_c = true;
                share_c = true;
                share_r = true;
                share_s = true;
            }
            else if(!std::strcmp(args[i], "-Whide"))
                show_warnings = false;
            else if(!std::strcmp(args[i], "-Werror"))
                warnings_as_errors = true;
            else if(!std::strcmp(args[i], "-skip-input"))
                skip_i = true;
            else if(!std::strcmp(args[i], "-skip-base"))
                skip_b = true;
            else if(!std::strcmp(args[i], "-skip-comments"))
                skip_c = true;
            else if(!std::strcmp(args[i], "-prolog-safe"))
                prolog_safe = true;
            else if(!std::strcmp(args[i], "-flinear-turn-counter"))
                logarithmic_c = false;
            else if(!std::strcmp(args[i], "-fshare-concatenations"))
                share_c = true;
            else if(!std::strcmp(args[i], "-fshare-repetitions"))
                share_r = true;
            else if(!std::strcmp(args[i], "-fshare-sums"))
                share_s = true;
            else
                throw wrong_argument_error("Unrecognized flag");
        }
    }
}

bool options::verifying(void)const{
    return just_verify;
}

bool options::domain_optimising(void)const{
    return optimise_domain;
}

bool options::showing_warnings(void)const{
    return show_warnings;
}
bool options::escalating_warnings(void)const{
    return warnings_as_errors;
}

bool options::allowed_unsafe(void)const{
    return !prolog_safe;
}

bool options::logarithmic_counter(void)const{
    return logarithmic_c;
}

bool options::share_concatenations(void)const{
    return share_c;
}

bool options::share_repetitions(void)const{
    return share_r;
}

bool options::share_sums(void)const{
    return share_s;
}

bool options::skip_input(void)const{
    return skip_i;
}
bool options::skip_base(void)const{
    return skip_b;
}
bool options::skip_comments(void)const{
    return skip_c;
}

const std::string& options::output_file(void)const{
    return output_name;
}
