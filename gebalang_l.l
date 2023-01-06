%option yylineno
%{
#include "gebalang_y.hpp"
#include <stdlib.h>
int yylex();
%}
%%

%x WORD
%x WHITESPACE
%x BLOCK_COMMENT

%%

"PROGRAM"  { return PROGRAM; }
"PROCEDURE" { return PROCEDURE; }
"IS"   { return IS; }
"VAR"  { return VAR; }
"BEGIN"    { return _BEGIN; }
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


"0|[1-9][0-9]*"     { 
                        parseTreeNode* node;
                        token t=num;
                        (node->tokens)::insert(t);
                        (node->params)::insert(yytext);
                        yylval=node; return num; 
                    }
[a-zA-Z][a-zA-Z0-9]*    { 
                            parseTreeNode* node;
                            token t=identifier;
                            (node->tokens)::insert(t);
                            (node->params)::insert(yytext);
                            yylval=node; return NUM; 
                        }

[ \t\n]   { }

[ \t]+ 	;

\n	{ return END; }
.   return ERROR;
%%