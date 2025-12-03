#ifndef READ_FROM_FILE_NEW_H
#define READ_FROM_FILE_NEW_H


struct MathParserContext
{
    Tree_t* tree;
    const char* buffer;
    int current_pos;
    const char* original;

    LoadNodeInfo* nodes;
    size_t nodes_size;
    size_t nodes_capacity;
};

static void CtorMathParser (MathParserContext* context, Tree_t* tree, const char* expression);
static void DtorMathParser (MathParserContext* context);
static void AddNodeToMathParser (MathParserContext* context, Node_t* node, int level);
static char GetCurrentChar (MathParserContext* context);
static void Advance (MathParserContext* context, int count);
static void SkipSpaces (MathParserContext* context);
static Node_t* GetG (MathParserContext* context);
static Node_t* GetE (MathParserContext* context, int current_level);
static Node_t* GetT (MathParserContext* context, int current_level);
static Node_t* GetF (MathParserContext* context, int current_level);
static Node_t* GetP (MathParserContext* context, int current_level);
static Node_t* GetN (MathParserContext* context, int current_level);
static Node_t* GetV (MathParserContext* context, int current_level);
static Node_t* GetFunction (MathParserContext* context, int current_level);

#endif
