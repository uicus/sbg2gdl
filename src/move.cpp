#include"move.hpp"

single_move::single_move(void):
x_delta(0),
y_delta(0),
kind(empty){
}

single_move::single_move(int x, int y, on o):
x_delta(x),
y_delta(y),
kind(o){
}

single_move::~single_move(void){
}

single_move::single_move(const single_move& src):
x_delta(src.x_delta),
y_delta(src.y_delta),
kind(src.kind){
}

single_move& single_move::operator=(const single_move& src){
    if(this == &src)
        return *this;
    x_delta = src.x_delta;
    y_delta = src.y_delta;
    kind = src.kind;
    return *this;
}

single_move::single_move(single_move&& src):
x_delta(src.x_delta),
y_delta(src.y_delta),
kind(src.kind){
}

single_move& single_move::operator=(single_move&& src){
    if(this == &src)
        return *this;
    x_delta = src.x_delta;
    y_delta = src.y_delta;
    kind = src.kind;
    return *this;
}

void single_move::write_as_gdl(
    std::ofstream& out,
    bool uppercase_player,
    const std::string& start_x_name,
    const std::string& start_y_name,
    const std::string& end_x_name,
    const std::string& end_y_name)const{
    if(x_delta > 0)
        out<<"\n\t(sum ?"<<start_x_name<<' '<<x_delta<<" ?"<<end_x_name<<")";
    else if(x_delta < 0)
        out<<"\n\t(sub ?"<<start_x_name<<' '<<(-x_delta)<<" ?"<<end_x_name<<")";
    else
        out<<"\n\t(equal ?"<<start_x_name<<" ?"<<end_x_name<<")";
    int true_y_delta = (uppercase_player ? y_delta : -y_delta);
    if(true_y_delta > 0)
        out<<"\n\t(sum ?"<<start_y_name<<' '<<true_y_delta<<" ?"<<end_y_name<<")";
    else if(true_y_delta < 0)
        out<<"\n\t(sub ?"<<start_y_name<<' '<<(-true_y_delta)<<" ?"<<end_y_name<<")";
    else
        out<<"\n\t(equal ?"<<start_y_name<<" ?"<<end_y_name<<")";
    if(kind == empty)
        out<<"\n\t(empty ?"<<end_x_name<<" ?"<<end_y_name<<")";
    else{
        out<<"\n\t(true (cell ?"<<end_x_name<<" ?"<<end_y_name<<" ?piece))";
        if(uppercase_player != (kind == own))
            out<<"\n\t(lowercasePieceType ?piece)";
        else
            out<<"\n\t(uppercasePieceType ?piece)";
    }
}

moves_sum::moves_sum(void):
m(){
}

moves_sum::moves_sum(const moves_sum& src):
m(src.m){
}

moves_sum& moves_sum::operator=(const moves_sum& src){
    if(this == &src)
        return *this;
    m = src.m;
    return *this;
}

moves_sum::moves_sum(moves_sum&& src):
m(std::move(src.m)){
}

moves_sum& moves_sum::operator=(moves_sum&& src){
    if(this == &src)
        return *this;
    m = std::move(src.m);
    return *this;
}

moves_sum::~moves_sum(void){
}

void moves_sum::wrap_in_brackets(void){
    moves_sum this_move;
    this_move.m = std::move(m);
    bracketed_move bm(std::move(this_move));
    moves_concatenation mc;
    mc.append(std::move(bm));
    m = std::vector<moves_concatenation>{mc};
}

moves_sum& moves_sum::append(moves_concatenation&& mc){
    m.push_back(std::move(mc));
    return *this;
}

moves_sum& moves_sum::add_moves(moves_sum&& src){
    if(m.size() < src.m.size())
        std::swap(m, src.m);
    if(m.size() + src.m.size() > m.capacity())
        m.reserve((m.size() + src.m.size())*2);
    if(this == &src)
        for(uint i = 0;i<src.m.size();++i)
            m.push_back(src.m[i]);
    else if(src.m.size() == 0);
    else
        for(auto& el: src.m)
            m.push_back(std::move(el));
    return *this;
}

moves_sum& moves_sum::concat_moves(moves_sum&& src){
    if(m.size() < src.m.size())
        std::swap(m, src.m);
    if(m.size() + src.m.size() > m.capacity() && m.size() > 0)
        m.reserve((m.size() + src.m.size())*2);
    if(this == &src)
        for(uint i = 0;i<src.m.size();++i)
            m.push_back(src.m[i]);
    else if(m.size() == 0);
    else if(src.m.size() == 0)
        m.clear();
    else if(m.size() == 1)
        m[0].concat_moves(std::move(src.m[0]));
    else{
        if(src.m.size() == 1)
            for(auto& el: m)
                el.concat_moves(moves_concatenation(src.m[0]));
        else{
            wrap_in_brackets();
            bracketed_move bm2(std::move(src));
            m[0].append(std::move(bm2));
        }
    }
    return *this;
}

moves_sum& moves_sum::increment(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].increment();
    return *this;
}

moves_sum& moves_sum::decrement(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].decrement();
    return *this;
}

moves_sum& moves_sum::set_star(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].set_star();
    return *this;
}

moves_sum& moves_sum::set_number(uint number_of_repetitions){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].set_number(number_of_repetitions);
    return *this;
}

uint moves_sum::max_number_of_repetitions(void)const{
    uint current_max = 0;
    for(const auto& el: m)
        current_max = std::max(current_max, el.max_number_of_repetitions());
    return current_max;
}

void moves_sum::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    const std::string& move_name,
    uint current_id,
    bool uppercase_player,
    uint& next_free_id)const{
    for(const auto& el: m){
        out<<"(<= ("<<move_name<<current_id<<" ?xin ?yin ?xout ?yout)";
        el.write_as_gdl(
            out,
            additional_moves_to_write,
            additional_bracketed_moves,
            move_name,
            uppercase_player,
            "xin",
            "yin",
            "xout",
            "yout",
            next_free_id);
        out<<")\n\n";
    }
}

moves_concatenation::moves_concatenation(void):
m(){
}

moves_concatenation::moves_concatenation(const moves_concatenation& src):
m(src.m){
}

moves_concatenation& moves_concatenation::operator=(const moves_concatenation& src){
    if(this == &src)
        return *this;
    m = src.m;
    return *this;
}

moves_concatenation::moves_concatenation(moves_concatenation&& src):
m(std::move(src.m)){
}

moves_concatenation& moves_concatenation::operator=(moves_concatenation&& src){
    if(this == &src)
        return *this;
    m = std::move(src.m);
    return *this;
}

moves_concatenation::~moves_concatenation(void){
}

void moves_concatenation::wrap_in_brackets(void){
    moves_concatenation this_move;
    this_move.m = std::move(m);
    moves_sum ms;
    ms.append(std::move(this_move));
    bracketed_move bm(std::move(ms));
    m = std::vector<bracketed_move>{bm};
}

moves_concatenation& moves_concatenation::append(bracketed_move&& bm){
    m.push_back(std::move(bm));
    return *this;
}

moves_concatenation& moves_concatenation::concat_moves(moves_concatenation&& src){
    if(m.size() < src.m.size())
        std::swap(m, src.m);
    if(m.size() + src.m.size() > m.capacity())
        m.reserve((m.size() + src.m.size())*2);
    if(this == &src)
        for(uint i = 0;i<src.m.size();++i)
            m.push_back(src.m[i]);
    else if(src.m.size() == 0);
    else
        for(auto& el: src.m)
            m.push_back(std::move(el));
    return *this;
}

moves_concatenation& moves_concatenation::increment(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].increment();
    return *this;
}

moves_concatenation& moves_concatenation::decrement(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].decrement();
    return *this;
}

moves_concatenation& moves_concatenation::set_star(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].set_star();
    return *this;
}

moves_concatenation& moves_concatenation::set_number(uint number_of_repetitions){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    m[0].set_number(number_of_repetitions);
    return *this;
}

uint moves_concatenation::max_number_of_repetitions(void)const{
    uint current_max = 0;
    for(const auto& el: m)
        current_max = std::max(current_max, el.max_number_of_repetitions());
    return current_max;
}

void moves_concatenation::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    const std::string& move_name,
    bool uppercase_player,
    const std::string& start_x_name,
    const std::string& start_y_name,
    const std::string& end_x_name,
    const std::string& end_y_name,
    uint& next_free_id)const{
    std::string current_start_x_name = start_x_name;
    std::string current_start_y_name = start_y_name;
    std::string current_end_x_name, current_end_y_name;
    for(uint i=0;i<m.size()-1;++i){
        current_end_x_name = start_x_name+std::to_string(i+1);
        current_end_y_name = start_y_name+std::to_string(i+1);
        m[i].write_as_gdl(
            out,
            additional_moves_to_write,
            additional_bracketed_moves,
            move_name,
            uppercase_player,
            current_start_x_name,
            current_start_y_name,
            current_end_x_name,
            current_end_y_name,
            next_free_id);
        current_start_x_name = current_end_x_name;
        current_start_y_name = current_end_y_name;
    }
    if(!m.empty()) // this if is theoretically unnecessary, but just to be sure...
        m.back().write_as_gdl(
            out,
            additional_moves_to_write,
            additional_bracketed_moves,
            move_name,
            uppercase_player,
            current_start_x_name,
            current_start_y_name,
            end_x_name,
            end_y_name,
            next_free_id);
}

bracketed_move::bracketed_move(const single_move& src):
sum(false),
number_of_repetitions(1),
single_m(new single_move(src)){
}

bracketed_move::bracketed_move(const moves_sum& src):
sum(true),
number_of_repetitions(1),
m_sum(new moves_sum(src)){
}

bracketed_move::bracketed_move(single_move&& src):
sum(false),
number_of_repetitions(1),
single_m(new single_move(std::move(src))){ // nothing more efficient can be done here
}

bracketed_move::bracketed_move(moves_sum&& src):
sum(true),
number_of_repetitions(1),
m_sum(new moves_sum(std::move(src))){
}

bracketed_move::bracketed_move(const bracketed_move& src):
sum(src.sum),
number_of_repetitions(src.number_of_repetitions){
    if(sum)
        m_sum = new moves_sum(*src.m_sum);
    else
        single_m = new single_move(*src.single_m);
}

bracketed_move& bracketed_move::operator=(const bracketed_move& src){
    if(this == &src)
        return *this;
    if(sum)
        delete m_sum;
    else
        delete single_m;
    sum = src.sum;
    if(sum)
        m_sum = new moves_sum(*src.m_sum);
    else
        single_m = new single_move(*src.single_m);
    number_of_repetitions = src.number_of_repetitions;
    return *this;
}

bracketed_move::bracketed_move(bracketed_move&& src):
sum(src.sum),
number_of_repetitions(src.number_of_repetitions){
    if(sum){
        m_sum = src.m_sum;
        src.m_sum = nullptr;
    }
    else{
        single_m = src.single_m;
        src.single_m = nullptr;
    }
}

bracketed_move& bracketed_move::operator=(bracketed_move&& src){
    if(this == &src)
        return *this;
    // temp = this:
    bool temp_sum = sum;
    moves_sum* temp_m_sum = nullptr;
    single_move* temp_single_m = nullptr;
    if(temp_sum)
        temp_m_sum = m_sum;
    else
        temp_single_m = single_m;
    //this = src:
    sum = src.sum;
    if(src.sum)
        m_sum = src.m_sum;
    else
        single_m = src.single_m;
    // src = temp:
    src.sum = temp_sum;
    if(temp_sum)
        src.m_sum = temp_m_sum;
    else
        src.single_m = temp_single_m;
    number_of_repetitions = src.number_of_repetitions;
    return *this;
}

bracketed_move::~bracketed_move(void){
    if(sum)
        delete m_sum;
    else
        delete single_m;
}

bracketed_move& bracketed_move::increment(void){
    if(number_of_repetitions > 0)
        ++number_of_repetitions;
    return *this;
}

bracketed_move& bracketed_move::decrement(void){
    if(number_of_repetitions > 1)
        --number_of_repetitions;
    return *this;
}

bracketed_move& bracketed_move::set_star(void){
    number_of_repetitions = 0;
    return *this;
}

bracketed_move& bracketed_move::set_number(uint number){
    number_of_repetitions = number;
    return *this;
}

uint bracketed_move::max_number_of_repetitions(void)const{
    return std::max(number_of_repetitions, (sum ? m_sum->max_number_of_repetitions() : 0));
}

void bracketed_move::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    const std::string& move_name,
    bool uppercase_player,
    const std::string& start_x_name,
    const std::string& start_y_name,
    const std::string& end_x_name,
    const std::string& end_y_name,
    uint& next_free_id)const{
    if(number_of_repetitions == 1) // if the move is repeated only once, creating another predicate is not worth the effort
        write_one_repetition(
            out,
            additional_moves_to_write,
            move_name,
            uppercase_player,
            start_x_name,
            start_y_name,
            end_x_name,
            end_y_name,
            next_free_id);
    else{
        out<<"\n\t("<<move_name<<next_free_id<<" ?"<<start_x_name<<" ?"<<start_y_name<<" ?"<<end_x_name<<" ?"<<end_y_name<<")";
        additional_bracketed_moves.push_back(std::make_pair(next_free_id++, this));
    }
}

void bracketed_move::write_one_repetition(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    const std::string& move_name,
    bool uppercase_player,
    const std::string& start_x_name,
    const std::string& start_y_name,
    const std::string& end_x_name,
    const std::string& end_y_name,
    uint& next_free_id)const{
    if(sum){
        out<<"\n\t("<<move_name<<next_free_id<<" ?"<<start_x_name<<" ?"<<start_y_name<<" ?"<<end_x_name<<" ?"<<end_y_name<<")";
        additional_moves_to_write.push_back(std::make_pair(next_free_id++, m_sum));
    }
    else
        single_m->write_as_gdl(
            out,
            uppercase_player,
            start_x_name,
            start_y_name,
            end_x_name,
            end_y_name);
}

void bracketed_move::write_freestanding_predicate(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    const std::string& move_name,
    uint current_id,
    bool uppercase_player,
    uint& next_free_id)const{
    if(number_of_repetitions > 0){
        out<<"(<= ("<<move_name<<current_id<<" ?xin ?yin ?xout ?yout)\n\t("<<move_name<<current_id<<"helper ?xin ?yin ?xout ?yout "<<number_of_repetitions<<"))\n";
        out<<"(<= ("<<move_name<<current_id<<"helper ?x ?y ?x ?y 0)\n\t(file ?x)\n\t(rank ?y))\n";
        out<<"(<= ("<<move_name<<current_id<<"helper ?xin ?yin ?xout ?yout ?n)";
        out<<"\n\t(movesSucc ?prevn ?n)";
        write_one_repetition(
            out,
            additional_moves_to_write,
            move_name,
            uppercase_player,
            "xin",
            "yin",
            "nextx",
            "nexty",
            next_free_id);
        out<<"\n\t("<<move_name<<current_id<<"helper ?nextx ?nexty ?xout ?yout ?prevn))\n\n";
    }
    else{ // star
        out<<"(<= ("<<move_name<<current_id<<" ?x ?y ?x ?y)\n\t(file ?x)\n\t(rank ?y))\n";
        out<<"(<= ("<<move_name<<current_id<<" ?xin ?yin ?xout ?yout)";
        write_one_repetition(
            out,
            additional_moves_to_write,
            move_name,
            uppercase_player,
            "xin",
            "yin",
            "nextx",
            "nexty",
            next_free_id);
        out<<"\n\t("<<move_name<<current_id<<" ?nextx ?nexty ?xout ?yout))\n\n";
    }
}

std::string single_move::to_string(void)const{
    return std::to_string(x_delta)+','+std::to_string(y_delta)+','+(kind == empty ? 'e' : (kind == enemy ? 'p' : 'w'));
}

std::string moves_sum::to_string(void)const{
    std::string result;
    for(uint i=0;i<m.size()-1;++i)
        result += m[i].to_string() + '+';
    if(m.size() > 0)
        result += m.back().to_string();
    return result;
}

std::string moves_concatenation::to_string(void)const{
    std::string result;
    for(uint i=0;i<m.size();++i)
        result += m[i].to_string();
    return result;
}

std::string bracketed_move::to_string(void)const{
    return "(" + (sum ? m_sum->to_string() : single_m->to_string()) + ")^" + (number_of_repetitions == 0 ? "*" : std::to_string(number_of_repetitions));
}
