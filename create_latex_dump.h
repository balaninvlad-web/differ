// create_latex_dump_adapted.h
#ifndef CREATE_LATEX_DUMP_ADAPTED_H
#define CREATE_LATEX_DUMP_ADAPTED_H

#include "differenciator.h"

const int MAX_LATEX_EXPRESSION_LENGTH = 1000;

// Функции для LaTeX дампа
void NodeToLatexString(Node_t* node, char* buffer, int* pos, int buffer_size);
int StartLatexDump(FILE* file);
int EndLatexDump(FILE* file);
int DumpOriginalFunctionToFile(FILE* file, Tree_t* tree, double result_value);
int DumpOptimizationStepToFile(FILE* file, const char* description, Tree_t* tree, double result_value);
int DumpDerivativeToFile(FILE* file, Tree_t* derivative_tree, double derivative_result, int derivative_order);
const char* get_funny_phrase();
void CreateAdaptedLatexDump(Tree_t* original_tree, Tree_t* diff_tree, int is_simplified);

void CreateFullLatexDump(const char* filename, Tree_t* original_tree, Tree_t* diff_tree, int is_simplified, int max_iterations);
void LatexDumpDiffStep(Tree_t* tree, const char* rule);
void LatexDumpSimplifyStep(Tree_t* tree, const char* rule);

#endif
