%option noyywrap
%option yylineno
%{
//#define YYSTYPE std::shared_ptr<ParseTreeNode>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include "utils.hpp"
#include "yacc.hpp"
%}
%x COMMENT
%%
\[   BEGIN(COMMENT);
"PROGRAM"  {return PROGRAM; }
"PROCEDURE" { return PROCEDURE; }
"IS"   { return IS; }
"VAR"     { return VAR; }
"BEGIN"    {return _BEGIN; }
"END"  { return END; }
"IF"   { return IF; }
"THEN" { return THEN; }
"ELSE" { return ELSE; }
"ENDIF"    { return ENDIF; }
"WHILE"    { return WHILE; }
"DO"   { return DO; }
"ENDWHILE" { return ENDWHILE; }
"REPEAT"   { return REPEAT; }
"UNTIL"    { return UNTIL; }
"READ" { return READ; }
"WRITE"    { return WRITE; }
";"    { return SEMI; }
":="   { return ASSIGN; }
"+"    { return PLUS; }
"-"    { return MINUS; }
"*"    { return MULT; }
"/"    { return DIV; }
"%"    { return MOD; }
"!="   { return NEQ; }
"="    { return EQ; }
">"    { return GT; }
"<"    { return LT; }
">="   { return GTE; }
"<="   { return LTE; }
","     {return COMA;}
"("     {return OPEN_BRACKET;}
")"     {return CLOSE_BRACKET;}

0|[1-9][0-9]*     { 
                        std::shared_ptr<ParseTreeNode> node = std::make_shared<ParseTreeNode>();
                        token t=t_num; 
                        node->tokens.push_back(t);
                        node->params.push_back(yytext);
                        yylval=node; return NUM; 
                    }
[_a-z]+                 { 
                            std::shared_ptr<ParseTreeNode> node = std::make_shared<ParseTreeNode>();
                            token t=t_identifier;
                            node->tokens.push_back(t);
                            node->params.push_back(yytext);
                            //std::cout << "returnED IDENTIFIER " << yytext << std::endl;

                            yylval=node; return IDENTIFIER; 
                        }

[ \t\n]   { }

[ \t]+ 	;

.    {std::cout << yytext << " error2\n";return ERROR;}

<COMMENT>{
    \] BEGIN(INITIAL);
    \n
    .
}
%%
