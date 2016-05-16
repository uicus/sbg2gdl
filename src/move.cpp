#include"move.hpp"
#include"gdl_constants.hpp"

reuse_tool::reuse_tool(void):
known_sums(),
known_bracketed(),
existing_concatenations(){
}

reuse_tool::reuse_tool(const reuse_tool& src):
known_sums(src.known_sums),
known_bracketed(src.known_bracketed),
existing_concatenations(src.existing_concatenations){
}

reuse_tool& reuse_tool::operator=(const reuse_tool& src){
    if(this == &src)
        return *this;
    known_sums = src.known_sums;
    known_bracketed = src.known_bracketed;
    existing_concatenations = src.existing_concatenations;
    return *this;
}

reuse_tool::reuse_tool(reuse_tool&& src):
known_sums(std::move(src.known_sums)),
known_bracketed(std::move(src.known_bracketed)),
existing_concatenations(std::move(src.existing_concatenations)){
}

reuse_tool& reuse_tool::operator=(reuse_tool&& src){
    if(this == &src)
        return *this;
    known_sums = std::move(src.known_sums);
    known_bracketed = std::move(src.known_bracketed);
    existing_concatenations = std::move(existing_concatenations);
    return *this;
}

reuse_tool::~reuse_tool(void){
}

void reuse_tool::insert_new_concatenation(std::vector<bracketed_move>&& src){
    auto it = existing_concatenations.find(src);
    if(it != existing_concatenations.end())
        ++(it->second);
    else
        existing_concatenations.insert(std::pair<std::vector<bracketed_move>, uint>(src, 1));
}

std::string reuse_tool::get_or_insert(
const moves_sum& src,
std::vector<std::pair<uint, const moves_sum*>>& additional_moves_to_write,
const std::string& move_name,
uint& id){
    const auto it = known_sums.find(src);
    if(it != known_sums.end())
        return it->second;
    else{
        std::string new_name = move_name+std::to_string(id);
        additional_moves_to_write.push_back(std::make_pair(id++, &src));
        known_sums.insert(std::pair<moves_sum, std::string>(src, new_name));
        return new_name;
    }
}

std::string reuse_tool::get_or_insert(
const bracketed_move& src,
std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
const std::string& move_name,
uint& id){
    const auto it = known_bracketed.find(src);
    if(it != known_bracketed.end())
        return it->second;
    else{
        std::string new_name = move_name+std::to_string(id);
        additional_bracketed_moves.push_back(std::make_pair(id++, &src));
        known_bracketed.insert(std::pair<bracketed_move, std::string>(src, new_name));
        return new_name;
    }
}

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

bool single_move::operator==(const single_move& m2)const{
    return x_delta == m2.x_delta && y_delta == m2.y_delta && kind == m2.kind;
}

bool single_move::operator<(const single_move& m2)const{
    return x_delta < m2.x_delta
        || (x_delta == m2.x_delta && y_delta < m2.y_delta)
        || (x_delta == m2.x_delta && y_delta == m2.y_delta && kind < m2.kind);
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

bool moves_sum::operator==(const moves_sum& m2)const{
    if(m.size() != m2.m.size())
        return false;
    else{
        for(auto i1=m.begin(), i2=m2.m.begin();i1!=m.end() && i2!=m2.m.end();++i1,++i2)
            if(!(*i1 == *i2))
                return false;
        return true;
    }
}

bool moves_sum::operator<(const moves_sum& m2)const{
    for(auto i1=m.begin(), i2=m2.m.begin();i1!=m.end() && i2!=m2.m.end();++i1,++i2){
        if(*i1 < *i2)
            return true;
        if(*i2 < *i1)
            return false;
    }
    return m.size()<m2.m.size();
}

void moves_sum::wrap_in_brackets(void){
    moves_sum this_move;
    this_move.m = std::move(m);
    bracketed_move bm(std::move(this_move));
    moves_concatenation mc;
    mc.append(std::move(bm));
    m = std::set<moves_concatenation>{mc};
}

moves_sum& moves_sum::append(moves_concatenation&& mc){
    m.insert(std::move(mc));
    return *this;
}

moves_sum& moves_sum::add_moves(moves_sum&& src){
    if(m.size() < src.m.size())
        std::swap(m, src.m);
    if(this != &src && src.m.size() != 0){
        for(auto& el: src.m)
            m.insert(std::move(el));
    }
    return *this;
}

moves_sum& moves_sum::concat_moves(moves_sum&& src){
    if(m.size() < src.m.size())
        std::swap(m, src.m); // we assure that this >= src
    //if(m.size() + src.m.size() > m.capacity() && m.size() > 0)
    //    m.reserve((m.size() + src.m.size())*2);
    //if(this == &src)
    //    for(uint i = 0;i<src.m.size();++i)
    //        m.push_back(src.m[i]);
    if(m.size() == 0);
    else if(src.m.size() == 0)
        m.clear();
    else if(m.size() == 1){
        moves_concatenation result = *m.begin();
        result.concat_moves(moves_concatenation(*src.m.begin()));
        //m.clear();
        m = std::set<moves_concatenation>{result};
    }
    else{
        if(src.m.size() == 1){
            std::vector<moves_concatenation> temp(m.size());
            std::copy(m.begin(),m.end(),temp.begin());
            for(auto& el: temp)
                el.concat_moves(moves_concatenation(*src.m.begin()));
            m = std::set<moves_concatenation>(std::make_move_iterator(temp.begin()),std::make_move_iterator(temp.end()));
        }
        else{
            wrap_in_brackets();
            bracketed_move bm2(this == &src ? src : std::move(src));
            moves_concatenation result = *m.begin();
            result.append(std::move(bm2));
            //m.clear();
            m = std::set<moves_concatenation>{result};
        }
    }
    return *this;
}

moves_sum& moves_sum::increment(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    moves_concatenation result = *m.begin();
    result.increment();
    m = std::set<moves_concatenation>{result};
    return *this;
}

moves_sum& moves_sum::decrement(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    moves_concatenation result = *m.begin();
    result.decrement();
    m = std::set<moves_concatenation>{result};
    return *this;
}

moves_sum& moves_sum::set_star(void){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    moves_concatenation result = *m.begin();
    result.set_star();
    m = std::set<moves_concatenation>{result};
    return *this;
}

moves_sum& moves_sum::set_number(uint number_of_repetitions){
    if(m.size() == 0)
        return *this;
    if(m.size() > 1)
        wrap_in_brackets();
    moves_concatenation result = *m.begin();
    result.set_number(number_of_repetitions);
    m = std::set<moves_concatenation>{result};
    return *this;
}

uint moves_sum::max_number_of_repetitions(uint treat_star_as)const{
    uint current_max = 0;
    for(const auto& el: m)
        current_max = std::max(current_max, el.max_number_of_repetitions(treat_star_as));
    return current_max;
}

void moves_sum::scan_for_concatenations(reuse_tool& known)const{
    for(const auto& el: m)
        el.scan_for_concatenations(known);
}

void moves_sum::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    reuse_tool& known,
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
            known,
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

bool moves_concatenation::operator==(const moves_concatenation& m2)const{
    if(m.size() != m2.m.size())
        return false;
    else{
        for(uint i=0;i<m.size();++i)
            if(!(m[i] == m2.m[i]))
                return false;
        return true;
    }
}

bool moves_concatenation::operator<(const moves_concatenation& m2)const{
    for(auto i1=m.begin(), i2=m2.m.begin();i1!=m.end() && i2!=m2.m.end();++i1,++i2){
        if(*i1 < *i2)
            return true;
        if(*i2 < *i1)
            return false;
    }
    return m.size()<m2.m.size();
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
    if(m.size() > 0 && m.back().can_be_merged(bm))
        m.back().set_number(m.back().get_number_of_repetitions()+bm.get_number_of_repetitions());
    else
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

uint moves_concatenation::max_number_of_repetitions(uint treat_star_as)const{
    uint current_max = 0;
    for(const auto& el: m)
        current_max = std::max(current_max, el.max_number_of_repetitions(treat_star_as));
    return current_max;
}

void moves_concatenation::scan_for_concatenations(reuse_tool& known)const{
    for(uint i=0;i+1<m.size();++i)
        for(uint j=i+2;j<m.size();++j)
            known.insert_new_concatenation(std::vector<bracketed_move>(m.begin()+i, m.begin()+j));
    for(const auto& el: m)
        el.scan_for_concatenations(known);
}

void moves_concatenation::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    reuse_tool& known,
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
    for(uint i=0;i+1<m.size();++i){
        current_end_x_name = start_x_name+std::to_string(i+1);
        current_end_y_name = start_y_name+std::to_string(i+1);
        m[i].write_as_gdl(
            out,
            additional_moves_to_write,
            additional_bracketed_moves,
            known,
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
            known,
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

bool bracketed_move::operator==(const bracketed_move& m2)const{
    if(sum != m2.sum)
        return false;
    else{
        if(sum)
            return number_of_repetitions == m2.number_of_repetitions && *m_sum == *(m2.m_sum);
        else
            return number_of_repetitions == m2.number_of_repetitions && *single_m == *(m2.single_m);
    }
}

bool bracketed_move::operator<(const bracketed_move& m2)const{
    if(sum && m2.sum)
        return *m_sum < *(m2.m_sum) || (*m_sum == *(m2.m_sum) && number_of_repetitions < m2.number_of_repetitions);
    if(!sum && !m2.sum)
        return *single_m < *(m2.single_m) || (*single_m == *(m2.single_m) && number_of_repetitions < m2.number_of_repetitions);
    return !sum;
}

bool bracketed_move::can_be_merged(const bracketed_move& m2)const{
    if(sum != m2.sum || number_of_repetitions == 0 || m2.number_of_repetitions == 0)
        return false;
    else{
        if(sum)
            return *m_sum == *(m2.m_sum);
        else
            return *single_m == *(m2.single_m);
    }
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

uint bracketed_move::get_number_of_repetitions(void)const{
    return number_of_repetitions;
}

uint bracketed_move::max_number_of_repetitions(uint treat_star_as)const{
    return std::max(number_of_repetitions == 0 ? treat_star_as : number_of_repetitions, (sum ? m_sum->max_number_of_repetitions(treat_star_as) : 0));
}

void bracketed_move::scan_for_concatenations(reuse_tool& known)const{
    if(sum)
        m_sum->scan_for_concatenations(known);
}

void bracketed_move::write_as_gdl(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    std::vector<std::pair<uint, const bracketed_move*>>& additional_bracketed_moves,
    reuse_tool& known,
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
            known,
            move_name,
            uppercase_player,
            start_x_name,
            start_y_name,
            end_x_name,
            end_y_name,
            next_free_id);
    else
        out<<"\n\t("<<known.get_or_insert(*this, additional_bracketed_moves, move_name, next_free_id)<<" ?"<<start_x_name<<" ?"<<start_y_name<<" ?"<<end_x_name<<" ?"<<end_y_name<<")";
}

void bracketed_move::write_one_repetition(
    std::ofstream& out,
    std::vector<std::pair<uint, const move*>>& additional_moves_to_write,
    reuse_tool& known,
    const std::string& move_name,
    bool uppercase_player,
    const std::string& start_x_name,
    const std::string& start_y_name,
    const std::string& end_x_name,
    const std::string& end_y_name,
    uint& next_free_id)const{
    if(sum)
        out<<"\n\t("<<known.get_or_insert(*m_sum, additional_moves_to_write, move_name, next_free_id)<<" ?"<<start_x_name<<" ?"<<start_y_name<<" ?"<<end_x_name<<" ?"<<end_y_name<<")";
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
    reuse_tool& known,
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
            known,
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
            known,
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
    for(auto i = m.begin();i!=m.end()--;++i)
        result += i->to_string() + '+';
    if(m.size() > 0)
        result += m.end()->to_string();
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
