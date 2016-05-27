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

inline std::string subsection(const std::string& title){
    std::string result;
    result += '\n' + separator;
    result += ";; "+title+"\n";
    result += separator+'\n';
    return result;
}

inline uint length_of(uint v){
    uint i;
    for(i=0;v>>=1;++i);
    return i+1;
}

inline std::string exist_at_least(uint quantity){
    std::string result;
    result += "(<= (existAtLeast"+std::to_string(quantity)+" ?piece)";
    result += "\n\t(pieceType ?piece)";
    result += "\n\t(true (cell ?x1 ?y1 ?piece))";
    for(uint i=2;i<=quantity;++i){
        result += "\n\t(true (cell ?x"+std::to_string(i)+" ?y"+std::to_string(i)+" ?piece))";
        result += "\n\t(less ?x"+std::to_string(i)+" ?y"+std::to_string(i)+" ?x"+std::to_string(i-1)+" ?y"+std::to_string(i-1)+")";
    }
    result += ")\n";
    return result;
}

const std::string digit = "(digit 0)\n(digit 1)\n";

/*inline std::string zero(uint max_number){
    uint length = length_of(max_number);
    std::string result = "(zero (number";
    for(uint i=0;i<length;++i)result+=" 0";
    result+="))\n";
    return result;
}*/

inline std::string succ(const std::string& name, uint max_number, bool logarithmic){
    std::string result;
    if(logarithmic){
        uint length = length_of(max_number);
        for(uint i=0;i<length;++i){
            if(i<length-1)
                result+="(<= ";
            result+="("+name;
            for(uint j=length-1;j>i;--j)
                result+=" ?x"+std::to_string(j);
            result+=" 0";
            for(uint j=i;j>0;--j)
                result+=" 1";
            //result+=") (number";
            for(uint j=length-1;j>i;--j)
                result+=" ?x"+std::to_string(j);
            result+=" 1";
            for(uint j=i;j>0;--j)
                result+=" 0";
            result+=")";
            for(uint j=length-1;j>i;--j)
                result+="\n\t(digit ?x"+std::to_string(j)+')';
            if(i<length-1)
                result+=')';
            result+='\n';
        }
    }
    else
        for(uint i=1;i<=max_number;++i){
            result += "("+name+' '+std::to_string(i)+' '+std::to_string(i+1)+")\n";
            if(i%2==0)
                result+= "("+name+"Even "+std::to_string(i)+")\n";
        }
    return result;
}

inline std::string number(uint n, uint max_number, bool logarithmic){
    std::string result;
    if(logarithmic){
        uint length = length_of(max_number);
        uint window = 1<<(length-1);
        for(uint i=0;i<length;window>>=1,++i)
            result+=((window&n) ? " 1" : " 0");
    }
    else
        result += " "+std::to_string(n);
    return result;
}

inline std::string variable(const std::string& base_name, uint max_number, bool logarithmic){
    std::string result;
    if(logarithmic){
        uint length = length_of(max_number);
        for(uint i=0;i<length;++i)
            result += " ?"+base_name+std::to_string(i);
    }
    else
        result += " ?"+base_name;
    return result;
}

#endif
