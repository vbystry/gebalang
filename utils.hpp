#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   //zmienic w bisonie list->vector
#include <string>
#include <map>

std::<std::string, Value> variables;

std::vector<std::string> program;

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
    null,
    plus,
    minus,
    mult,
    div,
    mod
}

enum Comparission{
    null,
    neq,
    eq,
    gt,
    gte,
    lt,
    lte
};

enum Operation{
    
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
    //do ifa i petli
    DISPRESSION_BEGIN,
    DISPRESSION_END
};

enum ValueType{
    num,
    identifier
}

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
}Variable;

/*typedef union Value{
    Variable,
    int
}Value;*/

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

typedef struct Expression{
    std::vector<Value> values;
    Operator operator;
}Expression;

typedef struct ConditionalEdge{
    InstructionVertex*  end;
    Condition condition;
}ConditionalEdge;

typedef union InstructionEdge{
    ConditionalEdge,
    InstructionVertex*
}InstructionEdge;

typedef struct InstructionVertex{
    int     begin;
    Instruction                   instruction;
    std::vector<InstructionEdge>    edges;
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

std::vector<std::string> translateAssignVertex(InstructionVertex* & assignVertex);

#endif  