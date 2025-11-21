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
#define LOADING_BASE_DUMP(tree, buf, pos, reason, progress) Create_dump_files(tree, __FILE__, __func__, __LINE__, reason, DUMP_LOAD, buf, pos, progress)

extern const int DUMMY_VALUE;
const int MAX_STR_SIZE = 1000;
const int MAX_VARIABLES = 10;
const int MAX_COMMAND_SIZE = 7;

typedef struct my_stack_t my_stack_t;
typedef int TreeErr_t;
typedef char* tree_elem_t;

typedef struct Node_t
{
    int type;
    int value;
    struct Node_t* left;
    struct Node_t* right;
} Node_t;

typedef struct Tree_t
{
    Node_t* root;
    size_t size;
} Tree_t;

typedef struct
{
    const char* name;
    int value;
    int type;
} StructCmd;

const int MAX_COUNT_COMMAND = 28;//TODO ykazat skolko

typedef struct
{
    Node_t* node;
    size_t rank;
} LoadNodeInfo;

typedef struct
{
    LoadNodeInfo* nodes;
    size_t capacity;
    size_t size;
    size_t current_rank;
} LoadProgress;

typedef enum
{
    YES     = 0,
    NO      = 1,
    UNKNOWN = 2
} UserAnswer;


enum
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

typedef enum
{
    OPERATORTYPE = 0,
    VARIABLETYPE = 1,
    NUMBERTYPE   = 2
} TypeArgument;

typedef enum
{
    ADD     = 1,
    SUB     = 2,
    MUL     = 3,
    DIV     = 4,
    POW     = 5, //TODO dodelat
} TypeValue;

typedef enum nameOFregistr
{
    ARGX = 1,
    ARGY = 2,
    ARGZ = 3,
} NameVariable;

typedef struct
{
    char name;
    double value;
} Variable_t;

extern StructCmd command_table[MAX_COUNT_COMMAND];

TreeErr_t TreeCtor (Tree_t** tree);
Node_t* NodeCtor (Tree_t* tree, int type, int value);
TreeErr_t TreeDtor (Tree_t* tree);

TreeErr_t PrintNode (const Node_t* node);

TreeErr_t TreeVerify (Tree_t* tree);
TreeErr_t TreeVerifyNode (Tree_t* tree, Node_t* node, Node_t** visited, size_t* counter);

TreeErr_t TreeInsertRoot (Tree_t* tree, int type, int value);
TreeErr_t TreeInsertLeft (Tree_t* tree, Node_t* parent, int type, int value);
TreeErr_t TreeInsertRight (Tree_t* tree, Node_t* parent, int type, int value);
Node_t* TreeInsertNode (Tree_t* tree, int type, int value);
TreeErr_t DeleteSubtree (Tree_t* tree, Node_t** node);


Node_t* LoadTreeFromFile (Tree_t* tree, const char** buffer, int* pos_in_buffer, LoadProgress* progress, const char* buffer_start);
TreeErr_t LoadDatabase (Tree_t* tree, const char* filename);

const char* SkipSpaces (const char* str, int* pos_in_buffer);
long getFileSize(FILE* file);
void DetermineType(const char* data, int* type, int* value);
const char* ReadCommand(const char* current, int* pos_in_buffer, int* type, int* value);

void CreateDefaultTree(Tree_t* tree);

void CtorLoadProgress (LoadProgress* progress);
void AddNodeToLoadProgress (LoadProgress* progress, Node_t* node);
void DtorLoadProgress (LoadProgress* progress);

double EvaluateOperator(Node_t* node, Variable_t* variables, int var_count);
double EvaluateNodeAdvanced(Node_t* node, Variable_t* variables, int var_count);
int CollectVariables(Node_t* node, int* variables);
double EvaluateTreeAdvanced(Tree_t* tree);
char GetterNameVariable (int variable_code);

#endif
