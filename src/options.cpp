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

wrong_argument_error::wrong_argument_error(const wrong_argument_error& source):
std::exception(),
description(source.description){
}

wrong_argument_error& wrong_argument_error::operator=(const wrong_argument_error& source){
    description = source.description;
    return *this;
}

wrong_argument_error::wrong_argument_error(wrong_argument_error&& source):
std::exception(),
description(std::move(source.description)){
}

wrong_argument_error& wrong_argument_error::operator=(wrong_argument_error&& source){
    if(this == &source)
        return *this;
    description = std::move(source.description);
    return *this;
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
            else if(args[i][1] == 'O'){
                if(args[i][2] < '0' || args[i][2] > '3' || args[i][3] != '\0')
                    throw wrong_argument_error("Unsupported optimisation level");
                else{
                    uint optimisation_level = args[i][2] - '0';
                    switch(optimisation_level){
                    case 0:
                        prolog_safe = true;
                        logarithmic_c = false;
                        share_c = false;
                        share_r = false;
                        share_s = false;
                        break;
                    case 1:
                    default:
                        prolog_safe = false;
                        logarithmic_c = true;
                        share_c = true;
                        share_r = true;
                        share_s = true;
                        break;
                    }
                }
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

options::options(const options& source):
just_verify(source.just_verify),
optimise_domain(source.optimise_domain),
show_warnings(source.show_warnings),
warnings_as_errors(source.warnings_as_errors),
prolog_safe(source.prolog_safe),
logarithmic_c(source.logarithmic_c),
share_c(source.share_c),
share_r(source.share_r),
share_s(source.share_s),
skip_i(source.skip_i),
skip_b(source.skip_b),
skip_c(source.skip_c),
output_name(source.output_name){
}

options& options::operator=(const options& source){
    if(this == &source)
        return *this;
    just_verify = source.just_verify;
    optimise_domain = source.optimise_domain;
    show_warnings = source.show_warnings;
    warnings_as_errors = source.warnings_as_errors;
    prolog_safe = source.prolog_safe;
    logarithmic_c = source.logarithmic_c;
    share_c = source.share_c;
    share_r = source.share_r;
    share_s = source.share_s;
    skip_i = source.skip_i;
    skip_b = source.skip_b;
    skip_c = source.skip_c;
    output_name = source.output_name;
    return *this;
}

options::options(options&& source):
just_verify(source.just_verify),
optimise_domain(source.optimise_domain),
show_warnings(source.show_warnings),
warnings_as_errors(source.warnings_as_errors),
prolog_safe(source.prolog_safe),
logarithmic_c(source.logarithmic_c),
share_c(source.share_c),
share_r(source.share_r),
share_s(source.share_s),
skip_i(source.skip_i),
skip_b(source.skip_b),
skip_c(source.skip_c),
output_name(std::move(source.output_name)){
}

options& options::operator=(options&& source){
    if(this == &source)
        return *this;
    just_verify = source.just_verify;
    optimise_domain = source.optimise_domain;
    show_warnings = source.show_warnings;
    warnings_as_errors = source.warnings_as_errors;
    prolog_safe = source.prolog_safe;
    logarithmic_c = source.logarithmic_c;
    share_c = source.share_c;
    share_r = source.share_r;
    share_s = source.share_s;
    skip_i = source.skip_i;
    skip_b = source.skip_b;
    skip_c = source.skip_c;
    output_name = std::move(source.output_name);
    return *this;
}

options::~options(void){
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
