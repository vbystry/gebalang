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
#include "procedure.hpp"

void neqCond(ProcedureCondition & cond)
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

ProcedureValue PROCEDURE_NULL_VALUE()
{
    ProcedureValue v;
    v.adress = -1;
    v.value = -1;
    return v;
}

ProcedureCondition PROCEDURE_TRUE_CONDITION()
{
    ProcedureCondition c;
    c.comparission = c_true;
    return c;
}

ProcedureEdge PROCEDURE_NON_COND_EDGE(std::shared_ptr<ProcedureInstructionVertex> end)
{
    ProcedureEdge e;
    e.condition = PROCEDURE_TRUE_CONDITION();
    e.end = end;
    return e;
}


std::string procedureReadValue(std::shared_ptr<ParseTreeNode> &valueNode)
{
    // Value value;
    std::string value;
    switch (valueNode->childs[0]->tokens[0])
    {
    case t_num:
        value = valueNode->childs[0]->params[0];
        break;
    case t_identifier:
        value = "@" + valueNode->childs[0]->params[0];
        // error jak undef
        break;

        // error, impossible
    }
    return value;
}


ProcedureExpression procedureReadExpression(std::shared_ptr<ParseTreeNode> &expressionTree)
{
    // check if node is
    ProcedureExpression expr;
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

    // readValues
    expr.values.push_back(procedureReadValue(expressionTree->childs[0]));
    if (expr.op != o_null)
        expr.values.push_back(procedureReadValue(expressionTree->childs[1]));

    return expr;
}

ProcedureCondition procedureReadCondition(std::shared_ptr<ParseTreeNode> &conditionTree)
{
    // check if node is condition

    ProcedureCondition cond;

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

    // readValues
    cond.values[0] = procedureReadValue(conditionTree->childs[0]);
    cond.values[1] = procedureReadValue(conditionTree->childs[1]);

    return cond;
}


std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandFlowChart(std::shared_ptr<ParseTreeNode> &commandTree, std::shared_ptr<ProcedureInstructionVertex> &beginVertex, std::shared_ptr<ProcedureInstructionVertex> &endVertex)
{
    std::shared_ptr<ProcedureInstructionVertex> commandRoot = std::make_shared<ProcedureInstructionVertex>();
    beginVertex->edges.push_back(PROCEDURE_NON_COND_EDGE(commandRoot));

    if (!std::strcmp(commandTree->params[0].c_str(), "ASSIGN"))
    {
        commandRoot->instruction.operation = o_ASSIGN;
        // error jesli zmienna niezadeklarowana
        std::string variableName = commandTree->childs[0]->params[0];
        commandRoot->instruction.values.push_back("@" + variableName);

        commandRoot->instruction.expression = procedureReadExpression(commandTree->childs[1]);
        commandRoot->begin = (beginVertex->begin + beginVertex->code.size());
        // std::vector<std::string> codeToAppend=translateAssignVertex(commandRoot);
        // commandRoot->code//dodac code to aapend
        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));
        endVertex->begin = (commandRoot->begin + commandRoot->code.size());
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "IF"))
    {
        ProcedureCondition cond1 = procedureReadCondition(commandTree->childs[0]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;

        std::shared_ptr<ProcedureInstructionVertex> ifEndVertex = std::make_shared<ProcedureInstructionVertex>();
        ifEndVertex->instruction.operation = o_DISPRESSION_END;

        ifEndVertex->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));

        // end of first edge is chart of commands after then statement
        ProcedureEdge edge1;
        edge1.condition = cond1;
        edge1.end = buildProcedureCommandsFlowChart(commandTree->childs[1], ifEndVertex);
        commandRoot->edges.push_back(edge1);

        // if statement is if then else
        if (!(std::strcmp(commandTree->params[2].c_str(), "ELSE")))
            commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(buildProcedureCommandsFlowChart(commandTree->childs[2], ifEndVertex)));
        else
            commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(ifEndVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "WHILE"))
    {
        ProcedureCondition cond1 = procedureReadCondition(commandTree->childs[0]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;

        std::shared_ptr<ProcedureInstructionVertex> whileEndVertex = std::make_shared<ProcedureInstructionVertex>();
        // end of first edge is chart of commands after then statement
        ProcedureConditionalEdge edge1;
        edge1.condition = cond1;

        edge1.end = buildProcedureCommandsFlowChart(commandTree->childs[1], whileEndVertex);
        commandRoot->edges.push_back(edge1);

        ProcedureEdge edge2;
        edge2.condition = cond1;
        edge2.end = commandRoot;
        whileEndVertex->instruction.operation = o_DISPRESSION_END;
        whileEndVertex->edges.push_back(edge2);
        whileEndVertex->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));

        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(whileEndVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "REPEAT"))
    {
        ProcedureCondition cond1 = procedureReadCondition(commandTree->childs[1]);
        commandRoot->instruction.operation = o_DISPRESSION_BEGIN;
        std::shared_ptr<ProcedureInstructionVertex> untillVertex = std::make_shared<ProcedureInstructionVertex>();
        ;
        untillVertex->instruction.operation = o_DISPRESSION_END;
        ProcedureConditionalEdge edge1;
        neqCond(cond1);
        edge1.condition = cond1;
        edge1.end = commandRoot;
        
        untillVertex->edges.push_back(edge1);

        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(buildProcedureCommandsFlowChart(commandTree->childs[0], untillVertex)));

        // end of first edge is chart of commands after then statement
        untillVertex->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "proc_head"))
    {
        std::shared_ptr<ProcedureInstructionVertex> untillVertex = std::make_shared<ProcedureInstructionVertex>();
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
        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "READ"))
    {
        commandRoot->instruction.operation = o_READ;
        // error jesli zmienna niezadeklarowana
        std::string variableName = commandTree->childs[0]->params[0];
        commandRoot->instruction.values.push_back("@" + variableName);
        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));
    }
    else if (!std::strcmp(commandTree->params[0].c_str(), "WRITE"))
    {
        commandRoot->instruction.operation = o_WRITE;
        commandRoot->instruction.values.push_back(procedureReadValue(commandTree->childs[0]));
        commandRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(endVertex));
    }
    return commandRoot;
}

std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<ProcedureInstructionVertex> &endVertex)
{
    std::shared_ptr<ProcedureInstructionVertex> commandsRoot = std::make_shared<ProcedureInstructionVertex>();

    if (commandsTree->childs.size() == 1)
    {
        return buildProcedureCommandFlowChart(commandsTree->childs[0], commandsRoot, endVertex);
    }
    else
    {
        // command chart ma dwoch ojcow, deadcodevertex i commandvertexlast
        std::shared_ptr<ProcedureInstructionVertex> deadCodeVertex = std::make_shared<ProcedureInstructionVertex>();
        deadCodeVertex->instruction.operation = o_OP_NULL;
        std::shared_ptr<ProcedureInstructionVertex> commandVertex = buildProcedureCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex);
        return buildProcedureCommandsFlowChart(commandsTree->childs[0], commandsRoot, commandVertex);
    }
}

std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<ProcedureInstructionVertex> &beginVertex, std::shared_ptr<ProcedureInstructionVertex> &endVertex)
{
    std::shared_ptr<ProcedureInstructionVertex> commandsRoot = std::make_shared<ProcedureInstructionVertex>();

    if (commandsTree->childs.size() == 1)
    {
        return buildProcedureCommandFlowChart(commandsTree->childs[0], beginVertex, endVertex);
    }
    else
    {
        // command chart ma dwoch ojcow, deadcodevertex i commandvertexlast
        std::shared_ptr<ProcedureInstructionVertex> deadCodeVertex = std::make_shared<ProcedureInstructionVertex>();
        deadCodeVertex->instruction.operation = o_OP_NULL;
        std::shared_ptr<ProcedureInstructionVertex> commandVertex = buildProcedureCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex);
        return buildProcedureCommandsFlowChart(commandsTree->childs[0], beginVertex, commandVertex);
    }
}

std::shared_ptr<ProcedureInstructionVertex> buildProcedureFlowChart(Procedure & procedure)
{
    std::shared_ptr<ParseTreeNode> procedureTree = procedure.parseTree;
    std::shared_ptr<ProcedureInstructionVertex> programRoot = std::make_shared<ProcedureInstructionVertex>();
    //deklaracja zmiennych referowanych
    std::shared_ptr<ParseTreeNode> proc_head = procedureTree->childs[0];
    //deklaracje zmiennych referowanych
    std::shared_ptr<ParseTreeNode> declarations = proc_head->childs[1];
    std::vector<ProcedureValue> values;
    int refNo=0;
    while (declarations->childs.size() > 1)
        {
            ProcedureValue val;
            val.name = declarations->childs[1]->params[0];
            val.indirectPostfix="I ";
            val.refNo=refNo;
            refNo++;
            if(procedure.variables.contains(declarations->childs[1]->params[0]))
            {
                errorInfo = "Multiple declaration of variable: " + declarations->childs[1]->params[0];
                
            }
            procedure.variables.insert({declarations->childs[1]->params[0], val});
            values.push_back(val);
            declarations = declarations->childs[0];
        }
    ProcedureValue val;
    val.name = declarations->childs[0]->params[0];
    val.indirectPostfix="I ";
    val.refNo=refNo;
            refNo++;
    if(procedure.variables.contains(declarations->childs[0]->params[0]))
    {
        errorInfo = "Multiple declaration of variable: " + declarations->childs[1]->params[0];
        
    }        
    procedure.variables.insert({declarations->childs[0]->params[0], val});
    values.push_back(val);

    programRoot->instruction.operation = o_ALLOC;

    int commandsNo=1;
    



    if (!procedureTree->params[1].compare("IS VAR"))
    {
        commandsNo=2;
        // zadeklaruj zmienne
        //deklaracje nieref
        declarations = procedureTree->childs[1];
        while (declarations->childs.size() > 1)
        {
            ProcedureValue val;
            val.name = declarations->childs[1]->params[0];
            val.indirectPostfix=" ";
            if(procedure.variables.contains(declarations->childs[1]->params[0]))
            {
                errorInfo = "Multiple declaration of variable: " + declarations->childs[1]->params[0];
                
            }
            procedure.variables.insert({declarations->childs[1]->params[0], val});
            values.push_back(val);
            declarations = declarations->childs[0];
        }
        ProcedureValue val;
        val.name = declarations->childs[0]->params[0];
        val.indirectPostfix=" ";
        if(procedure.variables.contains(declarations->childs[0]->params[0]))
        {
            errorInfo = "Multiple declaration of variable: " + declarations->childs[1]->params[0];
            
        }   
        procedure.variables.insert({declarations->childs[0]->params[0], val});

        values.push_back(val);

        programRoot->instruction.operation = o_ALLOC;
    }
    

    for (int i = 0; i < values.size(); i++)
            programRoot->instruction.values.push_back("@" + values[i].name);

    std::shared_ptr<ProcedureInstructionVertex> programEnd = std::make_shared<ProcedureInstructionVertex>();
    programEnd->instruction.operation = o_END;
    programRoot->edges.push_back(PROCEDURE_NON_COND_EDGE(buildProcedureCommandsFlowChart(
        procedureTree->childs[commandsNo], programRoot, programEnd)));

    return programRoot;
}


void addValues(ProcedureValue val1, ProcedureValue val2)
{
    // if val1 is variable
    std::string i;
    if (val1.adress > -1)
    {
        // if val2 is variable
        if (val2.adress > -1)
        {
            i = std::to_string(val1.adress);
            codePushBack("LOAD" + val1.indirectPostfix + i);
            i = std::to_string(val2.adress);
            code.push_back({"ADD"+ val2.indirectPostfix + i, 0});
        }
        else
        {
            i = std::to_string(val2.value);
            codePushBack("SET " + i);
            i = std::to_string(val1.adress);
            code.push_back({"ADD" + val1.indirectPostfix+ i, 0});
        }
    }
    else
    {
        if (val2.adress > -1)
        {
            i = std::to_string(val1.value);
            codePushBack("SET "  + val1.indirectPostfix + i);
            i = std::to_string(val2.adress);
            code.push_back({"ADD"  + val2.indirectPostfix + i, 0});
        }
        else
        {
            i = std::to_string(val1.value + val2.value);
            codePushBack("SET " + i);
        }
    }
}

void subValues(ProcedureValue val1, ProcedureValue val2)
{

    std::string i;
    // if val1 is variable
    if (val1.adress > -1)
    {
        // if val2 is variable
        if (val2.adress > -1)
        {
            i = std::to_string(val1.adress);
            codePushBack("LOAD" + val1.indirectPostfix + i);
            i = std::to_string(val2.adress);
            code.push_back({"SUB" + val2.indirectPostfix + i, 0});
        }
        else
        {
            i = std::to_string(val2.value);
            codePushBack("SET " + val2.indirectPostfix + i);
            i = std::to_string(freeMem);
            code.push_back({"STORE " + i, 0});
            i = std::to_string(val1.adress);
            code.push_back({"LOAD" + val1.indirectPostfix + i, 0});
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
            code.push_back({"SUB" + val2.indirectPostfix + i, 0});
        }
        else
        {
            // do opt predictable wystarczy wchodzic tu gdy valeus sa >-1 (done)
            i = std::to_string(fmax(val1.value - val2.value, 0));
            codePushBack("SET " + i);
        }
    }
}

void doubleValue(ProcedureValue val)
{
    std::string i;

    addValues(val, val);
}

void halfValue(ProcedureValue val)
{
    std::string i;
    // if val is variable
    if (val.adress > -1)
    {
        i = std::to_string(val.adress);
        codePushBack("LOAD"  + val.indirectPostfix + i);
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

void translateReadVertex(std::shared_ptr<ProcedureInstructionVertex> &readVertex, Procedure procedure)
{
    ProcedureValue val= readVertex->instruction.getValue(0, procedure);
    std::string i = val.indirectPostfix + std::to_string( val.adress);
    if(val.refNo>-1)
    {
        
        i = std::to_string(freeMem);
        codePushBack("GET" + i);
        codePushBack("LOAD " + i);
        std::string i = std::to_string(val.adress);
        codePushBack("STOREI " + i);
    }
    else
        codePushBack("GET" + i);
}

void translateWriteVertex(std::shared_ptr<ProcedureInstructionVertex> &readVertex, Procedure procedure)
{
    ProcedureValue val= readVertex->instruction.getValue(0, procedure);
    int adress =val.adress;
    if(val.refNo>-1)
    {
        std::string i = std::to_string(adress);
        codePushBack("LOADI " + i);
        i = std::to_string(freeMem);
        codePushBack("STORE " + i);
        codePushBack("PUT " + i);
    }
    else if (adress > -1)
    {
        std::string i =val.indirectPostfix+ std::to_string(adress);
        codePushBack("PUT" + i);
    }
    else
    {
        int value = val.value;
        std::string i = std::to_string(value);
        codePushBack("SET " + i);
        i = std::to_string(freeMem);
        codePushBack("STORE " + i);
        codePushBack("PUT " + i);
    }
}

void translateAllocVertex(std::shared_ptr<ProcedureInstructionVertex> &allocVertex, Procedure & procedure)
{
    procedure.retAdd=freeMem;
    freeMem++;
    for (std::string v : allocVertex->instruction.values)
    {
        ProcedureValue & val =procedure.variables[v.substr(1, v.size() - 1)];
        if(val.refNo>-1)
        {
            //pop do zmiany kolejnosci
            procedure.refVariables.push_back(v.substr(1, v.size() - 1));
            val.adress = freeMem;
            freeMem++;
        }
        
    }
    for (std::string v : allocVertex->instruction.values)
    {
        ProcedureValue & val =procedure.variables[v.substr(1, v.size() - 1)];
        if(val.refNo==-1)
        {
            val.adress = freeMem;
            freeMem++;
        }
        
    }
}

void translateAssignVertex(std::shared_ptr<ProcedureInstructionVertex> &assignVertex, Procedure procedure)
{
    ProcedureValue v1 = assignVertex->instruction.expression.getValue(0, procedure);
    ProcedureValue v2;
    if(assignVertex->instruction.expression.values.size()>1)
        v2= assignVertex->instruction.expression.getValue(1, procedure);

    std::string i;
    int jmp1;
    int jmp2;
    int jmp3;
    int jmp4;
    int jmp5;
    int jmp6;
    ProcedureValue val;
    switch (assignVertex->instruction.expression.op)
    {
    case o_null:

        if (assignVertex->instruction.expression.getValue(0, procedure).adress > -1)
        {
            // assign variable
            val = assignVertex->instruction.expression.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
            codePushBack("LOAD" + i);
        }
        else
        {
            i = std::to_string(assignVertex->instruction.expression.getValue(0, procedure).value);
            codePushBack("SET " + i);
        }
        val = assignVertex->instruction.getValue(0, procedure);
        i =val.indirectPostfix+ std::to_string(assignVertex->instruction.getValue(0, procedure).adress);
        code.push_back({"STORE" + i, 0});
        break;
    case o_plus:
    
        addValues(assignVertex->instruction.expression.getValue(0, procedure), assignVertex->instruction.expression.getValue(1, procedure));

        val = assignVertex->instruction.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
        codePushBack({"STORE" + i, 0});
        break;
    case o_minus:
        subValues(assignVertex->instruction.expression.getValue(0, procedure), assignVertex->instruction.expression.getValue(1, procedure));

        val = assignVertex->instruction.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
        code.push_back({"STORE" + i, 0});
        break;
    case o_mult:
        if (v1.adress < 0)
        {
            v1.adress = freeMem + 2;
            v1.indirectPostfix = " ";
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v1.indirectPostfix+ std::to_string(v1.adress);
            v1.adress = freeMem + 2;
            v1.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 3;
            v2.indirectPostfix = " ";
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v2.indirectPostfix+ std::to_string(v2.adress);
            v2.adress = freeMem + 3;
            v2.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }

        i = std::to_string(0);
        codePushBack("SET " + i);
        i = std::to_string(freeMem);
        code.push_back({"STORE " + i, 0});

        jumps_no++;
        jmp3 = jumps_no;
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i, jmp3});
        jumps_no++;
        jmp1 = jumps_no;
        code.push_back({"JZERO", jmp1});

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
        halfValue(v2);

        i = std::to_string(v2.adress);
        code.push_back({"STORE " + i, 0});
        // check if b>0 (do while)
        i = std::to_string(v2.adress);
        code.push_back({"LOAD " + i, 0});

        code.push_back({"JPOS", jmp3});

        i = std::to_string(freeMem);
        code.push_back({"LOAD " + i, 0});

        val = assignVertex->instruction.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
        code.push_back({"STORE" + i, jmp1});
        break;
    case o_div: 
        // initialise tmp
        jumps_no++;
        jmp5=jumps_no;
        if (v1.adress < 0)
        {
            v1.adress = freeMem + 3;
            v1.indirectPostfix = " ";
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v1.indirectPostfix+ std::to_string(v1.adress);
            v1.adress = freeMem + 3;
            v1.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 4;
            v2.indirectPostfix = " ";
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v2.indirectPostfix+ std::to_string(v2.adress);
            v2.adress = freeMem + 4;
            v2.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
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
        code.push_back({"SUB " + i, 0});
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

        val = assignVertex->instruction.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
        code.push_back({"STORE" + i, 0});
        break;
    case o_mod:
    jumps_no++;
        jmp5=jumps_no;
        if (v1.adress < 0)
        {
            v1.adress = freeMem + 3;
            v1.indirectPostfix = " ";
            i = std::to_string(v1.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v1.indirectPostfix+ std::to_string(v1.adress);
            v1.adress = freeMem + 3;
            v1.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
            i = std::to_string(v1.adress);
            code.push_back({"STORE " + i, 0});
        }
        if (v2.adress < 0)
        {
            v2.adress = freeMem + 4;
            v2.indirectPostfix = " ";
            i = std::to_string(v2.value);
            codePushBack({"SET " + i, 0});
            i = std::to_string(v2.adress);
            code.push_back({"STORE " + i, 0});
        }
        else
        {
            i =v2.indirectPostfix+ std::to_string(v2.adress);
            v2.adress = freeMem + 4;
            v2.indirectPostfix = " ";
            codePushBack({"LOAD" + i, 0});
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
        code.push_back({"SUB " + i, 0});
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

        val = assignVertex->instruction.getValue(0, procedure);
            i =val.indirectPostfix+ std::to_string(val.adress);
        code.push_back({"STORE" + i, 0});
        break;
    }
}


void transalteProcedureVertex(std::shared_ptr<ProcedureInstructionVertex> procedureVertex, Procedure universe)
{
    Procedure procedure = procedures[procedureVertex->instruction.values[0]];
    std::string i;
    for(int m = 0; m<procedure.refVariables.size(); m++)
    {
        i=std::to_string(universe.variables[ procedureVertex->instruction.values[m+1]].adress);

        if(universe.variables[ procedureVertex->instruction.values[m+1]].refNo>-1)
            codePushBack("LOAD "+i);
        else
            codePushBack("SET "+i);
        i=std::to_string(procedure.variables[procedure.refVariables[m]].adress);
        codePushBack("STORE " + i);
    }
    i=std::to_string(code.size()+3);
    codePushBack("SET "+i);
    i=std::to_string(procedure.retAdd);
    codePushBack("STORE "+i);
    code.push_back({"JUMP", procedure.jumpNo});
}

void translateConditionalEdge(ProcedureConditionalEdge &edge, Procedure procedure)
{
    ProcedureValue v1 = edge.condition.getValue(0, procedure);
    ProcedureValue v2 = edge.condition.getValue(1, procedure);
    std::string i;
    Line jump;
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

void translateNegConditionalEdge(ProcedureConditionalEdge &edge, Procedure procedure)
{
    Line jump;
    ProcedureValue v1 = edge.condition.getValue(0, procedure);
    ProcedureValue v2 = edge.condition.getValue(1, procedure);
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

    // return NULL;
}

std::shared_ptr<ProcedureInstructionVertex> getDispressionEnd(std::shared_ptr<ProcedureInstructionVertex> &dispressionVertex)
{
    std::shared_ptr<ProcedureInstructionVertex> activeVertex = dispressionVertex->edges[0].end;
    while (activeVertex->instruction.operation != o_DISPRESSION_END)
    {
        switch (activeVertex->instruction.operation)
        {
        case o_DISPRESSION_BEGIN:
            activeVertex = getDispressionEnd(activeVertex);
            activeVertex = activeVertex->edges[0].end;
            break;
        default:
            activeVertex = activeVertex->edges[0].end;
            break;
        }

        // error
    }
    return activeVertex;
}

std::shared_ptr<ProcedureInstructionVertex> translateDispressionVertex(std::shared_ptr<ProcedureInstructionVertex> &dispressionVertex, Procedure procedure)
{
    std::shared_ptr<ProcedureInstructionVertex> endVertex = getDispressionEnd(dispressionVertex);

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

    std::shared_ptr<ProcedureInstructionVertex> activeVertex;
    if (dispressionVertex->edges[0].condition.comparission == c_true)
    {
        activeVertex = dispressionVertex->edges[0].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex, procedure);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
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
                translateAssignVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex, procedure);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
        }
    }
    // jesli program w drugim edgu jest pusty
    else if (dispressionVertex->edges[1].end->instruction.operation == o_DISPRESSION_END)
    {
        translateNegConditionalEdge(dispressionVertex->edges[0], procedure);
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
                translateAssignVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex, procedure);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex, procedure);
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
        translateNegConditionalEdge(dispressionVertex->edges[0], procedure);
        Line &jump1 = code.back();
        int our_jump_no1 = jump1.jump_number;

        activeVertex = dispressionVertex->edges[0].end;
        while (activeVertex->instruction.operation != o_DISPRESSION_END)
        {
            switch (activeVertex->instruction.operation)
            {
            case o_ASSIGN:
                translateAssignVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex, procedure);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex, procedure);
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
                translateAssignVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_WRITE:
                translateWriteVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_READ:
                translateReadVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_DISPRESSION_BEGIN:
                activeVertex = translateDispressionVertex(activeVertex, procedure);
                break;
            case o_CALL_PROC:
                transalteProcedureVertex(activeVertex, procedure);
                activeVertex = activeVertex->edges[0].end;
                break;
            case o_OP_NULL:
                activeVertex = activeVertex->edges[0].end;
                break;
            }

            // error
        }
        
        jump_to_merge = our_jump_no2;
    }
    // jesli petla
    if (end_jump_no > 0)
    {
        translateConditionalEdge(endVertex->edges[0], procedure);
        Line &jmp = code.back();
        jmp.jump_number = end_jump_no;
    }
    for (ProcedureEdge e : activeVertex->edges)
    {
        if (e.condition.comparission == c_true)
            return e.end;
    }
    return nullptr;
}