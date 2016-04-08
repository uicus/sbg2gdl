#include<cstring>

#include"options.hpp"

wrong_argument_error::wrong_argument_error(void):
std::exception(),
description("Wrong argument error"){
}

wrong_argument_error::wrong_argument_error(const char* source):
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
optimisation_level(0),
output_name("a.gdl"){
}

options::options(uint number_of_args, const char** args)throw(wrong_argument_error):
just_verify(false),
optimise_domain(false),
show_warnings(true),
warnings_as_errors(false),
optimisation_level(0),
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
                else
                    optimisation_level = args[i][2] - '0';
            }
            else if(!std::strcmp(args[i], "-Whide"))
                show_warnings = false;
            else if(!std::strcmp(args[i], "-Werror"))
                warnings_as_errors = true;
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
optimisation_level(source.optimisation_level),
output_name(source.output_name){
}

options& options::operator=(const options& source){
    if(this == &source)
        return *this;
    just_verify = source.just_verify;
    optimise_domain = source.optimise_domain;
    show_warnings = source.show_warnings;
    warnings_as_errors = source.warnings_as_errors;
    optimisation_level = source.optimisation_level;
    output_name = source.output_name;
    return *this;
}

options::options(options&& source):
just_verify(source.just_verify),
optimise_domain(source.optimise_domain),
show_warnings(source.show_warnings),
warnings_as_errors(source.warnings_as_errors),
optimisation_level(source.optimisation_level),
output_name(std::move(source.output_name)){
}

options& options::operator=(options&& source){
    if(this == &source)
        return *this;
    just_verify = source.just_verify;
    optimise_domain = source.optimise_domain;
    show_warnings = source.show_warnings;
    warnings_as_errors = source.warnings_as_errors;
    optimisation_level = source.optimisation_level;
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

uint options::optimising(void)const{
    return optimisation_level;
}

const std::string& options::output_file(void)const{
    return output_name;
}
