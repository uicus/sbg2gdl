#ifndef MOVE
#define MOVE

#include<vector>
#include<set>
#include<map>
#include<string>
#include<fstream>

#include"types.hpp"
#include"options.hpp"

enum on{
    empty, // e
    enemy, // p
    own // w
};

class moves_concatenation;
class bracketed_move;
class moves_sum;
typedef moves_sum move;

class reuse_tool{
        std::map<moves_sum, std::string> known_sums;
        std::map<bracketed_move, std::string> known_bracketed;
        std::map<moves_concatenation, uint> existing_concatenations;
        std::map<moves_concatenation, std::string> concatenations_to_add;
        std::map<moves_concatenation, std::string> known_concatenations;
        std::map<moves_sum, std::string> existing_subsums;
        uint next_concatenation_id(void)const;
        uint next_subsum_id(void)const;
    public:
        reuse_tool(void);
        reuse_tool(const reuse_tool& src);
        reuse_tool& operator=(const reuse_tool& src);
        reuse_tool(reuse_tool&& src);
        reuse_tool& operator=(reuse_tool&& src);
        ~reuse_tool(void);

        void insert_new_concatenation(moves_concatenation&& src);
        void delete_singletons(void);
        bool there_are_new_concatenations(void)const;
        std::pair<uint, uint> best_subconcatenation(const moves_concatenation& src, bool can_be_whole)const;
        void write_all_concatenations(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            bool uppercase_player,
            const std::string& original_name,
            uint& next_free_id,
            const options& o);

        void insert_subsum(const moves_sum& src, bool uppercase);

        std::string get_or_insert(const moves_concatenation& src, bool uppercase);
        std::string get_or_insert(
            const moves_sum& src,
            std::vector<std::pair<uint, moves_sum>>& additional_moves_to_write,
            const std::string& move_name,
            uint& id);
        std::string get_or_insert(
            const bracketed_move& src,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            const std::string& move_name,
            uint& id);
};

class single_move{
        int x_delta;
        int y_delta;
        on kind;
    public:
        single_move(void);
        single_move(int x, int y, on o);
        single_move(const single_move& src);
        single_move& operator=(const single_move& src);
        single_move(single_move&& src);
        single_move& operator=(single_move&& src);
        ~single_move(void);

        std::string to_string(void)const;

        bool operator==(const single_move& m2)const;
        bool operator<(const single_move& m2)const;

        void write_as_gdl(
            std::ofstream& out,
            bool uppercase_player,
            const std::string& start_x_name,
            const std::string& start_y_name,
            const std::string& end_x_name,
            const std::string& end_y_name)const;
};

class moves_sum{
        std::set<moves_concatenation> m;
        void wrap_in_brackets(void);
    public:
        moves_sum(void);
        moves_sum(const moves_sum& src);
        moves_sum& operator=(const moves_sum& src);
        moves_sum(moves_sum&& src);
        moves_sum& operator=(moves_sum&& src);
        ~moves_sum(void);

        bool operator==(const moves_sum& m2)const;
        bool operator<(const moves_sum& m2)const;

        std::string to_string(void)const;

        moves_sum& append(moves_concatenation&& mc);

        moves_sum& add_moves(moves_sum&& src);
        moves_sum& concat_moves(moves_sum&& src);
        moves_sum& increment(void);
        moves_sum& decrement(void);
        moves_sum& set_star(void);
        moves_sum& set_number(uint number_of_repetitions);

        uint max_number_of_repetitions(uint treat_star_as)const;

        void scan_for_concatenations(reuse_tool& known)const;

        void write_as_gdl(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            reuse_tool& known,
            const std::string& move_name,
            uint current_id,
            bool uppercase_player,
            uint& next_free_id,
            const options& o)const;
};

class moves_concatenation{
        std::vector<bracketed_move> m;
        void wrap_in_brackets(void);
    public:
        moves_concatenation(void);
        moves_concatenation(const moves_concatenation& src);
        moves_concatenation& operator=(const moves_concatenation& src);
        moves_concatenation(moves_concatenation&& src);
        moves_concatenation& operator=(moves_concatenation&& src);
        ~moves_concatenation(void);

        bool operator==(const moves_concatenation& m2)const;
        bool operator<(const moves_concatenation& m2)const;
        uint length(void)const;

        std::string to_string(void)const;

        moves_concatenation& append(bracketed_move&& bm);

        moves_concatenation& concat_moves(moves_concatenation&& src);
        moves_concatenation& increment(void);
        moves_concatenation& decrement(void);
        moves_concatenation& set_star(void);
        moves_concatenation& set_number(uint number_of_repetitions);

        uint max_number_of_repetitions(uint treat_star_as)const;

        moves_concatenation sub_move(uint begin, uint end)const;
        void scan_for_concatenations(reuse_tool& known)const;

        void write_as_gdl(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            reuse_tool& known,
            const std::string& move_name,
            bool uppercase_player,
            const std::string& start_x_name,
            const std::string& start_y_name,
            const std::string& end_x_name,
            const std::string& end_y_name,
            uint& next_free_id,
            const options& o,
            bool can_be_whole = true)const;
        void write_freestanding_predicate(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            reuse_tool& known,
            const std::string& move_name,
            bool uppercase_player,
            const std::string& original_name,
            uint& next_free_id,
            const options& o)const;
};

class bracketed_move{
        bool sum;
        uint number_of_repetitions; // 0 means '*'
        union{
            single_move* single_m;
            moves_sum* m_sum;
        };
    public:
        bracketed_move(const single_move& src);
        bracketed_move(const moves_sum& src);
        bracketed_move(single_move&& src);
        bracketed_move(moves_sum&& src);
        bracketed_move(const bracketed_move& src);
        bracketed_move& operator=(const bracketed_move& src);
        bracketed_move(bracketed_move&& src);
        bracketed_move& operator=(bracketed_move&& src);
        ~bracketed_move(void);

        bool operator==(const bracketed_move& m2)const;
        bool operator<(const bracketed_move& m2)const;
        bool can_be_merged(const bracketed_move& m2)const;

        std::string to_string(void)const;

        bracketed_move& increment(void);
        bracketed_move& decrement(void);
        bracketed_move& set_star(void);
        bracketed_move& set_number(uint number);
        uint get_number_of_repetitions(void)const;

        uint max_number_of_repetitions(uint treat_star_as)const;

        void scan_for_concatenations(reuse_tool& known)const;

        void write_as_gdl(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            std::vector<std::pair<uint, bracketed_move>>& additional_bracketed_moves,
            reuse_tool& known,
            const std::string& move_name,
            bool uppercase_player,
            const std::string& start_x_name,
            const std::string& start_y_name,
            const std::string& end_x_name,
            const std::string& end_y_name,
            uint& next_free_id,
            const options& o)const;
        void write_one_repetition(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            reuse_tool& known,
            const std::string& move_name,
            bool uppercase_player,
            const std::string& start_x_name,
            const std::string& start_y_name,
            const std::string& end_x_name,
            const std::string& end_y_name,
            uint& next_free_id,
            const options& o)const;
        void write_freestanding_predicate(
            std::ofstream& out,
            std::vector<std::pair<uint, move>>& additional_moves_to_write,
            reuse_tool& known,
            const std::string& move_name,
            uint current_id,
            bool uppercase_player,
            uint& next_free_id,
            const options& o)const;
};

#endif
