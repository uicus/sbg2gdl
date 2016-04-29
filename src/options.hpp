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
        wrong_argument_error(const wrong_argument_error& source);
        wrong_argument_error& operator=(const wrong_argument_error& source);
        wrong_argument_error(wrong_argument_error&& source);
        wrong_argument_error& operator=(wrong_argument_error&& source);
        virtual ~wrong_argument_error(void);

        virtual const char* what(void)const noexcept;
};

class options{
        bool just_verify : 1;
        bool optimise_domain : 1;
        bool show_warnings : 1;
        bool warnings_as_errors : 1;
        uint optimisation_level : 2;
        std::string output_name;
    public:
        options(void);
        options(uint number_of_args, const char** args)throw(wrong_argument_error);
        options(const options& source);
        options& operator=(const options& source);
        options(options&& source);
        options& operator=(options&& source);
        ~options(void);

        bool verifying(void)const;
        bool domain_optimising(void)const;
        bool showing_warnings(void)const;
        bool escalating_warnings(void)const;
        uint optimising(void)const;
        const std::string& output_file(void)const;
};

#endif
