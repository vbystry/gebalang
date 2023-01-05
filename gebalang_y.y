%{
    #include <stdio.h>
    #include <string.h>
    #include <vector>
    #define YYSTYPE long
    int yylex (void);
    long pow(long a, long b);
    int yyerror (char *s);
    long inv1234576(long a);

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

    typedef struct parseTreeNode{
        std::list<token>            tokens;
        std::list<std::string>      params;
        *parseTreeNode               parent;
        std::list<parseTreeNode*>    childs;
    } parseTreeNode;

    typedef struct procedureInfo{
        parseTreeNode* procedureTree;
        int startLine;
        bool used;        
    }

    std::map<std::string, procedureInfo> procedures;
    std::map<std::string, int> variables;

    parseTreeNode program;

%}

/* Bison declarations. */
%define api.value.type {int}
%token PROCEDURE,
%token IS VAR,
%token BEGIN,
%token END,
%token IS BEGIN,
%token PROGRAM,
%token IF,
%token THEN,
%token ELSE,
%token ENDIF,
%token WHILE,
%token DO,
%token ENDWHILE,
%token REPEAT,
%token UNTIL,
%token READ,
%token WRITE,
%token num,
%token identifier
%token NUM
%token END
%token ERROR
%left '-' '+'
%left '*' '/'
%precedence NEG   /* negation--unary minus */
%right '^'        /* exponentiation */

%% /* The grammar follows. */
program_all: procedures main
{
    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
    token t=program_all;
    (node->childs)::insert($1);
    (node->childs)::insert($2);
    $$=node;
}

procedures: procedures PROCEDURE proc_head IS VAR declarations BEGIN commands END   
            {
                #nazwami procedur mapowac ich index w liscie procedur
                #dodac procedure do mapy
                
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=procedures;
                (node->tokens)::insert(t);
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                (node->childs)::insert($5);
                (node->childs)::insert($7);
                (node->params)::insert("PROCEDURE");
                (node->params)::insert("IS VAR");
                (node->params)::insert("BEGIN");
                (node->params)::insert("END");
                $$=node;
            }
            | procedures PROCEDURE proc_head IS BEGIN commands END
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=procedures;
                (node->tokens)::insert(t);
                (node->childs)::insert($3);
                (node->params)::insert("PROGRAM");
                (node->params)::insert("IS BEGIN");
                (node->params)::insert("END");
                $$=node;
            }
            |
            {
                $$=nullptr;
            }

main: PROGRAM IS VAR declarations BEGIN commands END
    {
        #podpiac pod program
        parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
        token t=main;
        (node->tokens)::insert(t);
        (node->childs)::insert($3);
        (node->childs)::insert($5);
        (node->params)::insert("PROGRAM");
        (node->params)::insert("IS VAR");
        (node->params)::insert("BEGIN");
        (node->params)::insert("END");
        $$=node;
    }
     | PROGRAM IS BEGIN commands END
     {
        parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
        token t=main;
        (node->tokens)::insert(t);
        (node->childs)::insert($3);
        (node->params)::insert("PROGRAM");
        (node->params)::insert("IS BEGIN");
        (node->params)::insert("END");
        $$=node;
    }

commands: commands command
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=commands;
            (node->tokens)::insert(t);
            (node->childs)::insert($1);
            (node->childs)::insert($2);
            $$=node;
        }
         | command
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=commands;
            (node->tokens)::insert(t);
            (node->childs)::insert($1);
            $$=node;
        }

command: identifier := expression ;
         | IF condition THEN commands ELSE commands ENDIF
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->childs)::insert($4);
            (node->childs)::insert($6);
            (node->params)::insert("IF");
            (node->params)::insert("THEN");
            (node->params)::insert("ELSE");
            (node->params)::insert("ENDIF");
            $$=node;
        }
         | IF condition THEN commands ENDIF
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->childs)::insert($4);
            (node->params)::insert("IF");
            (node->params)::insert("THEN");
            (node->params)::insert("ENDIF");
            $$=node;
        }
         | WHILE condition DO commands ENDWHILE
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->childs)::insert($4);
            (node->params)::insert("WHILE");
            (node->params)::insert("DO");
            (node->params)::insert("ENDWHILE");
            $$=node;
        }
         | REPEAT commands UNTIL condition ;
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->childs)::insert($4);
            (node->params)::insert("REPEAT");
            (node->params)::insert("UNTILL");
            $$=node;
        }
         | proc_head ;
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($1);
            (node->params)::insert("proc_head");
            $$=node;
        }
         | READ identifier ;
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->params)::insert("READ");
            $$=node;
        }
         | WRITE value ;
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens)::insert(t);
            (node->childs)::insert($2);
            (node->params)::insert("WRITE");
            $$=node;
        }

proc_head: identifier ( declarations )
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=proc_head;
                (node->tokens)::insert(t);
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }

declarations: declarations , identifier
                {
                    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                    token t=declarations;
                    (node->tokens)::insert(t);
                    (node->childs)::insert($1);
                    (node->childs)::insert($2);
                    $$=node;
                }
                 | identifier
                {
                    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                    token t=declarations;
                    (node->tokens)::insert(t);
                    (node->childs)::insert($1);
                    $$=node;
                }

expression: value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->childs)::insert($1);
                $$=node;
            }
             | value + value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->params)::insert("+");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value - value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->params)::insert("-");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value * value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->params)::insert("*");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value / value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->params)::insert("/");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value % value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens)::insert(t);
                (node->params)::insert("%");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }

condition: value = value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert("=");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value != value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert("!=");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value > value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert(">");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value < value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert("<");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value >= value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert(">=");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }
             | value <= value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens)::insert(t);
                (node->params)::insert("<=");
                (node->childs)::insert($1);
                (node->childs)::insert($3);
                $$=node;
            }

value: num
        {
            /*parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=num;
            (node->tokens)::insert(t)
            (node->params)::insert=$1
            $$=node*/

            //$$=$1;

            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=value;
            (node->tokens)::insert(t);
            (node->childs)::insert($1);
            $$=node;
        }
         | identifier
        {
            /*parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=identifier;
            (node->tokens)::insert(t)
            (node->params)::insert=$1
            $$=node*/

            //$$=$1;
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=value;
            (node->tokens)::insert(t);
            (node->childs)::insert($1);
            $$=node;
        }
;
%%
long pow(long a, long b)
{
    a = a%1234577;
    long ret=1;
    for(int i = 0; i<b; i++)
    {
        ret*=a;
        ret %= 1234577;
    }
    return ret;
}

long inv1234576(long a)
{
    for(long i = 1; i<1234576; i++)
    {
        if((a*i)%1234576==1)    return i;
    }
    return 0;
}

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