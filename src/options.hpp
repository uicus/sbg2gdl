#ifndef OPTIONS
#define OPTIONS

#include<string>
#include<exception>

#include"types.hpp"

class wrong_argument_error : public std::exception{
        std::string description;
    public:
        wrong_argument_error(void);
        wrong_argument_error(const std::string& description);
        virtual ~wrong_argument_error(void);

        virtual const char* what(void)const noexcept;
};

class options{
        bool just_verify : 1;
        bool optimise_domain : 1;
        bool show_warnings : 1;
        bool warnings_as_errors : 1;
        bool prolog_safe : 1;
        bool logarithmic_c : 1;
        bool share_c : 1;
        bool share_r : 1;
        bool share_s : 1;
        bool skip_i : 1;
        bool skip_b : 1;
        bool skip_c : 1;
        std::string output_name;
    public:
        options(void);
        options(uint number_of_args, const char** args)throw(wrong_argument_error);

        bool verifying(void)const;
        bool domain_optimising(void)const;
        bool showing_warnings(void)const;
        bool escalating_warnings(void)const;
        bool allowed_unsafe(void)const;
        bool logarithmic_counter(void)const;
        bool share_concatenations(void)const;
        bool share_repetitions(void)const;
        bool share_sums(void)const;
        bool skip_input(void)const;
        bool skip_base(void)const;
        bool skip_comments(void)const;
        const std::string& output_file(void)const;
};

#endif
