#ifndef GDL_CONSTANTS
#define GDL_CONSTANTS

#include<string>

#include"types.hpp"

const std::string separator = ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";

std::string player_name(bool uppercase);
std::string piece_name(char symbol, bool uppercase);

inline std::string player_name(bool uppercase){
    return uppercase ? "uppercasePlayer" : "lowercasePlayer";
}

inline std::string piece_name(char symbol, bool uppercase){
    return uppercase ? std::string("u")+char(toupper(symbol)) : std::string("l")+char(tolower(symbol));
}

inline uint length_of(uint v){
    uint i;
    for(i=0;v>>=1;++i);
    return i+1;
}

const std::string digit = "(digit 0)\n(digit 1)\n";

inline std::string zero(uint max_number){
    uint length = length_of(max_number);
    std::string result = "(zero (number";
    for(uint i=0;i<length;++i)result+=" 0";
    result+="))\n";
    return result;
}

inline std::string succ(const std::string& name, uint max_number){
    uint length = length_of(max_number);
    std::string result;
    for(uint i=0;i<length;++i){
        if(i<length-1)
            result+="(<= ";
        result+="("+name+" (number";
        for(uint j=length-1;j>i;--j)
            result+=" ?x"+std::to_string(j);
        result+=" 0";
        for(uint j=i;j>0;--j)
            result+=" 1";
        result+=") (number";
        for(uint j=length-1;j>i;--j)
            result+=" ?x"+std::to_string(j);
        result+=" 1";
        for(uint j=i;j>0;--j)
            result+=" 0";
        result+="))";
        for(uint j=length-1;j>i;--j)
            result+="\n\t(digit ?x"+std::to_string(j)+')';
        if(i<length-1)
            result+=')';
        result+='\n';
    }
    return result;
}

inline std::string number(uint n, uint max_number){
    uint length = length_of(max_number);
    uint window = 1<<(length-1);
    std::string result = "(number";
    for(uint i=0;i<length;window>>=1,++i)
        result+=((window&n) ? " 1" : " 0");
    result+=')';
    return result;
}

#endif
