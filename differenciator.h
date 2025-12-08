#ifndef DIFFERENCIATOR_H
#define DIFFERENCIATOR_H

#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define QUICK_DUMP(tree, reason, ...) Create_dump_files(tree, __FILE__, __func__, __LINE__, reason, ##__VA_ARGS__)
#define LOADING_BASE_DUMP(tree, buf, pos, reason, progress, buffer_start) Create_dump_files(tree, __FILE__, __func__, __LINE__, reason, DUMP_LOAD, buf, pos, progress)

const int MAX_STR_SIZE = 1000;
const int MAX_LOOP_SIMPLE = 40;
const int MAX_VARIABLES = 10;
const int MAX_COMMAND_SIZE = 7;

//typedef struct my_stack_t my_stack_t;
typedef int TreeErr_t;
typedef char* tree_elem_t;

union NodeValue
{
    int number;
    int operator_type;
    int variable_code;
    double constant;
};

struct Node_t
{
    int type;
    union NodeValue value;
    struct Node_t* left;
    struct Node_t* right;
};

struct Tree_t
{
    Node_t* root;
    size_t size;
};

struct StructCmd
{
    const char* name;
    union NodeValue value;
    int type;
};

//const int MAX_COUNT_COMMAND = ;//TODO ykazat skolko

struct LoadNodeInfo
{
    Node_t* node;
    size_t rank;
};

struct LoadProgress
{
    LoadNodeInfo* nodes;
    size_t capacity;
    size_t size;
    size_t current_rank;
};

enum UserAnswer
{
    YES     = 0,
    NO      = 1,
    UNKNOWN = 2
};


enum TypeErorr
{
    NOERORR = 0,
    ERORRTREENULL,
    ERORRNODENULL,
    ERORRDUMMYALLOC,
    ERORRTREEALLOC,
    ERORRINVALIDROOT,
    ERORRCYCLE,
    ERORRININSERT,
    ERORRFILEOPEN,
    ERORRBUFFER
};

enum TypeArgument
{
    OPERATORTYPE = 0,
    VARIABLETYPE = 1,
    NUMBERTYPE   = 2
};

enum TypeValue
{
    ADD     = 1,
    SUB     = 2,
    MUL     = 3,
    DIV     = 4,
    POW     = 5, //TODO dodelat
    SIN     = 6,
    COS     = 7,
    TAN     = 8,
    LN      = 9,
    EXP     = 10
};

enum NameVariable
{
    ARGX = 1,
    ARGY = 2,
    ARGZ = 3,
    ARGA = 4,
    ARGB = 5,
};

struct Variable_t
{
    char name;
    double value;
};

TreeErr_t TreeCtor (Tree_t** tree);
Node_t* NodeCtor (Tree_t* tree, int type, union NodeValue value);
TreeErr_t TreeDtor (Tree_t* tree);

TreeErr_t PrintNode (const Node_t* node);

TreeErr_t TreeVerify (Tree_t* tree);
TreeErr_t TreeVerifyNode (Tree_t* tree, Node_t* node, Node_t** visited, size_t* counter);

TreeErr_t TreeInsertRoot (Tree_t* tree, int type, union NodeValue value);
TreeErr_t TreeInsertLeft (Tree_t* tree, Node_t* parent, int type, union NodeValue value);
TreeErr_t TreeInsertRight (Tree_t* tree, Node_t* parent, int type, union NodeValue value);
Node_t* TreeInsertNode (Tree_t* tree, int type, union NodeValue value);
TreeErr_t DeleteSubtree (Tree_t* tree, Node_t** node);


TreeErr_t LoadMathExpressionFromFile (Tree_t* tree, const char* filename);
TreeErr_t LoadMathExpression (Tree_t* tree, const char* expression);

const char* SkipSpaces (const char* str, int* pos_in_buffer);
long getFileSize (FILE* file);
void DetermineType (const char* data, int* type, union NodeValue* value);
const char* ReadCommand (const char* current, int* pos_in_buffer, int* type, union NodeValue* value);

void CreateDefaultTree (Tree_t* tree);

void CtorLoadProgress (LoadProgress* progress);
void AddNodeToLoadProgress (LoadProgress* progress, Node_t* node, int level);
void DtorLoadProgress (LoadProgress* progress);

double EvaluateOperator (Node_t* node, Variable_t* variables, int var_count);
double EvaluateNodeAdvanced (Node_t* node, Variable_t* variables, int var_count);
int CollectVariables (Node_t* node, int* variables);
double EvaluateTreeAdvanced (Tree_t* tree);
char GetterNameVariable (int variable_code);
double GetVariableValue (char var_name, Variable_t* variables, int var_count);
int CheckVariableExists (int var_code, int* variables, int count);
double EvaluateAtPoint (Tree_t* tree, char variable, double value);

struct Node_t* CopySubtree (struct Tree_t* tree, struct Node_t* node);
Node_t* DifferentiateNode (Tree_t* tree, Node_t* node, char variable);
struct Node_t* DifferentiateOperator (struct Tree_t* tree, struct Node_t* node, char variable);
struct Node_t* NewNode (struct Tree_t* tree, int type, union NodeValue value, struct Node_t* left, struct Node_t* right);

Node_t* CreateNumberNode (Tree_t* tree, int number);
Node_t* CreateVariableNode (Tree_t* tree, int variable_code);
Node_t* CreateOperatorNode (Tree_t* tree, int operator_type, Node_t* left, Node_t* right);

//#include "create_latex_dump.h"

#endif // DIFFERENCIATOR_H
