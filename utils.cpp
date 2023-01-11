#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   //zmienic w bisonie list->vector
#include <string>
#include <map>
#include "utils.hpp"



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
    //declare value
}

Value declareValues(ParseTreeNode* valuesNode)
{
    //declare values
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
InstructionVertex* buildCommandFlowChart(ParseTreeNode* commandTree, InstructionVertex* & beginVertex, InstructionVertex* & endVertex)
{
    InstructionVertex* commandRoot = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
    beginVertex->edges.insert(commandRoot);

    //InstructionVertex* current = commandChart;
    switch(commandTree->params[0])
    {
        case "ASSIGN":
        {
            commandRoot->instruction.operation=ASSIGN;
            //error jesli zmienna niezadeklarowana
            std::string variableName = commandTree->childs[0]->params[0];
            commandRoot->instruction.values.insert(variableName);

            commandRoot->instruction.expression=readExpression(commandTree->childs[1]);
            commandRoot->edges.insert(endVertex);
        }
        case "IF":
        {
            Condition cond1=readCondition(commandTree->childs[0]);
            commandRoot->instruction.operation=OP_NULL;



            //end of first edge is chart of commands after then statement
            ConditionalEdge edge1;
            edge1.condition=cond1;
            edge1.end=buildCommandsFlowChart(commandTree->childs[1], commandRoot, endVertex);
            commandRoot->edges.insert(edge1);

            //if statement is if then else
            if(!(strcmp(commandTree->params[2], "ELSE" )))
                commandRoot->edges.insert( buildCommandsFlowChart(commandTree->childs[1], commandRoot, endVertex));
            else
                commandRoot->edges.insert(endVertex);

            break;
        }
        case "WHILE":
        {
            Condition cond1=readCondition(commandTree->childs[0]);
            commandRoot->instruction.operation=OP_NULL;

            //end of first edge is chart of commands after then statement
            ConditionalEdge edge1;
            edge1.condition=cond1;
            edge1.end=buildCommandsFlowChart(commandTree->childs[1], commandRoot, commandRoot);
            commandRoot->edges.insert(edge1);
            
            commandRoot->edges.insert(endVertex);

            break;
        }
        case "REPEAT":
        {
            Condition cond1=readCondition(commandTree->childs[1]);
            commandRoot->instruction.operation=OP_NULL;
            InstructionVertex* untillVertex = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));;
            untillVertex->instruction.operation=OP_NULL;
            ConditionalEdge edge1;
            edge1.condition=cond1;
            edge1.end=commandRoot
            untillVertex->edges.insert(edge1);

            commandRoot->edges.insert(buildCommandsFlowChart(commandTree->childs[0], commandRoot, untillVertex));


            //end of first edge is chart of commands after then statement
            
            commandRoot->edges.insert(endVertex);

            break;
        }
        case "proc_head":
        {
            //call procedure
            break;
        }
        case "READ":
        {
            commandRoot->instruction.operation=READ;
            //error jesli zmienna niezadeklarowana
            std::string variableName = commandTree->childs[0]->params[0];
            commandRoot->instruction.values.insert(variableName);
            commandRoot->edges.insert(endVertex);
        }
        case "WRITE":
        {
            commandRoot->instruction.operation=READ;
            commandRoot->instruction.values.insert(readValue(commandTree->childs[0]));
            commandRoot->edges.insert(endVertex);
        }
    }

    return commandRoot;
}

InstructionVertex* buildCommandsFlowChart(ParseTreeNode* commandsTree, InstructionVertex* & beginVertex, InstructionVertex* & endVertex)
{
    InstructionVertex* commandsRoot = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));

    if(commandsTree->childs.size==1)
    {
        return buildCommandFlowChart(commandsTree->childs[0], beginVertex, endVertex);
    }
    else
    {
        //command chart ma dwoch ojcow, deadcodevertex i commandvertexlast
        InstructionVertex* deadCodeVertex = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
        deadCodeVertex->instruction=OP_NULL;
        return buildCommandsFlowChart(commandsTree->childs[0], beginVertex, buildCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex));
    }

}

InstructionVertex buildFlowChart(ParseTreeNode* programTree)
{
    if( programTree->params[2].compare("VAR")  )
    {
        //zadeklaruj zmienne
    }

}