// create_latex_dump_adapted.h
#ifndef CREATE_LATEX_DUMP_ADAPTED_H
#define CREATE_LATEX_DUMP_ADAPTED_H

#include "differenciator.h"

const int MAX_LATEX_EXPRESSION_LENGTH = 1000;

struct LatexDumpState
{
    FILE* file;
    int step_counter;
    const char*  filename;
    const char** funny_phrases;
    int funny_phrases_count;
    int current_funny_index;
};

const char* get_funny_phrase ();
void NodeToLatexString (Node_t* node, char* buffer, int* pos, int buffer_size);
int StartLatexDump (FILE* file);
int EndLatexDump (FILE* file);
int DumpOriginalFunctionToFile (FILE* file, Tree_t* tree, double result_value);
int DumpOptimizationStepToFile (FILE* file, const char* description, Tree_t* tree, double result_value);
int DumpDerivativeToFile (FILE* file, Tree_t* derivative_tree, double derivative_result, int derivative_order);
void CtorLatexDump (LatexDumpState* state, const char* filename);
void AddLatexStep (LatexDumpState* state, const char* description, Tree_t* tree);
void DtorLatexDump (LatexDumpState* state);
void print_tree_latex (FILE* file, Node_t* node);
void LatexDumpDifferentiationStep (LatexDumpState* state, const char* rule_used, Tree_t* original, Tree_t* derivative);
void LatexDumpSimplifyStep (LatexDumpState* state, const char* rule_used, Tree_t* before, Tree_t* after);
void LatexDumpDetailedProcess (Tree_t* original, Tree_t* derivative);
void CreateAdaptedLatexDump (Tree_t* original_tree, Tree_t* diff_tree, int is_simplified);

#endif
