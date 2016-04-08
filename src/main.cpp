#include<iostream>
#include<fstream>
#include<string>
#include<unordered_set>

#include"options.hpp"
#include"parser.hpp"
#include"board.hpp"
#include"piece.hpp"
#include"goals.hpp"
#include"parser_events.hpp"

typedef std::pair<uint, std::pair<goals, goals>> all_goals_type;

std::pair<std::pair<board,std::vector<piece>>,all_goals_type> parse_file(const std::string& file_name, std::vector<warning>& warnings_list)throw(std::exception);

int main(int argc, const char** argv){
    if(argc < 2){
        std::cerr<<"No input specified"<<std::endl;
    }
    else{
        try{
            std::string input_file_name(argv[1]);
            options o(argc-2, argv+2);
            std::vector<warning> warnings_list;
            parse_file(input_file_name, warnings_list);
            if(!o.escalating_warnings() || warnings_list.size() == 0)
                std::cout<<"ACC"<<std::endl;
            if(o.showing_warnings() || o.escalating_warnings())
                for(auto const& w: warnings_list)
                    std::cerr<<w.to_string(o.escalating_warnings())<<std::endl;
        }
        catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    return 0;
}

std::pair<std::pair<board,std::vector<piece>>,all_goals_type> parse_file(const std::string& file_name, std::vector<warning>& warnings_list)throw(std::exception){
    std::ifstream input_file(file_name);
    if(!input_file.good())
        throw parse_error("Couldn't open input file");
    parser p(&input_file);
    std::string game_name;
    while(!p.expect_whitespace())
        game_name.push_back(p.expect_plain_char());
    if(game_name == "")
        throw parse_error(p.get_line_number(), p.get_char_in_line_number(), "Input should begin with game name");
    std::pair<board, std::unordered_set<char>> board_result = parse_board(p, warnings_list);
    std::vector<piece> pieces = parse_pieces(p, warnings_list, board_result.second);
    all_goals_type g = parse_goals(p, warnings_list, board_result.second, board_result.first);
    p.expect_whitespace();
    if(!p.expect_end_of_file())
        throw parse_error(p.get_line_number(), p.get_char_in_line_number(), "Unexpected characters at the end of input");
    return std::make_pair(std::make_pair(board_result.first, pieces), g);
}
