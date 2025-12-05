#ifndef SIMPLIFYING_THE_EQUATION_H
#define SIMPLIFYING_THE_EQUATION_H

void ChangeNodeToNumber (Node_t* node, double num_value, Tree_t* tree);
double TryCalculateNode (Node_t* node);
void ReplaceWithLeftChild (Node_t* node);
void ReplaceWithRightChild (Node_t* node);
int IsZero (Node_t* node);
int IsOne (Node_t* node);
int AreNodesEqual (Node_t* a, Node_t* b);
int AreNodeValuesEqual (union NodeValue a, union NodeValue b);
void LatexDumpSimplifyStep (Tree_t* tree, const char* rule);
bool SimplifyUntilStable (Tree_t* tree, int max_iterations, LatexDumpState* latex_dump);
bool SimplificationTree (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplificatoinPrimeNumbers (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);

void SimplifyingSub (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingMul (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingDiv (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingPow (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingAdd (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump);
#endif
