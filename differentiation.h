#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H

Node_t* CopySubtree(Tree_t* tree, Node_t* node);
Node_t* DifferentiateNode(Tree_t* tree, Node_t* node, char variable);
Node_t* DifferentiateOperator(Tree_t* tree, Node_t* node, char variable);
Node_t* NewNode(Tree_t* tree, int type, int value, Node_t* left, Node_t* right);

#endif
