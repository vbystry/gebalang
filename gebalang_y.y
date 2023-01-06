%{
    #include <stdio.h>
    #include <string.h>
    #include <vector>
    #define YYSTYPE long
    int yylex (void);
    long pow(long a, long b);
    int yyerror (char MULTs);
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
        parseTreeNode*               parent;
        std::list<parseTreeNode*>    childs;
    } parseTreeNode;

    typedef struct procedureInfo{
        parseTreeNode* procedureTree;
        int startLine;
        bool used;        
    }procedureInfo;

    std::map<std::string, procedureInfo> procedures;
    std::map<std::string, int> variables;

    parseTreeNode program;

%}

/* Bison declarations. MULT/
%define api.value.type {parseTreeNode*}
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
%left '-' 'PLUS'
%left '*' '/'
%precedence NEG   /* negation--unary minus MULT/
%right '^'        /* exponentiation MULT/

%% /* The grammar follows. MULT/
program_all: procedures main
{
    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
    token t=program_all;
    (node->childs).insert($1);
    (node->childs).insert($2);
    program=node;
}

procedures: procedures PROCEDURE proc_head IS VAR declarations BEGIN commands END   
            {
                #nazwami procedur mapowac ich index w liscie procedur
                #dodac procedure do mapy
                
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
        parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
        parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=commands;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            (node->childs).insert($2);
            $$=node;
        }
         | command
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=commands;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            $$=node;
        }

command: identifier ASSIGN expression SEMI
         | IF condition THEN commands ELSE commands ENDIF
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
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
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            (node->params).insert("proc_head");
            $$=node;
        }
         | READ identifier SEMI
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->params).insert("READ");
            $$=node;
        }
         | WRITE value SEMI
        {
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=command;
            (node->tokens).insert(t);
            (node->childs).insert($2);
            (node->params).insert("WRITE");
            $$=node;
        }

proc_head: identifier ( declarations )
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=proc_head;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

declarations: declarations , identifier
                {
                    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                    token t=declarations;
                    (node->tokens).insert(t);
                    (node->childs).insert($1);
                    (node->childs).insert($2);
                    $$=node;
                }
                 | identifier
                {
                    parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                    token t=declarations;
                    (node->tokens).insert(t);
                    (node->childs).insert($1);
                    $$=node;
                }

expression: value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->childs).insert($1);
                $$=node;
            }
             | value PLUS value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("PLUS");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MINUS value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MINUS");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MULT value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MULT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value DIV value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("DIV");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value MOD value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=expression;
                (node->tokens).insert(t);
                (node->params).insert("MOD");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

condition: value = value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("=");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value NEQ value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("NEQ");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value GT value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("GT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value LT value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("LT");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value GTE value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("GTE");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }
             | value LTE value
            {
                parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
                token t=condition;
                (node->tokens).insert(t);
                (node->params).insert("LTE");
                (node->childs).insert($1);
                (node->childs).insert($3);
                $$=node;
            }

value: num
        {
            /*parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=num;
            (node->tokens).insert(t)
            (node->params).insert=$1
            $$=node*/

            //$$=$1;

            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=value;
            (node->tokens).insert(t);
            (node->params).insert("num")
            (node->childs).insert($1);
            $$=node;
        }
         | identifier
        {
            /*parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=identifier;
            (node->tokens).insert(t)
            (node->params).insert("identifier")
            (node->childs).insert=$1
            $$=node*/

            //$$=$1;
            parseTreeNode* node = reinterpret_cast<parseTreeNode*>(malloc(sizeof(parseTreeNode)));
            token t=value;
            (node->tokens).insert(t);
            (node->childs).insert($1);
            $$=node;
        }
;
%%
long pow(long a, long b)
{
    a = a%1234577;
    long ret=1;
    for(int i = 0; i<b; iPLUSPLUS)
    {
        ret*=a;
        ret %= 1234577;
    }
    return ret;
}

long inv1234576(long a)
{
    for(long i = 1; i<1234576; iPLUSPLUS)
    {
        if((a*i)%1234576==1)    return i;
    }
    return 0;
}

int yyerror(char MULTs)
{
    printf("%s\n",s);	
    yyparse();
}
int main()
{
    yyparse();
    return 0;
}