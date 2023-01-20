#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   //zmienic w bisonie list->vector
#include <string>
#include <map>

#define NULL_VALUE     {-1,-1}
#define TRUE_CONDITION {NULL_VALUE, c_true}
#define NON_COND_EDGE(end)   {end, TRUE_CONDITION}

std::vector<Line> code;

std::vector<std::string, Value> variables;

std::vector<std::string> program;

int jumps_no;

int jump_to_merge=0;

int freeMem;

enum token{
        PROCEDURE,
        IS,
        VAR,    //zmienic w bisonie
        _BEGIN,
        END,
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
        ASSIGN,
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
    std::vector<token>            tokens;
    std::vector<std::string>      params;
    ParseTreeNode*               parent;
    std::vector<ParseTreeNode*>    childs;
} ParseTreeNode;

typedef struct ProcedureInfo{
    ParseTreeNode* procedureTree;
    int startLine;
    int used;        //jesli tylko raz, to lepiej wkleic
}ProcedureInfo;

enum Operator{
    o_null,
    plus,
    minus,
    mult,
    div,
    mod
};

enum Comparission{
    c_null,
    neq,
    eq,
    gt,
    gte,
    lt,
    lte,
    //
    c_true,
    c_false
};

enum Operation{
    OP_NULL,
    READ,
    WRITE,
    SEMI,
    ASSIGN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,
    CALL_PROC,
    //compiler utils operations
    HALF,
    DOUBLE,
    ALLOC,
    END,
    //do ifa i petli
    DISPRESSION_BEGIN,
    DISPRESSION_END,
    procedure
};

enum ValueType{
    num,
    identifier
};

/*class Value
{
    public:
    int value;
}

class Variable : Value
{
    public:
    int adress;
}*/

typedef struct Value{
    //std::string name;
    int     adress = -1;    //-1 for numbers
    int     value;          //-1 - unpredictable
    std::string name;
}Value;

typedef struct Line{
    std::string instruction;
    //void*       parameter;  //optional
    int jump_number;
}Line;
/*typedef union Value{
    Variable,
    int
}Value;*/
typedef struct Expression{
    std::vector<Value> values;
    Operator operator;
}Expression;

typedef struct Instruction{
    Operation operation;
    //zmienic na Value value
    std::vector<Value> values;
    Expression      expression;
}Instruction;

typedef struct Condition{
    Value values[2];
    Comparission comparission;
}Condition;



typedef struct Edge{
    InstructionVertex*  end;
    Condition condition;
}ConditionalEdge;

/*typedef union InstructionEdge{
    Edge conditionalEdge,
    InstructionVertex* normalEdge
}InstructionEdge;*/

typedef struct InstructionVertex{
    int     begin;
    Instruction                   instruction;
    std::vector<Edge>    edges;
    std::vector<std::string>        code;
}InstructionVertex;


Value readValue(ParseTreeNode* valueNode);
Value declareValue(ParseTreeNode* valueNode);
Value declareValues(ParseTreeNode* valuesNode);
Condition readExpression(ParseTreeNode* expressionTree);
Condition readCondition(ParseTreeNode* conditionTree);
InstructionVertex* buildCommandFlowChart(ParseTreeNode* commandTree, InstructionVertex* & beginVertex, InstructionVertex* & endVertex);
InstructionVertex* buildCommandsFlowChart(ParseTreeNode* commandsTree, InstructionVertex* & beginVertex, InstructionVertex* & endVertex);
InstructionVertex buildFlowChart(ParseTreeNode* programTree);

//wynik wszystkich operacji trzymany jest w akumulatorze
//przy optymalizacji predictable variables dodac aktualizacje value po kazdym store
std::vector<std::string> addValues(Value val1, Value val2);
std::vector<std::string> subValues(Value val1, Value val2);
std::vector<std::string> doubleValue(Value val);

//transformacja mult vertex na algorytm ruskich chlopow
InstructionVertex* transformMultVertex( InstructionVertex* & multVertex);

void translateAssignVertex(InstructionVertex* & assignVertex);
//zwracamy pointer na jumpa
Line* translateConditionalEdge(ConditionalEdge* edge)
//zwracamy dispression end
InstructionVertex* translateDispressionVertex(InstructionVertex* & dispressionVertex);


//w callu procedury zostawiamy puste sety i uzupelniamy je w ostatniej fazie kompilacji

#endif  