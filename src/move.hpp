#ifndef MOVE
#define MOVE

#include<vector>
#include<string>

#include"types.hpp"

enum on{
    empty, // e
    enemy, // p
    own // w
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
};

class moves_concatenation;

class moves_sum{
        std::vector<moves_concatenation> m;
        void wrap_in_brackets(void);
    public:
        moves_sum(void);
        moves_sum(const moves_sum& src);
        moves_sum& operator=(const moves_sum& src);
        moves_sum(moves_sum&& src);
        moves_sum& operator=(moves_sum&& src);
        ~moves_sum(void);

        std::string to_string(void)const;

        moves_sum& append(moves_concatenation&& mc);

        moves_sum& add_moves(moves_sum&& src);
        moves_sum& concat_moves(moves_sum&& src);
        moves_sum& increment(void);
        moves_sum& decrement(void);
        moves_sum& set_star(void);
        moves_sum& set_number(uint number_of_repetitions);
};

class bracketed_move;

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

        std::string to_string(void)const;

        moves_concatenation& append(bracketed_move&& bm);

        moves_concatenation& concat_moves(moves_concatenation&& src);
        moves_concatenation& increment(void);
        moves_concatenation& decrement(void);
        moves_concatenation& set_star(void);
        moves_concatenation& set_number(uint number_of_repetitions);
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

        std::string to_string(void)const;

        bracketed_move& increment(void);
        bracketed_move& decrement(void);
        bracketed_move& set_star(void);
        bracketed_move& set_number(uint number);
};

typedef moves_sum move;

#endif
