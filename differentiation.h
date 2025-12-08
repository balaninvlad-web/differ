#ifndef DIFFERENTIATION_H
#define DIFFERENTIATION_H

#include "differenciator.h"
#include "create_latex_dump.h"

#define dL                  DifferentiateNode (tree, node->left,  variable)
#define dR                  DifferentiateNode (tree, node->right, variable)
#define cL                  CopySubtree (tree, node->left)
#define cR                  CopySubtree (tree, node->right)
#define DIV_(var1, var2)    CreateOperatorNode(tree, DIV, var1, var2)
#define MUL_(var1, var2)    CreateOperatorNode(tree, MUL, var1, var2)
#define SUB_(var1, var2)    CreateOperatorNode(tree, SUB, var1, var2)
#define ADD_(var1, var2)    CreateOperatorNode(tree, ADD, var1, var2)
#define POW_(var1, var2)    CreateOperatorNode(tree, POW, var1, var2)
#define SIN_(var1)          CreateOperatorNode(tree, SIN, var1, NULL)
#define COS_(var1)          CreateOperatorNode(tree, COS, var1, NULL)
#define LN_(var1)           CreateOperatorNode(tree, LN, var1, NULL)
#define TAN_(var1)          CreateOperatorNode(tree, TAN, var1, NULL)
#define EXP_(var1)          CreateOperatorNode(tree, EXP, var1, NULL)
#define NUM_VALUE(x) ((union NodeValue){.number = (x)})
#define VAR_VALUE(x) ((union NodeValue){.variable_code = (x)})
#define OP_VALUE(x)  ((union NodeValue){.operator_type = (x)})

#define NUM_(value)         CreateNumberNode(tree, value)
#define VAR_(code)          CreateVariableNode(tree, code)

Node_t* DifferentiateTreeN (Tree_t* tree, char variable, int n, LatexDumpState* latex_dump);
void SubstitudeVariableWithValue(Node_t* node, char variable, double value);

#endif
