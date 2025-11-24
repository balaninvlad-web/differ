#ifndef CREATE_LOG_FILE_H
#define CREATE_LOG_FILE_H
#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "differenciator.h"

const int DUMP_NORMAL = 0;
const int DUMP_LOAD = 1;

extern const int MAX_COMMAND_LENGTH;

void Create_dump_files (Tree_t* tree, const char* file, const char* func, int line, const char* reason, ...);
void Create_html_file (Tree_t* tree, int dump_counter, const char* graph_filename, const char* func, const char* formatted_reason, ...);
void Create_head_html (FILE** html_file);
void PrintBuffer (FILE* html_file, const char* buffer, size_t position);
void Close_html_file (void);

void Create_log_file (Tree_t* tree, const char* filename, int dump_type, LoadProgress* progress);
void Create_head_log_file (FILE* dot_file);

void Create_load_graph (Tree_t* tree, FILE* dot_file, LoadProgress* progress);
void Create_load_node (Tree_t* tree, FILE* dot_file, Node_t* node, size_t rank);
void Create_load_arrows (Tree_t* tree, FILE* dot_file, Node_t* node);

void Create_graph_node (Tree_t* tree, FILE* dot_file, Node_t* node);
void Make_arrow (Tree_t* tree, FILE* dot_file, Node_t* node);
void Create_picture (void);

const char* GetTypeName (int type);
const char* GetOperatorName (int value);
const char* GetVariableName (int value);
void EscapeHtml (FILE* html_file, const char* text, size_t len);

#endif
