#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   
#include <string>
#include <map>
#include <memory>


enum token{
        t_PROCEDURE,
        t_IS,
        t_VAR,    
        t_BEGIN,
        t_END,
        t_PROGRAM,
        t_IF,
        t_THEN,
        t_ELSE,
        t_ENDIF,
        t_WHILE,
        t_DO,
        t_ENDWHILE,
        t_REPEAT,
        t_UNTIL,
        t_READ,
        t_WRITE,
        t_ASSIGN,
        t_value,
        t_declarations,
        t_condition,
        t_expression,
        t_num,
        t_identifier,
        t_proc_head,
        t_command,
        t_procedures,
        t_main,
        t_program_all,
        t_commands
    };

typedef struct ParseTreeNode{
    std::vector<token>            tokens;
    std::vector<std::string>      params;
    std::shared_ptr<ParseTreeNode>               parent;
    std::vector<std::shared_ptr<ParseTreeNode> >    childs;
} ParseTreeNode;

typedef struct ProcedureInfo{
    std::shared_ptr<ParseTreeNode> procedureTree;
    int startLine;
    int used;        //jesli tylko raz, to lepiej wkleic
}ProcedureInfo;

typedef enum Operator{
    o_null,
    o_plus,
    o_minus,
    o_mult,
    o_div,
    o_mod
}Operator;

enum Comparission{
    c_true,
    c_null,
    neq,
    eq,
    gt,
    gte,
    lt,
    lte,
    //
    
    c_false
};

typedef enum Operation{
    o_OP_NULL,
    o_READ,
    o_WRITE,
    o_SEMI,
    o_ASSIGN,
    o_PLUS,
    o_MINUS,
    o_MULT,
    o_DIV,
    o_MOD,
    o_CALL_PROC,
    //compiler utils operations
    o_HALF,
    o_DOUBLE,
    o_ALLOC,
    o_END,
    //do ifa i petli
    o_DISPRESSION_BEGIN,
    o_DISPRESSION_END,
    o_procedure
} Operation;

enum ValueType{
    num,
    identifier
};

//dodac initialized
typedef struct Value{
    //std::string name;
    int     adress = -1;    //-1 for numbers
    long long     value=-1;          //-1 - unpredictable
    std::string name;
}Value;

extern std::map<std::string, Value> variables;

typedef struct Line{
    std::string instruction;
    int jump_number;
    int jump_jump_number=0;
}Line;

typedef struct Expression{
    std::vector<std::string> values;
    Operator op;

    Value getValue(int i){
        if(values[i].at(0)=='@')
            return variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            Value val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}Expression;

typedef struct Instruction{
    Operation operation;
    std::vector<std::string> values;
    Expression      expression;

    Value getValue(int i){
        if(values[i].at(0)=='@')
            return variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            Value val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}Instruction;

typedef struct Condition{
    std::string values[2];
    Comparission comparission;

    Value getValue(int i){
        if(values[i].at(0)=='@')
            return variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            Value val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}Condition;

struct InstructionVertex;

typedef struct Edge{
    std::shared_ptr<InstructionVertex>  end;
    Condition condition;
}ConditionalEdge;
typedef struct InstructionVertex{
    int     begin;
    Instruction                   instruction;
    std::vector<Edge>    edges;
    std::vector<std::string>        code;
}InstructionVertex;



struct ProcedureValue{
    int     adress = -1;    //-1 for numbers
    long long     value = -1;          //-1 - unpredictable
    int refNo = -1;         //-1 for is var
    std::string name;
    std::string indirectPostfix;    
};
struct ProcedureInstructionVertex;

struct Procedure{
    std::shared_ptr<ParseTreeNode> parseTree;
    std::shared_ptr<ProcedureInstructionVertex> flowChart;
    std::map<std::string, ProcedureValue> variables;
    std::vector< std::string> refVariables;
    int jumpNo;
    int retAdd;
    bool used;
};

typedef struct ProcedureExpression{
    std::vector<std::string> values;
    Operator op;

    ProcedureValue getValue(int i, Procedure procedure){
        if(values[i].at(0)=='@')
            return procedure.variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            ProcedureValue val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}ProcedureExpression;

typedef struct ProcedureInstruction{
    Operation operation;
    std::vector<std::string> values;
    ProcedureExpression      expression;

    ProcedureValue getValue(int i, Procedure procedure){
        if(values[i].at(0)=='@')
            return procedure.variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            ProcedureValue val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}ProcedureInstruction;

typedef struct ProcedureCondition{
    std::string values[2];
    Comparission comparission;

    ProcedureValue getValue(int i, Procedure procedure){
        if(values[i].at(0)=='@')
            return procedure.variables[values[i].substr(1,values[i].size()-1)];
        else
        {
            ProcedureValue val;
            val.value=std::atoi(values[i].c_str());
            return val;
        }
    }
}ProcedureCondition;



typedef struct ProcedureEdge{
    std::shared_ptr<ProcedureInstructionVertex>  end;
    ProcedureCondition condition;
}ProcedureConditionalEdge;


typedef struct ProcedureInstructionVertex{
    int     begin;
    ProcedureInstruction                   instruction;
    std::vector<ProcedureEdge>    edges;
    std::vector<std::string>        code;
};

//mozna zrobic w ciele modulo
void neqCond(Condition & cond);

Value NULL_VALUE();

Condition TRUE_CONDITION();

Edge NON_COND_EDGE(std::shared_ptr<InstructionVertex> end);

std::string readValue(std::shared_ptr<ParseTreeNode> & valueNode);
Expression readExpression(std::shared_ptr<ParseTreeNode> & expressionTree);
Condition readCondition(std::shared_ptr<ParseTreeNode> & conditionTree);
std::shared_ptr<InstructionVertex> buildCommandFlowChart(std::shared_ptr<ParseTreeNode> & commandTree, std::shared_ptr<InstructionVertex> & beginVertex, std::shared_ptr<InstructionVertex> & endVertex);
std::shared_ptr<InstructionVertex> buildCommandsFlowChart(std::shared_ptr<ParseTreeNode> & commandsTree, std::shared_ptr<InstructionVertex> & endVertex);
std::shared_ptr<InstructionVertex> buildCommandsFlowChart(std::shared_ptr<ParseTreeNode> & commandsTree, std::shared_ptr<InstructionVertex> & beginVertex, std::shared_ptr<InstructionVertex> & endVertex);
std::shared_ptr<InstructionVertex> buildFlowChart(std::shared_ptr<ParseTreeNode> programTree);

//wynik wszystkich operacji trzymany jest w akumulatorze
void addValues(Value val1, Value val2);
void subValues(Value val1, Value val2);
void doubleValue(Value val);


void translateReadVertex(std::shared_ptr<InstructionVertex> & readVertex);
void translateWriteVertex(std::shared_ptr<InstructionVertex> & readVertex);
void translateAssignVertex(std::shared_ptr<InstructionVertex> & assignVertex);
void transalteProcedureVertex(std::shared_ptr<InstructionVertex> procedureVertex);
void translateAllocVertex(std::shared_ptr<InstructionVertex> & allocVertex);
void translateConditionalEdge(ConditionalEdge* edge);

std::shared_ptr<InstructionVertex> getDispressionEnd(std::shared_ptr<InstructionVertex> & dispressionVertex);
//zwracamy dispression end
std::shared_ptr<InstructionVertex> translateDispressionVertex(std::shared_ptr<InstructionVertex> & dispressionVertex);

void codePushBack(std::string instruction);

void jumpMerge();


//w callu procedury zostawiamy puste sety i uzupelniamy je w ostatniej fazie kompilacji
extern std::string errorInfo;
extern std::vector<Line> code;
extern std::map<std::string, Procedure> procedures;

extern std::vector<std::string> procedureNamesInOrder;

extern int jumps_no;

extern int jump_to_merge;

extern int freeMem;

#endif  