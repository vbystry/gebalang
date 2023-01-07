#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   //zmienic w bisonie list->vector
#include <string>
#include <map>

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
    bool used;        
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
};

enum ValueType{
    num,
    identifier
}

typedef struct Variable{
    //std::string name;
    int     adress;
    int     value;  //-1 - unpredictable
}Variable;

std::map<std::string, Variable> variables;


typedef union Value{
    Variable,
    int
}Value;

typedef struct Instruction{
    Operation operation;
    std::vector<Value> values;
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

Value readValue(ParseTreeNode* valueNode)
{
    Value value;
    switch(valueNode->tokens[0])
    {
        case num:
            value=std::atoi(valueNode->childs[0]->params[0]);
            break;
        case identifier:
            value=variables[valueNode->childs[0]->params[0]];
            //error jak undef
            break;
        default:
            //error, impossible
    }
}

Value declareValue(ParseTreeNode* valueNode)
{
    //
}

Condition readExpression(ParseTreeNode* expressionTree)
{
    //check if node is expression

    Expression expr;

    switch(conditionTree->params[0])
    {
        case "NULL":
            expr.Operator=null;
            break;
        case "PLUS":
            expr.Operator=plus;
            break;
        case "MINUS":
            expr.Operator=minus;
            break;
        case "DIV":
            expr.Operator=div;
            break;
        case "MULT":
            expr.Operator=mult;
            break;
        case "MOD":
            expr.Operator=mod;
            break;
        default:
            //error, imposibble
    }

    //readValues
    expr.values[0]=readValue(conditionTree->childs[0]);
    if(expr.Operator!=null)
        expr.values[1]=readValue(conditionTree->childs[1]);

    return expr;
}

Condition readCondition(ParseTreeNode* conditionTree)
{
    //check if node is condition

    Condition cond;

    switch(conditionTree->params[0])
    {
        case "EQ":
            cond.comparission=eq;
            break;
        case "NEQ":
            cond.comparission=neq;
            break;
        case "GT":
            cond.comparission=gt;
            break;
        case "LT":
            cond.comparission=lt;
            break;
        case "GTE":
            cond.comparission=gte;
            break;
        case "LTE":
            cond.comparission=lte;
            break;
        default:
            //error, imposibble
    }

    //readValues
    cond.values[0]=readValue(conditionTree->childs[0]);
    cond.values[1]=readValue(conditionTree->childs[1]);

    return cond;
}

//takes the beginning, returns the end
InstructionVertex* buildCommandFlowChart(ParseTreeNode* commandTree, InstructionVertex* & beginVertex)
{
    InstructionVertex* commandChart = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));;
    beginVertex->edges.insert(commandChart);

    //InstructionVertex* current = commandChart;
    switch(commandTree->params[0])
    {
        case "IF":
        {
            
        }
    }
}

InstructionVertex buildFlowChart(ParseTreeNode* programTree)
{
    if( programTree->params[2].compare("VAR")  )
    {
        //zadeklaruj zmienne
    }

}