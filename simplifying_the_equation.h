#ifndef SIMPLIFYING_THE_EQUATION_H
#define SIMPLIFYING_THE_EQUATION_H

bool SimplificationTree(Node_t* node, Tree_t* tree, bool* changed);
bool SimplifyUntilStable(Tree_t* tree, int max_iterations);
void ChangeNodeToNumber(Node_t* node, double num_value, Tree_t* tree);
void SimplificatoinPrimeNumbers(Node_t* node, Tree_t* tree, bool* changed);
double TryCalculateNode(Node_t* node);
void ReplaceWithLeftChild(Node_t* node);
void ReplaceWithRightChild(Node_t* node);
int IsZero(Node_t* node);
int IsOne(Node_t* node);
int AreNodesEqual(Node_t* a, Node_t* b);
int AreNodeValuesEqual(union NodeValue a, union NodeValue b);
void LatexDumpSimplifyStep(Tree_t* tree, const char* rule);

#endif
