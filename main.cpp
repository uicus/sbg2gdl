#include<iostream>
#include<fstream>
#include<string>
#include<exception>
#include<unordered_set>

#include"options.hpp"
#include"parser.hpp"
#include"board.hpp"
#include"piece.hpp"
#include"goals.hpp"

typedef std::pair<uint, std::pair<goals, goals>> all_goals_type;

std::pair<std::pair<board,std::vector<piece>>,all_goals_type> parse_file(const std::string& file_name)throw(std::exception);

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr<<"No input specified"<<std::endl;
    }
    else{
        try{
            std::string input_file_name(argv[1]);
            options o(argc-2, argv+2);
            parse_file(input_file_name);
            std::cout<<"ACC"<<std::endl;
        }
        catch(std::exception& e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    return 0;
}

std::pair<std::pair<board,std::vector<piece>>,all_goals_type> parse_file(const std::string& file_name)throw(std::exception){
    std::ifstream input_file(file_name);
    parser p(&input_file);
    std::string game_name;
    while(!p.expect_whitespace())
        game_name.push_back(p.expect_plain_char());
    if(game_name == "")
        throw std::exception();
    std::pair<board, std::unordered_set<char>> board_result = parse_board(p);
    std::vector<piece> pieces = parse_pieces(p, board_result.second);
    all_goals_type g = parse_goals(p, board_result.second, board_result.first);
    return std::make_pair(std::make_pair(board_result.first, pieces), g);
    p.expect_whitespace();
    if(!p.expect_end_of_file())
        throw std::exception();
}
