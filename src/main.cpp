#include<iostream>
#include<fstream>
#include<string>
#include<unordered_set>

#include"options.hpp"
#include"game.hpp"
#include"parser_events.hpp"

int main(int argc, const char** argv){
    if(argc < 2){
        std::cerr<<"No input specified"<<std::endl;
    }
    else{
        try{
            std::string input_file_name(argv[1]);
            options o(argc-2, argv+2);
            std::vector<warning> warnings_list;
            game g = parse_game(input_file_name, warnings_list);
            if(!o.escalating_warnings() || warnings_list.size() == 0)
                std::cout<<"ACC"<<std::endl;
            if(o.showing_warnings() || o.escalating_warnings())
                for(auto const& w: warnings_list)
                    std::cerr<<w.to_string(o.escalating_warnings())<<std::endl;
            if(!o.verifying())
                g.write_as_gdl(o.output_file());
        }
        catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    return 0;
}
