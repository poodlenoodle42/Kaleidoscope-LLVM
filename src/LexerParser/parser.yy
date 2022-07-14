%{
    #include <string>
    #include <iostream>

    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %sn", s); }
    
%}

%require "3.2"
%language "c++"

%union {
    double number;
    std::string string;
}

%token DEF EXTERN 
%token <string> IDENTIFIER
%token <number> NUMBER

%type <number> num
%start program
%%
program : numbers;
numbers : numbers num {std::cout << $2 << "\n";}
        | num {std::cout << $1 << "\n";};
num : NUMBER {$$ = $1;};
%%