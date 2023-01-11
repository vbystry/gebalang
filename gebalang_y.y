%{
    #include <stdio.h>
    #include <string.h>
    #include <vector>
    #define YYSTYPE long
    int yylex (void);
    long pow(long a, long b);
    int yyerror (char *s);

    enum token{
        PROCEDURE,
        IS_VAR,
        _BEGIN,
        END,
        IS_BEGIN,
        PROGRAM,
        IF,
        THEN,
        ELSE,
        ENDIF,
        WHILE,
        DO,
        ENDWHILE,
        REPEAT,
        UNTIL,
        READ,
        WRITE,
        value,
        declarations,
        condition,
        expression,
        num,
        identifier,
        proc_head,
        command
    };

    typedef struct ParseTreeNode{
        std::list<token>            tokens;
        std::list<std::string>      params;
        ParseTreeNode*               parent;
        std::list<ParseTreeNode*>    childs;
    } ParseTreeNode;

    typedef struct procedureInfo{
        ParseTreeNode* procedureTree;
        int startLine;
        bool used;        
    }procedureInfo;

    std::map<std::string, procedureInfo> procedures;
    std::map<std::string, int> variables;

    ParseTreeNode program;

%}

/* Bison declarations. */
%define api.value.type {ParseTreeNode*}
%token PROCEDURE
%token IS 
%token VAR
%token BEGIN
%token END
%token IS BEGIN
%token PROGRAM
%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token REPEAT
%token UNTIL
%token READ
%token WRITE
%token num
%token identifier
%token NUM
%token END
%token ERROR
%token ,
%token
%left '-' 'PLUS'
%left '*' '/'
%precedence NEG   /* negation--unary minus */
%right '^'        /* exponentiation */

%% /* The grammar follows. */
program_all: procedures main
{
    ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
    token t=program_all;
    (node->childs).insert($1);
    (node->childs).insert($2);
    program=node;
}

procedures: procedures PROCEDURE proc_head IS VAR declarations BEGIN commands END   
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=procedures;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                (node->childs).insert($3);
                (node->childs).insert($5);
                (node->childs).insert($7);
                (node->params).insert("PROCEDURE");
                (node->params).insert("IS VAR");
                (node->params).insert("BEGIN");
                (node->params).insert("END");
                $$=node;
            }
            | procedures PROCEDURE proc_head IS BEGIN commands END
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=procedures;
                (node->tokens).insert(t);
                (node->childs).insert($3);
                (node->params).insert("PROGRAM");
                (node->params).insert("IS BEGIN");
                (node->params).insert("END");
                $$=node;
            }
            |
            {
                $$=nullptr;
            }

main: PROGRAM IS VAR declarations BEGIN commands END
    {
        #podpiac pod program
        ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
        token t=main;
        (node->tokens).insert(t);
        (node->childs).insert($3);
        (node->childs).insert($5);
        (node->params).insert("PROGRAM");
        (node->params).insert("IS VAR");
        (node->params).insert("BEGIN");
        (node->params).insert("END");
        $$=node;
    }
     | PROGRAM IS BEGIN commands END
     {
        ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
        token t=main;
        (node->tokens).insert(t);
        (node->childs).insert($3);
        (node->params).insert("PROGRAM");
        (node->params).insert("IS BEGIN");
        (node->params).insert("END");
        $$=node;
    }

commands: commands command
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=commands;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            (node->childs).insert($2);
            $$=node;
        }
         | command
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=commands;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            $$=node;
        }

command: identifier ASSIGN expression SEMI
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=ASSIGN;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
         | IF condition THEN commands ELSE commands ENDIF
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->childs).insert($4);
            (node->childs).insert($6);
            (node->params).insert("IF");
            (node->params).insert("THEN");
            (node->params).insert("ELSE");
            (node->params).insert("ENDIF");
            $$=node;
        }
         | IF condition THEN commands ENDIF
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->childs).insert($4);
            (node->params).insert("IF");
            (node->params).insert("THEN");
            (node->params).insert("ENDIF");
            $$=node;
        }
         | WHILE condition DO commands ENDWHILE
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->childs).insert($4);
            (node->params).insert("WHILE");
            (node->params).insert("DO");
            (node->params).insert("ENDWHILE");
            $$=node;
        }
         | REPEAT commands UNTIL condition SEMI
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->childs).insert($4);
            (node->params).insert("REPEAT");
            (node->params).insert("UNTILL");
            $$=node;
        }
         | proc_head SEMI
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            (node->params).insert("proc_head");
            $$=node;
        }
         | READ identifier SEMI
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->params).insert("READ");
            $$=node;
        }
         | WRITE value SEMI
        {
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->params).insert("WRITE");
            $$=node;
        }

proc_head: identifier ( declarations )
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=proc_head;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

declarations: declarations , identifier
                {
                    ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                    token t=declarations;
                    (node->tokens).insert(t);
                    (node->childs).insert($1);
                    (node->childs).insert($2);
                    $$=node;
                }
                 | identifier
                {
                    ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                    token t=declarations;
                    (node->tokens).insert(t);
                    (node->childs).insert($1);
                    $$=node;
                }

expression: value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                (node->params).insert("NULL");
                $$=node;
            }
             | value PLUS value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("PLUS");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MINUS value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MINUS");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MULT value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MULT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value DIV value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("DIV");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MOD value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MOD");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

condition: value = value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("EQ");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value NEQ value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("NEQ");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value GT value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("GT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value LT value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("LT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value GTE value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("GTE");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value LTE value
            {
                ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("LTE");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

value: num
        {
            /*ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=num;
            (node->tokens).insert(t)
            (node->params).insert=$1
            $$=node*/

            //$$=$1;

            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=value;
            (node->tokens).insert(t);
            (node->params).insert("num")
            (node->childs).insert($1);
            $$=node;
        }
         | identifier
        {
            /*ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=identifier;
            (node->tokens).insert(t)
            (node->params).insert("identifier")
            (node->childs).insert=$1
            $$=node*/

            //$$=$1;
            ParseTreeNode* node = reinterpret_cast<ParseTreeNode*>(malloc(sizeof(ParseTreeNode)));
            token t=value;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            $$=node;
        }
;
%%

int yyerror(char *s)
{
    printf("%s\n",s);	
    yyparse();
}
int main()
{
    yyparse();
    return 0;
}