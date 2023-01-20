#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <vector>   //zmienic w bisonie list->vector
#include <string>
#include <map>
#include <typeinfo>
#include "utils.hpp"

Value readValue(ParseTreeNode* valueNode)
{
    Value value;
    switch(valueNode->tokens[0])
    {
        case num:
            value.value=std::atoi(valueNode->childs[0]->params[0]);
            break;
        case identifier:
            value=variables[valueNode->childs[0]->params[0]];
            //error jak undef
            break;
        default:
            //error, impossible
    }
    return value;
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
    //check if node is 
    


    Expression expr;

    switch(expressionTree->params[0])
    {
        case "NULL":
            expr.operator=null;
            break;
        case "PLUS":
            expr.operator=plus;
            break;
        case "MINUS":
            expr.operator=minus;
            break;
        case "DIV":
            expr.operator=div;
            break;
        case "MULT":
            expr.operator=mult;
            break;
        case "MOD":
            expr.operator=mod;
            break;
        default:
            //error, imposibble
    }

    //readValues
    expr.values[0]=readValue(expressionTree->childs[0]);
    if(expr.Operator!=null)
        expr.values[1]=readValue(expressionTree->childs[1]);

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
//zmienic nonConditionalEdge na conditionalEdge z warunem true
//ustawiac warun true lub false gdy values predicrtable
//conditional edge moga wychodzic tylko z dispression
InstructionVertex* buildCommandFlowChart(ParseTreeNode* commandTree, InstructionVertex* & beginVertex, InstructionVertex* & endVertex)
{
    InstructionVertex* commandRoot = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
    beginVertex->edges.push_back(NON_COND_EDGE(commandRoot));

    //InstructionVertex* current = commandChart;
    switch(commandTree->params[0])
    {
        case "ASSIGN":
        {
            commandRoot->instruction.operation=ASSIGN;
            //error jesli zmienna niezadeklarowana
            std::string variableName = commandTree->childs[0]->params[0];
            commandRoot->instruction.values.push_back(variableName);

            commandRoot->instruction.expression=readExpression(commandTree->childs[1]);
            commandRoot->begin=(beginVertex->begin+beginVertex->code.size());
            std::vector<std::string> codeToAppend=translateAssignVertex(commandRoot);
            commandRoot->code//dodac code to aapend
            commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
            endVertex->begin=(commandRoot->begin+commandRoot->code.size());
        }
        case "IF":
        {
            Condition cond1=readCondition(commandTree->childs[0]);
            commandRoot->instruction.operation=DISPRESSION_BEGIN;

            InstructionVertex* ifEndVertex = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
            ifEndVertex->instruction=DISPRESSION_END;
            ifEndVertex->edges.push_back(NON_COND_EDGE(endVertex));

            //end of first edge is chart of commands after then statement
            Edge edge1;
            edge1.condition=cond1;
            edge1.end=buildCommandsFlowChart(commandTree->childs[1], commandRoot, endVertex);
            commandRoot->edges.push_back(edge1);

            //if statement is if then else
            if(!(strcmp(commandTree->params[2], "ELSE" )))
            //moze sie wykrzaczyc
                commandRoot->edges.push_back(NON_COND_EDGE( buildCommandsFlowChart(commandTree->childs[1], commandRoot, ifEndVertex)));
            else
                commandRoot->edges.push_back(NON_COND_EDGE(ifEndVertex));

            break;
        }
        case "WHILE":
        {
            Condition cond1=readCondition(commandTree->childs[0]);
            commandRoot->instruction.operation=DISPRESSION_BEGIN;


            InstructionVertex* whileEndVertex = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
            //end of first edge is chart of commands after then statement
            ConditionalEdge edge1;
            edge1.condition=cond1;

            edge1.end=buildCommandsFlowChart(commandTree->childs[1], commandRoot, whileEndVertex);
            commandRoot->edges.push_back(edge1);


            ConditionalEdge edge2;
            edge2.condition=cond1;
            edge2.end=commandRoot;
            whileEndVertex->instruction=DISPRESSION_END;
            whileEndVertex->edges.push_back(edge2);
            whileEndVertex->edges.push_back(NON_COND_EDGE(endVertex));

            commandRoot->edges.push_back(whileEndVertex);

            break;
        }
        case "REPEAT":
        {
            Condition cond1=readCondition(commandTree->childs[1]);
            commandRoot->instruction.operation=DISPRESSION_BEGIN;
            InstructionVertex* untillVertex = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));;
            untillVertex->instruction.operation=DISPRESSION_END;
            ConditionalEdge edge1;
            edge1.condition=cond1;
            edge1.end=commandRoot
            untillVertex->edges.push_back(edge1);

            commandRoot->edges.push_back(NON_COND_EDGE(buildCommandsFlowChart(commandTree->childs[0], commandRoot, untillVertex)));


            //end of first edge is chart of commands after then statement
            
            commandRoot->edges.push_back(NON_COND_EDGE(endVertex));

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
            commandRoot->instruction.values.push_back(variableName);
            commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
        }
        case "WRITE":
        {
            commandRoot->instruction.operation=READ;
            commandRoot->instruction.values.push_back(readValue(commandTree->childs[0]));
            commandRoot->edges.push_back(NON_COND_EDGE(endVertex));
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
        deadCodeVertex->instruction.operation=OP_NULL;
        return buildCommandsFlowChart(commandsTree->childs[0], beginVertex, buildCommandFlowChart(commandsTree->childs[1], deadCodeVertex, endVertex));
    }

}

InstructionVertex buildFlowChart(ParseTreeNode* programTree)
{
    InstructionVertex* programRoot = reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
    if( !programTree->params[2].compare("VAR")  )
    {
        std::vector<Value> values;
        //zadeklaruj zmienne
        ParseTreeNode* declarations = programTree->childs[0];
        while(declarations->childs[1] != nullptr)
        {
            Value val;
            val.name=declarations->childs[1]->params[0];
            variables.insert({declarations->childs[1]->params[0], val});
            values.push_back(val);
            declarations=declarations->childs[0];
        }
        Value val;
        val.name=declarations->childs[0]->params[0];
        variables.insert({declarations->childs[0]->params[0], val});
        values.push_back(val);

        programRoot->instruction.operation=ALLOC;

        //przekopiowac po bozemu
        programRoot->instruction.values=values;

        for(int i=0; i<values.size; i++)
            programRoot->instruction.values.push_back(values[i]);
        
    }
    else
        programRoot->instruction.operation=OP_NULL;

    InstructionVertex* programEnd= reinterpret_cast<InstructionVertex*>(malloc(sizeof(InstructionVertex)));
    programEnd->instruction.operation=END;
    programRoot->edges.push_back(NON_COND_EDGE(buildCommandsFlowChart(
        programTree->childs[1], programRoot, programEnd
    )))

    return programRoot;
}

std::vector<std::string> addValues(Value & val1, Value & val2)
{
    std::vector<std::string> code;
    //if val1 is variable
    std::string i;
    if(val1.adress>-1)
    {
        //if val2 is variable
        if(val2.adress>-1)
        {
            i=std::to_string(val1.adress);
            code.push_back({"LOAD "+i, nullptr});
            i=std::to_string(val2.adress);
            code.push_back({"ADD "+i, nullptr});
        }
        else
        {
            i=std::to_string(val2.value);
            code.push_back({"SET "+ i, nullptr});
            i=std::to_string(val1.adress);
            code.push_back({"ADD "+i, nullptr});
        }
    }
    else
    {
        if(val2.adress>-1)
        {
            i=std::to_string(val1.value);
            code.push_back({"SET "+ i, nullptr});
            i=std::to_string(val2.adress);
            code.push_back({"ADD "+i, nullptr});
        }
        else
        {
            i=std::to_string(val1.value + val2.value);
            code.push_back({"SET "+i, nullptr});
        }
    }
    return code;
}

std::vector<std::string> subValues(Value & val1, Value & val2)
{
    std::vector<std::string> code;
    //if val1 is variable
    if(val1.adress>-1)
    {
        //if val2 is variable
        if(val2.adress>-1)
        {
            i=std::to_string(val1.adress);
            code.push_back({"LOAD "+i, nullptr});
            i=std::to_string(val2.adress)
            code.push_back({"SUB "+i, nullptr});
        }
        else
        {
            i=std::to_string(val2.value)
            code.push_back({"SET "+ i, nullptr});
            i=std::to_string(freeMem);
            code.push_back({"STORE " + i, nullptr})
            i=std::to_string(val1.adress)
            code.push_back({"LOAD "+i, nullptr});
            i=std::to_string(freeMem);
            code.push_back({"SUB "+i, nullptr});
        }
    }
    else
    {
        if(val2.adress>-1)
        {
            i=std::to_string(val1.value);
            code.push_back({"SET "+ , nullptr});
            i=std::to_string(val2.adress);
            code.push_back({"SUB "+, nullptr});
        }
        else
        {
            //do opt predictable wystarczy wchodzic tu gdy valeus sa >-1
            i=std::to_string(max(val1.value - val2.value,0));
            code.push_back({"SET "+ i, nullptr});
        }
    }
    return code;
}

std::vector<std::string> doubleValue(Value val)
{
    std::vector<std::string> code;
    std::string i;
    //if val is variable
    /*if(val.adress>-1)
    {
        
        i=std::to_string(val1.adress);
        code.push_back({"LOAD "+i);
        code.push_back({"ADD "+i);
        
        return addValues(val, val);
    }
    else
    {
        i=std::to_string(val1.value);
        code.push_back({"SET "+i);
        i=std::to_string(freeMem);
        code.push_back({"STORE "+i);
        code.push_back({"ADD "+i);
        return add
    }*/
    return addValues(val,val);
}

std::vector<std::string> halfValue(Value val)
{
    std::vector<std::string> code;
    std::string i;
    //if val is variable
    if(val.adress>-1)
    {
        i=std::to_string(val1.adress);
        code.push_back({"LOAD "+i, nullptr});
        code.push_back({"HALF", nullptr});
    }
    else
    {
        i=std::to_string(val1.value);
        code.push_back({"SET "+i, nullptr});
        code.push_back({"HALF", nullptr});
    }
    return code;
}

InstructionVertex* transformMultVertex( InstructionVertex* & multVertex)
{
    std::vector<InstructionEdge> outEdges;
    for(int i =0; i<multVertex->edges.size(); i++)
    {
        outEdges.push_back(multVertex->edges[i]);
    }

}

//code jest teraz globalne
//dodac jump merge
void translateAssignVertex(InstructionVertex* & assignVertex)
{
    //std::vector<Line> code;
    std::string i;
    switch (assignVertex->instruction.expression.Operator)
    {
    case null:
        
        
        if(assignVertex->instruction.expression.values[0].adress>-1)
        {
            //assign variable
            i=std::to_string(assignVertex->instruction.expression.values[0].adress);
            code.push_back({"LOAD " + i, nullptr});
        }
        else
        {
            i=std::to_string(assignVertex->instruction.expression.values[0].value);
            code.push_back({"SET " + i, nullptr});
        }
        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"STORE " + i, nullptr});
    case plus:
        code = addValues(assignVertex->instruction.expression.values[0], assignVertex->instruction.expression.values[1]);
 
        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"STORE " + i, nullptr});
        break;
    case minus:
        code = subValues(assignVertex->instruction.expression.values[0], assignVertex->instruction.expression.values[1]);
  
        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"STORE " + i, nullptr});
        break;
    case mult:
        std::vector<std::string> codeToAppend;
        //algorytm ruskich chlopow
        Value v1=assignVertex->instruction.expression.values[0];
        Value v2=assignVertex->instruction.expression.values[1];
        //initialise tmp
        i=std::to_string(0);
        code.push_back({"SET " + i, nullptr});
        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});
        Value tmp;
        //tmp.val
        tmp.adress = freeMem;
        variables.insert({"tmp", freeMem})

        //while(v2>0)  
        i=std::to_string(v2.adress);
        code.push_back({"LOAD " + i, nullptr});    
        i=std::to_string(/* skok na ost linie */);
        code.push_back({"JZERO " + i, nullptr});

        //    if(v2%2==1)
        codeToAppend= halfValue(v2);
        code.insert(std::end(code), std::begin(codeToAppend), std::end(codeToAppend));
        //double value
        i=std::to_string(freeMem+1);
        code.push_back({"STORE " + i, nullptr});
        code.push_back({"ADD " + i, nullptr});
        code.push_back({"STORE " + i, nullptr});
        //check gt
        i=std::to_string(v2.adress);
        code.push_back({"LOAD " + i, nullptr});
        i=std::to_string(freeMem+1);
        code.push_back({"SUB " + i, nullptr});
        i=std::to_string(/* skok na +2 linie */);
        code.push_back({"JZERO " + i, nullptr});
        //      tmp+=a
        i=std::to_string(freeMem);
        code.push_back({"LOAD " + i, nullptr});
        i=std::to_string(v1.adress);
        code.push_back({"ADD " + i, nullptr});
        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});//9
        //a*=2, tu jumpujemy
        codeToAppend= doubleValue(v1);
        code.insert(std::end(code), std::begin(codeToAppend), std::end(codeToAppend));
        i=std::to_string(v1.adress);
        code.push_back({"STORE " + i, nullptr});
        //b/=2
        codeToAppend= halfValue(v2);    //robilismy przed chwila, mozna zapamietac
        code.insert(std::end(code), std::begin(codeToAppend), std::end(codeToAppend));
        i=std::to_string(v2.adress);
        code.push_back({"STORE " + i, nullptr});
        //check if b>0 (do while)
        i=std::to_string(v2.adress);
        code.push_back({"LOAD " + i, nullptr});    
        i=std::to_string(/* skok do while */);
        code.push_back({"JPOS " + i, nullptr});   //tu skaczemy z while
        
        //assign p=tmp
        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"LOAD " + i, nullptr});
        break;
    case div:   //dodac case jesli v2=2^x to robimy halfem, tak samo w modulo
        //initialise tmp
        i=std::to_string(v1.adress);
        code.push_back({"LOAD " + i, nullptr});
        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});
        //initialise cnt
        i=std::to_string(0);
        code.push_back({"SET " + i, nullptr});
        i=std::to_string(freeMem+1);
        code.push_back({"STORE " + i, nullptr});
        //while(v1>v2)  
        i=std::to_string(v2.adress);
        code.push_back({"SUB " + i, nullptr});
        i=std::to_string(/* skok na ost linie */);
        code.push_back({"JZERO " + i, nullptr});

        //increase cnt
        i=std::to_string(1);
        code.push_back({"SET " + i, nullptr});//tu wracamy jumpem
        i=std::to_string(freeMem+1);
        code.push_back({"ADD " + i, nullptr});
        code.push_back({"STORE " + i, nullptr});

        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});
        

        i=std::to_string(v2.adress);
        code.push_back({"SUB " + i, nullptr});
        i=std::to_string(/* skok pare up */);
        code.push_back({"JPOS " + i, nullptr});

        //assign p=cnt
        i=std::to_string(freeMem+1);
        code.push_back({"LOAD " + i, nullptr});

        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"LOAD " + i, nullptr});
        break;
    case mod:
        //initialise tmp
        i=std::to_string(v1.adress);
        code.push_back({"LOAD " + i, nullptr});
        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});
        //while(v1>v2)  
        //i=std::to_string(v1.adress);
        //code.push_back({"LOAD " + i, nullptr});
        i=std::to_string(v2.adress);
        code.push_back({"SUB " + i, nullptr});
        i=std::to_string(/* skok na ost linie */);
        code.push_back({"JZERO " + i, nullptr});
        i=std::to_string(freeMem);
        code.push_back({"STORE " + i, nullptr});//tu wracamy jumpem
        i=std::to_string(v2.adress);
        code.push_back({"SUB " + i, nullptr});
        i=std::to_string(/* skok 2 up */);
        code.push_back({"JPOS " + i, nullptr});
        i=std::to_string(/* skok na ost linie */);
        code.push_back({"JZERO " + i, nullptr});  //tym skaczemy gdy w petle weszlismy (czyli a>b)
        //jesli a<b to wczytujemy a jako reszte z dzielenia
        i=std::to_string(v1.adress);
        code.push_back({"LOAD " + i, nullptr});

        //assign p=tmp
        i=std::to_string(assignVertex->instruction.values[0].adress);
        code.push_back({"LOAD " + i, nullptr});
        break;
    default:
        break;
    }
}


//dodac jump merge
//zwracamy jump gdy warunek true
//nullptr gdy warunek c_true
//nie zwracajmy jumpa, jumpno sie odczyta z globala
//w gte itp mamy dwa jumpy o tym samym numerze, ale nie ma jumpa na jumpa
//dodac prediction
Line* translateConditionalEdge(ConditionalEdge* edge)
{
    Value v1 = edge->condition.values[0];
    Value v2 = edge->condition.values[1];
    std::string i;
    switch(edge->condition.comparission)
    {
        case neq:
            subValues(v1, v2);
            i=std::to_string(freeMem);
            code.push_back({"STORE "+i, 0});

            subValues(v2, v1);
            i=std::to_string(freeMem);
            code.push_back({"ADD "+i, 0});

            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            return &code.end();
            break;
        case eq:
            subValues(v1, v2);
            i=std::to_string(freeMem);
            code.push_back({"STORE "+i, 0});

            subValues(v2, v1);
            i=std::to_string(freeMem);
            code.push_back({"ADD "+i, 0});

            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            return &code.end();
            break;
        case gt:
            subValues(v1, v2);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case gte:
            //neq lt
            subValues(v2, v1);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case lt:
            subValues(v2, v1);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case lte:
            //neq gt
            subValues(v1, v2);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        //nie wchodzimy:
        //case c_true:
          //  break;
        //case c_false:
          //  break;
    }
}

//dodac jump merge
Line* translateNegConditionalEdge(ConditionalEdge* edge)
{
    switch(edge->condition.comparission)
    {
        
        case eq:    //~neq
            subValues(v1, v2);
            i=std::to_string(freeMem);
            code.push_back({"STORE "+i, 0});

            subValues(v2, v1);
            i=std::to_string(freeMem);
            code.push_back({"ADD "+i, 0});

            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            return &code.end();
            break;
        case neq:   //~eq
            subValues(v1, v2);
            i=std::to_string(freeMem);
            code.push_back({"STORE "+i, 0});

            subValues(v2, v1);
            i=std::to_string(freeMem);
            code.push_back({"ADD "+i, 0});

            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            return &code.end();
            break;
        case lte:   //~gt
            subValues(v1, v2);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case lt:    //~gte
            //neq lt
            subValues(v2, v1);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case gte:    //~le
            subValues(v2, v1);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JPOS";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
        case gt:   //~lte
            //neq gt
            subValues(v1, v2);
            i=std::to_string(freeMem);
            jumps_no++;
            Line jump;
            jump.instruction="JZERO";
            jump.jump_number=jumps_no;
            code.push_back(jump);
            break;
    }

    return NULL;
}

InstructionVertex* translateDispressionVertex(InstructionVertex* & dispressionVertex)
{
    if(dispressionVertex->edges[0].condition.comparission==c_true)
    {
        InstructionVertex* activeVertex=dispressionVertex.edges[0].end;
        while(activeVertex.instruction.operation != DISPRESSION_END)
        {
            switch(activeVertex.instruction.operation)
            case ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex=activeVertex->edges[0].end;
                break;
            case WRITE:
                //translateWrite
                break;
            case READ:
                //translateRead
                break;
            case DISPRESSION_BEGIN:
                activeVertex=translateDispressionVertex(activeVertex);
                break;
            default:
                //error
        }
        return activeVertex;
    }
    else if(dispressionVertex->edges[0].condition.comparission==c_false)
    {
        InstructionVertex* activeVertex=dispressionVertex.edges[1].end;
        while(activeVertex.instruction.operation != DISPRESSION_END)
        {
            switch(activeVertex.instruction.operation)
            case ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex=activeVertex->edges[0].end;
                break;
            case WRITE:
                //translateWrite
                break;
            case READ:
                //translateRead
                break;
            case DISPRESSION_BEGIN:
                activeVertex=translateDispressionVertex(activeVertex);
                break;
            default:
                //error
        }
        return activeVertex;
    }
    //jesli program w drugim edgu jest pusty
    else if(dispressionVertex->edges[0].end->instruction.operation=DISPRESSION_END)
    {
        Line* jump = translateNegConditionalEdge(dispressionVertex->edges[0]);
        jumps_no++;
        jump->jump_number=jumps_no;
        int our_jump_no=jumps_no;
        InstructionVertex* activeVertex=dispressionVertex.edges[0].end;
        while(activeVertex.instruction.operation != DISPRESSION_END)
        {
            switch(activeVertex.instruction.operation)
            case ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex=activeVertex->edges[0].end;
                break;
            case WRITE:
                //translateWrite
                break;
            case READ:
                //translateRead
                break;
            case DISPRESSION_BEGIN:
                activeVertex=translateDispressionVertex(activeVertex);
                break;
            default:
                //error
        }
        jump_to_merge=our_jump_no;

        return activeVertex;
        
    }
    else
    {
        Line* jump1 = translateNegConditionalEdge(dispressionVertex->edges[0]);
        jumps_no++;
        jump1->jump_number=jumps_no;
        int our_jump_no1=jumps_no;

        InstructionVertex* activeVertex=dispressionVertex.edges[0].end;
        while(activeVertex.instruction.operation != DISPRESSION_END)
        {
            switch(activeVertex.instruction.operation)
            case ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex=activeVertex->edges[0].end;
                break;
            case WRITE:
                //translateWrite
                break;
            case READ:
                //translateRead
                break;
            case DISPRESSION_BEGIN:
                activeVertex=translateDispressionVertex(activeVertex);
                break;
            default:
                //error
        }
        jumps_no++;
        int our_jump_no2=jumps_no;
        //jump na end
        code.push_back({"JUMP", jumps_no});

        jump_to_merge=our_jump_no1;

        activeVertex=dispressionVertex.edges[1].end;
        while(activeVertex.instruction.operation != DISPRESSION_END)
        {
            switch(activeVertex.instruction.operation)
            case ASSIGN:
                translateAssignVertex(activeVertex);
                activeVertex=activeVertex->edges[0].end;
                break;
            case WRITE:
                //translateWrite
                break;
            case READ:
                //translateRead
                break;
            case DISPRESSION_BEGIN:
                activeVertex=translateDispressionVertex(activeVertex);
                break;
            default:
                //error
        }
        jump_to_merge=our_jump_no2;
        return activeVertex;
    }
}
