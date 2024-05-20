#ifndef PROCEDURE_HPP
#define PROCEDURE_HPP

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include "utils.hpp"




void neqCond(ProcedureCondition & cond);

ProcedureValue PROCEDURE_NULL_VALUE();

ProcedureCondition PROCEDURE_TRUE_CONDITION();

ProcedureEdge PROCEDURE_NON_COND_EDGE(std::shared_ptr<ProcedureInstructionVertex> end);

std::string procedureReadValue(std::shared_ptr<ParseTreeNode> & valueNode);
ProcedureExpression procedureReadExpression(std::shared_ptr<ParseTreeNode> & expressionTree);
ProcedureCondition procedureReadCondition(std::shared_ptr<ParseTreeNode> & conditionTree);

std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandFlowChart(std::shared_ptr<ParseTreeNode> &commandTree, std::shared_ptr<ProcedureInstructionVertex> &beginVertex, std::shared_ptr<ProcedureInstructionVertex> &endVertex);
std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<ProcedureInstructionVertex> &endVertex);
std::shared_ptr<ProcedureInstructionVertex> buildProcedureCommandsFlowChart(std::shared_ptr<ParseTreeNode> &commandsTree, std::shared_ptr<ProcedureInstructionVertex> &beginVertex, std::shared_ptr<ProcedureInstructionVertex> &endVertex);
std::shared_ptr<ProcedureInstructionVertex> buildProcedureFlowChart(Procedure & procedure);

void addValues(Value val1, Value val2);
void subValues(Value val1, Value val2);
void doubleValue(Value val);

void translateReadVertex(std::shared_ptr<ProcedureInstructionVertex> & readVertex, Procedure procedure);
void translateWriteVertex(std::shared_ptr<ProcedureInstructionVertex> & readVertex, Procedure procedure);
void translateAssignVertex(std::shared_ptr<ProcedureInstructionVertex> & assignVertex, Procedure procedure);
void transalteProcedureVertex(std::shared_ptr<ProcedureInstructionVertex> procedureVertex, Procedure universe);
void translateAllocVertex(std::shared_ptr<ProcedureInstructionVertex> & allocVertex, Procedure & procedure);
//returns pointer for jump
void translateConditionalEdge(ProcedureEdge* edge, Procedure procedure);

std::shared_ptr<ProcedureInstructionVertex> getDispressionEnd(std::shared_ptr<ProcedureInstructionVertex> & dispressionVertex, Procedure procedure);
std::shared_ptr<ProcedureInstructionVertex> translateDispressionVertex(std::shared_ptr<ProcedureInstructionVertex> & dispressionVertex, Procedure procedure);

extern std::map<std::string, Procedure> procedures;

#endif 