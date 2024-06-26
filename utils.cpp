#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector> //zmienic w bisonie list->vector
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <math.h>

#include "utils.hpp"
#include "yacc.hpp"

std::vector<Line> code;
std::string errorInfo;

std::map<std::string, Value> variables;
std::vector<std::string> procedureNamesInOrder;

int jumps_no;

int jump_to_merge = 0;

int freeMem = 1;

std::map<std::string, Procedure> procedures;

void neqCond(Condition & cond)
{
    switch (cond.comparission)
    {
    case c_true:
        cond.comparission=c_false;
        break;
    case neq:
        cond.comparission=eq;
        break;
    case eq:
        cond.comparission=neq;
        break;
    case gt:
        cond.comparission=lte;
        break;
    case gte:
        cond.comparission=lt;
        break;
    case lt:
        cond.comparission=gte;
        break;
    case lte:
        cond.comparission=gt;
        break;
    
    default:
        break;
    }
}

Value NULL_VALUE()
{
    Value v;
    v.adress = -1;
    v.value = -1;
    return v;
}

Condition TRUE_CONDITION()
{
    Condition c;
    c.comparission = c_true;
    return c;
}

Edge NON_COND_EDGE(std::shared_ptr<InstructionVertex> end)
{
    Edge e;
    e.condition = TRUE_CONDITION();
    e.end = end;
    return e;
}

std::string readValue(std::shared_ptr<ParseTreeNode> &valueNode)
{
    std::string value;
    std::string i = std::to_string(freeMem);
     Value val;
    switch (valueNode->childs[0]->tokens[0])
    {
    case t_num:
        //alloc const

         
        if(!variables.contains(valueNode->childs[0]->params[0]))
        {
            val.adress=freeMem;
        val.name=valueNode->childs[0]->params[0];
        variables.insert({valueNode->childs[0]->params[0], val});
        freeMem++;
        codePushBack("SET " + valueNode->childs[0]->params[0]);
        codePushBack("STORE " + i);
        }
        
        
        value ="@" + valueNode->childs[0]->params[0];
        break;
    case t_identifier:
        value = "@" + valueNode->childs[0]->params[0];
        // error jak undef
        break;

        // error, impossible
    }
    return value;
}

Expression readExpression(std::shared_ptr<ParseTreeNode> &expressionTree)
{
    Expression expr;
    if (!std::strcmp(expressionTree->params[0].c_str(), "NULL"))
        expr.op = o_null;
    else if (!std::strcmp(expressionTree->params[0].c_str(), "PLUS"))
        expr.op = o_plus;
    else if (!std::strcmp(expressionTree->params[0].c_str(), "MINUS"))
        expr.op = o_minus;
    else if (!std::strcmp(expressionTree->params[0].c_str(), "DIV"))
        expr.op = o_div;
    else if (!std::strcmp(expressionTree->params[0].c_str(), "MULT"))
        expr.op = o_mult;
    else if (!std::strcmp(expressionTree->params[0].c_str(), "MOD"))
        expr.op = o_mod;

        expr.values.push_back(readValue(expressionTree->childs[0]));
    if (expr.op != o_null)
        expr.values.push_back(readValue(expressionTree->childs[1]));

    return expr;
}

Condition readCondition(std::shared_ptr<ParseTreeNode> &conditionTree)
{
    Condition cond;

    if (!std::strcmp(conditionTree->params[0].c_str(), "EQ"))
        cond.comparission = eq;
    else if (!std::strcmp(conditionTree->params[0].c_str(), "NEQ"))
        cond.comparission = neq;
    else if (!std::strcmp(conditionTree->params[0].c_str(), "GT"))
        cond.comparission = gt;
    else if (!std::strcmp(conditionTree->params[0].c_str(), "LT"))
        cond.comparission = lt;
    else if (!std::strcmp(conditionTree->params[0].c_str(), "GTE"))
        cond.comparission = gte;
    else if (!std::strcmp(conditionTree->params[0].c_str(), "LTE"))
        cond.comparission = lte;

    cond.values[0] = readValue(conditionTree->childs[0]);
    cond.values[1] = readValue(conditionTree->childs[1]);

    return cond;
}

std::shared_ptr<InstructionVertex> buildCommandFlowChart(std::shared_ptr<ParseTreeNode> &commandTree, std::shared_ptr<InstructionVertex> &beginVertex, std::shared_ptr<InstructionVertex> &endVertex)
{
    std::shared_ptr<InstructionVertex> commandRoot = std::make_shared<InstructionVertex>();
    beginVertex->edges.push_back(NON_COND_EDGE(commandRoot));

    if (!std::strcmp(commandTree->params[0].c_str(), "ASSIGN"))
    {
        commandRoot->instruction.operation = o_ASSIGN;
        // error jesli zmienna niezadeklarowana
        std::string variableName = commandTree->childs[0]->params[0];
        commandRoot->instruction.values.push_back("@" + variableName);

        commandRoot->instruction.expression = readExpression(commandTree->childs[1]);
        commandRoot->begin = (beginVertex->begin + beginVertex->code.size());

        commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
        endVertex->begin = (commandRoot->begin + commandRoot->code.size());
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "IF"))
    {
        Condition cond1 = readCondition(commandTree->childs[0]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;

        std::shared_ptr<InstructionVertex> ifEndVertex = std::make_shared<InstructionVertex>();
        ifEndVertex->instruction.operation = o_DISPRESSION_END;

        ifEndVertex->edges.push_back(NON_COND_EDGE(endVertex));

        Edge edge1;
        edge1.condition = cond1;
        edge1.end = buildCommandsFlowChart(commandTree->childs[1], ifEndVertex);
        commandRoot->edges.push_back(edge1);

        if (!(std::strcmp(commandTree->params[2].c_str(), "ELSE")))
            commandRoot->edges.push_back(NON_COND_EDGE(buildCommandsFlowChart(commandTree->childs[2], ifEndVertex)));
        else
            commandRoot->edges.push_back(NON_COND_EDGE(ifEndVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "WHILE"))
    {
        Condition cond1 = readCondition(commandTree->childs[0]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;

        std::shared_ptr<InstructionVertex> whileEndVertex = std::make_shared<InstructionVertex>();
        ConditionalEdge edge1;
        edge1.condition = cond1;

        edge1.end = buildCommandsFlowChart(commandTree->childs[1], whileEndVertex);
        commandRoot->edges.push_back(edge1);

        ConditionalEdge edge2;
        edge2.condition = cond1;
        edge2.end = commandRoot;
        whileEndVertex->instruction.operation = o_DISPRESSION_END;
        whileEndVertex->edges.push_back(edge2);
        whileEndVertex->edges.push_back(NON_COND_EDGE(endVertex));

        commandRoot->edges.push_back(NON_COND_EDGE(whileEndVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "REPEAT"))
    {
        Condition cond1 = readCondition(commandTree->childs[1]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;
        std::shared_ptr<InstructionVertex> untillVertex = std::make_shared<InstructionVertex>();
        ;//tu jest cos nie tak
        untillVertex->instruction.operation = o_DISPRESSION_END;
        ConditionalEdge edge1;
        neqCond(cond1);
        edge1.condition = cond1;
        edge1.end = commandRoot;
        
        untillVertex->edges.push_back(edge1);

        commandRoot->edges.push_back(NON_COND_EDGE(buildCommandsFlowChart(commandTree->childs[0], untillVertex)));

        untillVertex->edges.push_back(NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "proc_head"))
    {
        std::shared_ptr<InstructionVertex> untillVertex = std::make_shared<InstructionVertex>();
        commandRoot->instruction.operation=o_CALL_PROC;
        commandRoot->instruction.values;

        commandRoot->instruction.values.push_back(commandTree->childs[0]->childs[0]->params[0]);
        std::shared_ptr<ParseTreeNode> declarations = commandTree->childs[0]->childs[1];
        while (declarations->childs.size() > 1)
        {
    
            commandRoot->instruction.values.push_back(declarations->childs[1]->params[0]);
            declarations = declarations->childs[0];
        }
        commandRoot->instruction.values.push_back(declarations->childs[0]->params[0]);
        commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "READ"))
    {
        commandRoot->instruction.operation = o_READ;
        // error jesli zmienna niezadeklarowana
        std::string variableName = commandTree->childs[0]->params[0];
        commandRoot->instruction.values.push_back("@" + variableName);
        commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "WRITE"))
    {
        commandRoot->instruction.operation = o_WRITE;
        commandRoot->instruction.values.push_back(readValue(commandTree->childs[0]));
        commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
    }

    return commandRoot;
}

std::shared_ptr<InstructionVertex> buildCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<InstructionVertex> &endVertex)
{
    std::shared_ptr<InstructionVertex> commandsRoot = std::make_shared<InstructionVertex>();

    if (commandsTree->childs.size() == 1)
    {
        return buildCommandFlowChart(commandsTree->childs[0], commandsRoot, endVertex);
    }
    else
    {
        // command chart ma dwoch ojcow, deadcodevertex i commandvertexlast
        std::shared_ptr<InstructionVertex> deadCodeVertex = std::make_shared<InstructionVertex>();
        deadCodeVertex->instruction.operation = o_OP_NULL;
        std::shared_ptr<InstructionVertex> commandVertex = buildCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex);
        return buildCommandsFlowChart(commandsTree->childs[0], commandsRoot, commandVertex);
    }
}

std::shared_ptr<InstructionVertex> buildCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<InstructionVertex> &beginVertex, std::shared_ptr<InstructionVertex> &endVertex)
{
    std::shared_ptr<InstructionVertex> commandsRoot = std::make_shared<InstructionVertex>();

    if (commandsTree->childs.size() == 1)
    {
        return buildCommandFlowChart(commandsTree->childs[0], beginVertex, endVertex);
    }
    else
    {
        // command chart ma dwoch ojcow, deadcodevertex i commandvertexlast
        std::shared_ptr<InstructionVertex> deadCodeVertex = std::make_shared<InstructionVertex>();
        deadCodeVertex->instruction.operation = o_OP_NULL;
        std::shared_ptr<InstructionVertex> commandVertex = buildCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex);
        return buildCommandsFlowChart(commandsTree->childs[0], beginVertex, commandVertex);
    }
}

std::shared_ptr<InstructionVertex> buildFlowChart(std::shared_ptr<ParseTreeNode> programTree)
{
    std::shared_ptr<InstructionVertex> programRoot = std::make_shared<InstructionVertex>();
    if (!programTree->params[1].compare("IS VAR"))
    {
        std::vector<Value> values;
        std::shared_ptr<ParseTreeNode> declarations = programTree->childs[0];
        while (declarations->childs.size() > 1)
        {
            Value val;
            val.name = declarations->childs[1]->params[0];
            if(variables.contains(declarations->childs[1]->params[0]))
            {
                errorInfo = "Multiple declaration of variable: " + declarations->childs[1]->params[0];
                
            }
            variables.insert({declarations->childs[1]->params[0], val});
            values.push_back(val);
            declarations = declarations->childs[0];
        }
        Value val;
        val.name = declarations->childs[0]->params[0];
        if(variables.contains(declarations->childs[0]->params[0]))
        {
            errorInfo = "Multiple declaration of variable: " + declarations->childs[0]->params[0];
            

        }
        variables.insert({declarations->childs[0]->params[0], val});
        values.push_back(val);

        programRoot->instruction.operation = o_ALLOC;



        for (int i = 0; i < values.size(); i++)
            programRoot->instruction.values.push_back("@" + values[i].name);
    }
    else
        programRoot->instruction.operation = o_OP_NULL;

    std::shared_ptr<InstructionVertex> programEnd = std::make_shared<InstructionVertex>();
    programEnd->instruction.operation = o_END;
    programRoot->edges.push_back(NON_COND_EDGE(buildCommandsFlowChart(
        programTree->childs[1], programRoot, programEnd)));

    return programRoot;
}

void addValues(Value val1, Value val2)
{
    std::string i;
    if (val1.adress > -1)
    {
        if (val2.adress > -1)
        {
            i = std::to_string(val1.adress);
            codePushBack("LOAD " + i);
            i = std::to_string(val2.adress);
            code.push_back({"ADD " + i, 0});
        }
        else
        {
            i = std::to_string(val2.value);
            codePushBack("SET " + i);
            i = std::to_string(val1.adress);
            code.push_back({"ADD " + i, 0});
        }
    }
    else
    {
        if (val2.adress > -1)
        {
            i = std::to_string(val1.value);
            codePushBack("SET " + i);
            i = std::to_string(val2.adress);
            code.push_back({"ADD " + i, 0});
        }
        else
        {
            i = std::to_string(val1.value + val2.value);
            codePushBack("SET " + i);
        }
    }
}

void subValues(Value val1, Value val2)
{

    std::string i;
    if (val1.adress > -1)
    {
        if (val2.adress > -1)
        {
            i = std::to_string(val1.adress);
            codePushBack("LOAD " + i);
            i = std::to_string(val2.adress);
            code.push_back({"SUB " + i, 0});
        }
        else
        {
            i = std::to_string(val2.value);
            codePushBack("SET " + i);
            i = std::to_string(freeMem);
            code.push_back({"STORE " + i, 0});
            i = std::to_string(val1.adress);
            code.push_back({"LOAD " + i, 0});
            i = std::to_string(freeMem);
            code.push_back({"SUB " + i, 0});
        }
    }
    else
    {
        if (val2.adress > -1)
        {
            i = std::to_string(val1.value);
            codePushBack("SET " + i);
            i = std::to_string(val2.adress);
            code.push_back({"SUB " + i, 0});
        }
        else
        {
            i = std::to_string(fmax(val1.value - val2.value, 0));
            codePushBack("SET " + i);
        }
    }
}

void doubleValue(Value val)
{
   
    std::string i;
   
    addValues(val, val);
}

void halfValue(Value val)
{
    std::string i;
    if (val.adress > -1)
    {
        i = std::to_string(val.adress);
        codePushBack("LOAD " + i);
        code.push_back({"HALF", 0});
    }
    else
    {
        i = std::to_string(val.value);
        codePushBack("SET " + i);
        code.push_back({"HALF", 0});
    }
    code;
}

void translateReadVertex(std::shared_ptr<InstructionVertex> &readVertex)
{
    // zmienic value na upredictable
    std::string i = std::to_string(readVertex->instruction.getValue(0).adress);
    codePushBack("GET " + i);
}

void translateWriteVertex(std::shared_ptr<InstructionVertex> &readVertex)
{
    int adress = readVertex->instruction.getValue(0).adress;
    if (adress > -1)
    {
        std::string i = std::to_string(adress);
        codePushBack("PUT " + i);
    }
    else
    {
        int value = readVertex->instruction.getValue(0).value;
        std::string i = std::to_string(value);
        codePushBack("SET " + i);
        i = std::to_string(freeMem);
        codePushBack("STORE " + i);
        codePushBack("PUT " + i);
    }
}

void translateAllocVertex(std::shared_ptr<InstructionVertex> &allocVertex)
{
    for (std::string v : allocVertex->instruction.values)
    {
        variables[v.substr(1, v.size() - 1)].adress = freeMem;
        freeMem++;
    }
}

void translateAssignVertex(std::shared_ptr<InstructionVertex> &assignVertex)
{
    Value v1 = assignVertex->instruction.expression.getValue(0);

    Value v2;
    if(assignVertex->instruction.expression.values.size()>1)
     v2= assignVertex->instruction.expression.getValue(1);

    std::string i;
    int jmp1;
    int jmp2;
    int jmp3;
    int jmp4;
    int jmp5;
    int jmp6;

    switch (assignVertex->instruction.expression.op)
    {
    case o_null:

        if (assignVertex->instruction.expression.getValue(0).adress > -1)
        {
            // assign variable
            i = std::to_string(assignVertex->instruction.expression.getValue(0).adress);
            codePushBack("LOAD " + i);
        }
        else
        {
            i = std::to_string(assignVertex->instruction.expression.getValue(0).value);
            codePushBack("SET " + i);
        }
        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        code.push_back({"STORE " + i, 0});
        break;
    case o_plus:
        addValues(assignVertex->instruction.expression.getValue(0), assignVertex->instruction.expression.getValue(1));

        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        codePushBack({"STORE " + i, 0});
        break;
    case o_minus:
        subValues(assignVertex->instruction.expression.getValue(0), assignVertex->instruction.expression.getValue(1));

        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        code.push_back({"STORE " + i, 0});
        break;
    case o_mult:
        if (v1.adress < 0)
        {
            v1.adress = freeMem + 2;
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v1.adress);
            v1.adress = freeMem + 2;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 3;
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v2.adress);
            v2.adress = freeMem + 3;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }

        i = std::to_string(0);
        codePushBack("SET " + i);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        // while(v2>0)
        jumps_no++;
        jmp3 = jumps_no;
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i, jmp3});
        jumps_no++;
        jmp1 = jumps_no;
        code.push_back({"JZERO", jmp1});

        //    if(v2%2==1)
        halfValue(v2);

        // double value
        i = std::to_string(freeMem + 1);
        code.push_back({"STORE " + i, 0});
        code.push_back({"ADD " + i, 0});
        code.push_back({"STORE " + i, 0});
        // check gt
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i, 0});
        i = std::to_string(freeMem + 1);
        code.push_back({"SUB " + i, 0});
        jumps_no++;
        jmp2 = jumps_no;
        code.push_back({"JZERO", jmp2});
        //      tmp+=a
        i = std::to_string(freeMem);
        code.push_back({"LOAD " + i, 0});
        i = std::to_string(v1.adress);
        code.push_back({"ADD " + i, 0});
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0}); // 9
        // a*=2, tu jumpujemy
        jump_to_merge = jmp2;
        doubleValue(v1);

        i = std::to_string(v1.adress);
        code.push_back({"STORE " + i, 0});
        // b/=2
        halfValue(v2); // robilismy przed chwila, mozna zapamietac

        i = std::to_string(v2.adress);
        code.push_back({"STORE " + i, 0});
        // check if b>0 (do while)
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i, 0}); // niepotrzebna linia, b w accu po halfie

        code.push_back({"JPOS", jmp3}); // tu skaczemy z while

        // assign p=tmp
        i = std::to_string(freeMem);
        code.push_back({"LOAD " + i, 0});

        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        code.push_back({"STORE " + i, jmp1});
        break;
    //moze byc odwrocona kolejnosc
    case o_div:
        // initialise tmp
        jumps_no++;
        jmp5=jumps_no;

        if (v1.adress < 0)
        {
            v1.adress = freeMem + 3;
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v1.adress);
            v1.adress = freeMem + 3;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 4;
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v2.adress);
            v2.adress = freeMem + 4;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }

        //check v2==0 (mamy w accu 0)
        code.push_back({"JZERO", jmp5});

        // initialise cnt
        i = std::to_string(0);
        code.push_back({"SET " + i, 0});
        i = std::to_string(freeMem + 2);
        code.push_back({"STORE " + i, 0});

        // while(b<=a)
        subValues(v2, v1);
        jumps_no++;
        jmp1 = jumps_no;
        code.push_back({"JPOS", jmp1}); // skaczemy gdy v2>v1
        //  i=1
        jumps_no++;
        jmp2 = jumps_no;
        i = std::to_string(1); // tutaj jumpem z pierwszego wjile
        code.push_back({"SET " + i, jmp2});
        i = std::to_string(freeMem + 1);
        code.push_back({"STORE " + i, 0});
        //  c=b
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});
        //  while(2c<a)
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0}); // double c
        ////i = std::to_string(v1.adress);
        code.push_back({"SUB " + i, 0});
        // i=std::to_string(v1.adress);
        jumps_no++;
        jmp3 = jumps_no;
        code.push_back({"JZERO", jmp3});
        //      i=2i
        i = std::to_string(freeMem + 1);
        jumps_no++;
        jmp4 = jumps_no;
        code.push_back({"LOAD " + i, jmp4});
        code.push_back({"ADD " + i, 0});
        code.push_back({"STORE " + i, 0});
        //      c=2c
        i = std::to_string(freeMem);
        
        code.push_back({"LOAD " + i, 0});
        code.push_back({"ADD " + i, 0});
        code.push_back({"STORE " + i, 0});
        // warun petli
        i = std::to_string(v1.adress);
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0}); // double c
        code.push_back({"SUB " + i, 0});
        code.push_back({"JPOS", jmp4});
        //  cnt+=i
        i = std::to_string(freeMem + 1); // tutaj jumpem z 2 petli
        code.push_back({"LOAD " + i, jmp3});
        i = std::to_string(freeMem + 2);
        code.push_back({"ADD " + i, 0});
        i = std::to_string(freeMem + 2);
        code.push_back({"STORE " + i, 0});
        //  a-=c
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i, 0});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0});
        i = std::to_string(v1.adress);
        code.push_back({"STORE " + i, 0});
        // warun 2 petli
        subValues(v2, v1);
        code.push_back({"JZERO", jmp2});

        // assign cnt = wynik
        i = std::to_string(freeMem + 2);
        code.push_back({"LOAD " + i, jmp1});

        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        code.push_back({"STORE " + i, jmp5});
        break;
    case o_mod:
        jumps_no++;
        jmp5=jumps_no;
        // initialise tmp
        if (v1.adress < 0)
        {
            v1.adress = freeMem + 3;
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v1.adress);
            v1.adress = freeMem + 3;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 4;
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i = std::to_string(v2.adress);
            v2.adress = freeMem + 4;
            codePushBack({"LOAD " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }

        //check v2==0 (mamy w accu 0)
        code.push_back({"JZERO", jmp5});

        // initialise cnt
        i = std::to_string(0);
        code.push_back({"SET " + i, 0});
        i = std::to_string(freeMem + 2);
        code.push_back({"STORE " + i, 0});

        // while(b<=a)
        subValues(v2, v1);
        jumps_no++;
        jmp1 = jumps_no;
        code.push_back({"JPOS", jmp1}); // skaczemy gdy v2>v1
        //  i=1
        jumps_no++;
        jmp2 = jumps_no;
        i = std::to_string(1); // tutaj jumpem z pierwszego wjile
        code.push_back({"SET " + i, jmp2});
        i = std::to_string(freeMem + 1);
        code.push_back({"STORE " + i, 0});
        //  c=b
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});
        //  while(2c<a)
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0}); // double c
        ////i = std::to_string(v1.adress);
        code.push_back({"SUB " + i, 0});
        // i=std::to_string(v1.adress);
        jumps_no++;
        jmp3 = jumps_no;
        code.push_back({"JZERO", jmp3});
        //      i=2i
        i = std::to_string(freeMem + 1);
        jumps_no++;
        jmp4 = jumps_no;
        code.push_back({"LOAD " + i, jmp4});
        code.push_back({"ADD " + i, 0});
        code.push_back({"STORE " + i, 0});
        //      c=2c
        i = std::to_string(freeMem);
        
        code.push_back({"LOAD " + i, 0});
        code.push_back({"ADD " + i, 0});
        code.push_back({"STORE " + i, 0});
        // warun petli
        i = std::to_string(v1.adress);
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0}); // double c
        code.push_back({"SUB " + i, 0});
        code.push_back({"JPOS", jmp4});
        //  cnt+=i
        i = std::to_string(freeMem + 1); // tutaj jumpem z 2 petli
        code.push_back({"LOAD " + i, jmp3});
        i = std::to_string(freeMem + 2);
        code.push_back({"ADD " + i, 0});
        i = std::to_string(freeMem + 2);
        code.push_back({"STORE " + i, 0});
        //  a-=c
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i, 0});
        i = std::to_string(freeMem);
        code.push_back({"SUB " + i, 0});
        i = std::to_string(v1.adress);
        code.push_back({"STORE " + i, 0});
        // warun 2 petli

        subValues(v2, v1);
        code.push_back({"JZERO", jmp2});

        // assign cnt = wynik
        i = std::to_string(v1.adress);
        code.push_back({"LOAD " + i, jmp1});

        i = std::to_string(assignVertex->instruction.getValue(0).adress);
        code.push_back({"STORE " + i, jmp5});
        break;
    }
}

void translateConditionalEdge(ConditionalEdge &edge)
{
    Line jump;
    if(edge.condition.comparission==c_true)
    {
        jumps_no++;
        // Line jump;
        jump.instruction = "JUMP";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        return;
    }
    Value v1 = edge.condition.getValue(0);
    Value v2 = edge.condition.getValue(1);
    std::string i;
    
    switch (edge.condition.comparission)
    {
    case neq:
        subValues(v1, v2);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        subValues(v2, v1);
        i = std::to_string(freeMem);
        code.push_back({"ADD " + i, 0});

        jumps_no++;
        // Line jump;
        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        // return &code.back();
        break;
    case eq:
        subValues(v1, v2);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        subValues(v2, v1);
        i = std::to_string(freeMem);
        code.push_back({"ADD " + i, 0});

        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case gt:
        subValues(v1, v2);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case gte:
        // neq lt
        subValues(v2, v1);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case lt:
        subValues(v2, v1);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case lte:
        // neq gt
        subValues(v1, v2);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;

    }
}

void translateNegConditionalEdge(ConditionalEdge &edge)
{
    Line jump;
    Value v1 = edge.condition.getValue(0);
    Value v2 = edge.condition.getValue(1);
    std::string i;
    switch (edge.condition.comparission)
    {

    case eq: //~neq
        subValues(v1, v2);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        subValues(v2, v1);
        i = std::to_string(freeMem);
        code.push_back({"ADD " + i, 0});

        jumps_no++;

        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case neq: //~eq
        subValues(v1, v2);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        subValues(v2, v1);
        i = std::to_string(freeMem);
        code.push_back({"ADD " + i, 0});

        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        // return &code.back();
        break;
    case lte: //~gt
        subValues(v1, v2);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case lt: //~gte
        // neq lt
        subValues(v2, v1);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case gte: //~le
        subValues(v2, v1);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JPOS";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    case gt: //~lte
        // neq gt
        subValues(v1, v2);
        i = std::to_string(freeMem);
        jumps_no++;
        // Line jump;
        jump.instruction = "JZERO";
        jump.jump_number = jumps_no;
        code.push_back(jump);
        break;
    }
}

void transalteProcedureVertex(std::shared_ptr<InstructionVertex> procedureVertex)
{
    Procedure procedure = procedures[procedureVertex->instruction.values[0]];
    std::string i;
    for(int m = 0; m<procedure.refVariables.size(); m++)
    {
        i=std::to_string(variables[ procedureVertex->instruction.values[m+1]].adress);
        codePushBack("SET "+i);
        i=std::to_string(procedure.variables[procedure.refVariables[m]].adress);
        codePushBack("STORE " + i);
    }
    i=std::to_string(code.size()+3);
    codePushBack("SET "+i);
    i=std::to_string(procedure.retAdd);
    codePushBack("STORE "+i);
    //check czy jumpno>0, inaczej niezainicjowana
    code.push_back({"JUMP", procedure.jumpNo});
}

std::shared_ptr<InstructionVertex> getDispressionEnd(std::shared_ptr<InstructionVertex> &dispressionVertex)
{
    std::shared_ptr<InstructionVertex> activeVertex = dispressionVertex->edges[0].end;
    while (activeVertex->instruction.operation != o_DISPRESSION_END)
    {
        switch (activeVertex->instruction.operation)
        {
        case o_DISPRESSION_BEGIN:
            activeVertex = getDispressionEnd(activeVertex);
            //edge x petli
            if(activeVertex->edges.size()>1)
                activeVertex = activeVertex->edges[1].end;
            else
                activeVertex = activeVertex->edges[0].end;
            break;
        default:
                activeVertex = activeVertex->edges[0].end;
            
            break;
        }

    }
    return activeVertex;
}

std::shared_ptr<InstructionVertex> translateDispressionVertex(std::shared_ptr<InstructionVertex> &dispressionVertex)
{
    std::shared_ptr<InstructionVertex> endVertex = getDispressionEnd(dispressionVertex);


    int end_jump_no = 0;
    if (endVertex->edges.size() > 1)
    {
        // rezerwujemy jumpa (i ustawiamy na 1 linie od begin vertexa)
        jumps_no++;
        end_jump_no = jumps_no;
        if (jump_to_merge > 0)
            std::cout << "dodac kolejke trzeba" << std::endl;
        jump_to_merge = end_jump_no;
    }

    std::shared_ptr<InstructionVertex> activeVertex;
    if (dispressionVertex->edges[0].condition.comparission == c_true)
    {
        activeVertex = dispressionVertex->edges[0].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

        }
    }
    else if (dispressionVertex->edges[0].condition.comparission == c_false)
    {
        activeVertex = dispressionVertex->edges[1].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }
        }
    }
    // jesli program w drugim edgu jest pusty
    else if (dispressionVertex->edges[1].end->instruction.operation == o_DISPRESSION_END)
    {
        translateNegConditionalEdge(dispressionVertex->edges[0]);
        Line &jump = code.back();
        jumps_no++;
        jump.jump_number = jumps_no;
        int our_jump_no = jumps_no;
        activeVertex = dispressionVertex->edges[0].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
        }
        if(jump_to_merge>0){
               jump.jump_number=jump_to_merge;
        }
        else
            jump_to_merge = our_jump_no;

    }
    else
    {
        translateNegConditionalEdge(dispressionVertex->edges[0]);
        Line &jump1 = code.back();
        int our_jump_no1 = jump1.jump_number;

        activeVertex = dispressionVertex->edges[0].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
        }
        jumps_no++;
        int our_jump_no2=jumps_no;
        if(jump_to_merge>0)   our_jump_no2=jump_to_merge;
        else
            jump_to_merge = jumps_no;
        // jump na end
        code.push_back({"JUMP", our_jump_no2});

        jump_to_merge = our_jump_no1;

        activeVertex = dispressionVertex->edges[1].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
        }
        jump_to_merge = our_jump_no2;
        // return activeVertex;
    }
    // jesli petla
    if (end_jump_no > 0)
    {
        
        translateConditionalEdge(endVertex->edges[0]);
        Line &jmp = code.back();
        jmp.jump_number = end_jump_no;
        return endVertex->edges[1].end;
    }
    return endVertex->edges[0].end;

}

void codePushBack(std::string instruction)
{
    code.push_back({instruction, jump_to_merge});

    if (jump_to_merge > 0)
        jump_to_merge = 0;
}

void jumpMerge()
{
    for (int j = 0; j < code.size(); j++)
    {
        Line &jumpLine = code[j];
        if (jumpLine.jump_number > 0)
        {
            std::string cmd = jumpLine.instruction;
            // jesli mamy jumpa
            if (jumpLine.jump_number > 0 && (!cmd.compare("JUMP") || !cmd.compare("JZERO") || !cmd.compare("JPOS")))
            {
                int jmpno = jumpLine.jump_number;
                bool jump_merged = false;
                for (int i = 0; i < code.size(); i++)
                {
                    Line line = code[i];
                    if (line.jump_number == jmpno)
                    {
                        std::string cmd2 = line.instruction;
                        // jesli nie na jumpie
                        if (cmd2.find("JUMP") == std::string::npos && cmd2.find("JZERO") == std::string::npos && cmd2.find("JPOS") == std::string::npos)
                        {
                            jumpLine.instruction.append(" " + std::to_string(i));
                            jump_merged = true;
                        }
                    }
                    else if(line.jump_jump_number == jmpno)
                    {
                        jumpLine.instruction.append(" " + std::to_string(i));
                        jump_merged = true;
                    }
                }
                // jump moze nie miec odpowiednika, moze miec to miejsce np
                // przy zagniezdzonych dispr gdy nie ma instrukcji
                if (!jump_merged)
                {
                    jumpLine.instruction.append(" " + std::to_string(j + 1));
                }
            }
        }
    }
}

