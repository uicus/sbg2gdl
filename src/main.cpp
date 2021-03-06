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
        std::cerr<<"Usage: \"sbg2gdl [flags] input_file\""<<std::endl;
        std::cerr<<"Available flags:"<<std::endl;
        std::cerr<<"\"-o output_file\" - write output to file with given name; defaults to \"a.gdl\""<<std::endl;
        std::cerr<<"\"-v\" - just verify input; do not generate output file"<<std::endl;
        std::cerr<<"\"-Whide\" - do not show warnings"<<std::endl;
        std::cerr<<"\"-Werror\" - treat warnings as errors"<<std::endl;
        std::cerr<<"\"-prolog-safe\" - generated code is reasonable for prolog players"<<std::endl;
        std::cerr<<"\"-flinear-turn-counter\" - implement turn counter using linear number of rules"<<std::endl;
        std::cerr<<"\"-fno-share-concatenations\" - do not implement the same moves concatenation for every piece separately"<<std::endl;
        std::cerr<<"\"-fno-share-repetitions\" - do not implement the same moves repetition (^number, ^*) for every piece separately"<<std::endl;
        std::cerr<<"\"-fno-share-sums\" - do not implement the same moves sum for every piece separately"<<std::endl;
        std::cerr<<"\"-skip-input\" - do not generate input segment"<<std::endl;
        std::cerr<<"\"-skip-base\" - do not generate base segment"<<std::endl;
        std::cerr<<"\"-skip-comments\" - do not generate comments"<<std::endl;
    }
    else{
        try{
            std::string input_file_name(argv[argc-1]);
            options o(argc-2, argv+1);
            std::vector<warning> warnings_list;
            game g = parse_game(input_file_name, warnings_list);
            if(o.showing_warnings() || o.escalating_warnings())
                for(auto const& w: warnings_list)
                    std::cerr<<w.to_string(o.escalating_warnings())<<std::endl;
            if((!o.escalating_warnings() || warnings_list.size() == 0) && o.verifying())
                std::cout<<"Verification successful"<<std::endl;
            if(!o.verifying() && (!o.escalating_warnings() || warnings_list.size() == 0)){
                g.write_as_gdl(o.output_file(), o);
                std::cout<<"Produced "<<o.output_file()<<std::endl;
            }
        }
        catch(parse_error& e){
            std::cerr<<e.to_string()<<std::endl;
        }
        catch(wrong_argument_error& e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    return 0;
}
