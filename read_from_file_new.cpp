#include <math.h>
#include "differenciator.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"
#include "create_dump_files.h"
#include "read_from_file_new.h"

static void CtorMathParser (MathParserContext* context, Tree_t* tree, const char* expression)
{
    context->tree = tree;
    context->buffer = expression;
    context->current_pos = 0;
    context->original = expression;

    context->nodes_capacity = 100; // TODO поменять
    context->nodes_size = 0;
    context->nodes = (LoadNodeInfo*) calloc (context->nodes_capacity, sizeof(Node_t*));
}

static void DtorMathParser (MathParserContext* context)
{
    free (context->nodes);
    context->nodes = NULL;
    context->nodes_capacity = 0;
    context->nodes_size = 0;
}

static void AddNodeToMathParser (MathParserContext* context, Node_t* node, int level)
{
    if (context->nodes_size >= context->nodes_capacity)
    {
        context->nodes_capacity *= 2;
        context->nodes = (LoadNodeInfo*) realloc(context->nodes,
                                context->nodes_capacity * sizeof(*context->nodes));
    }
    context->nodes[context->nodes_size].node = node;
    context->nodes[context->nodes_size].rank = level;
    context->nodes_size++;

    #ifdef DEBUG
        printf ("DEBUG: Added node to math parser array: level=%d, total=%d\n",level, context->nodes_size);
    #endif
}

TreeErr_t LoadMathExpressionFromFile (Tree_t* tree, const char* filename)
{
    assert (tree && filename);

    #ifdef DEBUG
        printf ("DEBUG: LoadMathExpressionFromFile called for: %s\n", filename);
    #endif

    FILE* file = fopen (filename, "rb");

    if (!file)
    {
        printf ("Cannot open file %s\n", filename);
        return ERORRFILEOPEN;
    }

    long file_size = getFileSize (file);

    if (file_size <= 0)
    {
        fclose(file);
        return ERORRFILEOPEN;
    }

    char* expression = (char*) calloc (file_size + 2, sizeof(char)); // +2 для '$' и '\0'
    if (!expression)
    {
        fclose(file);
        return ERORRBUFFER;
    }

    fread (expression, 1, file_size, file);
    fclose (file);

    while (file_size > 0 && isspace((unsigned char) expression[file_size - 1]))
    {
        expression[file_size - 1] = '\0';
        file_size--;
    }

    size_t len = strlen (expression);

    if (len > 0 && expression[len - 1] != '$')
    {
        if (len + 1 < (size_t) file_size + 2)
        {
            expression[len] = '$';
            expression[len + 1] = '\0';
        }
    }

    #ifdef DEBUG
        printf("DEBUG: Expression from file: %s\n", expression);
    #endif

    TreeErr_t result = LoadMathExpression (tree, expression);

    free (expression);

    return result;
}


static Node_t* GetG (MathParserContext* context)
{
    #ifdef DEBUG
        printf ("DEBUG: MathParser GetG started at pos=%d\n", context->current_pos);
    #endif
    Node_t* val = GetE (context, 0);

    if (!val)
    {
        #ifdef DEBUG
            //LOADING_BASE_DUMP(context->tree, context->buffer, context->current_pos,"MathParser GetG failed", (LoadProgress*)context, context->original);
        #endif

        return NULL;
    }

    SkipSpaces (context);

    if (GetCurrentChar (context) != '$' && GetCurrentChar (context) != '\0')
    {
        #ifdef DEBUG
            printf ("DEBUG: ERROR: Expected end of expression but found '%c'\n", GetCurrentChar (context));
            //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos, "MathParser expected end '$'", (LoadProgress*)context, context->original);
        #endif

        DeleteSubtree (context->tree, &val);
        return NULL;
    }

    //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos,"MathParser GetG completed", (LoadProgress*)context, context->original);
    return val;
}

static Node_t* GetE (MathParserContext* context, int current_level)
{
    Node_t* val = GetT (context, current_level + 1);

    if (!val)
        return NULL;

    while (GetCurrentChar(context) == '+' || GetCurrentChar (context) == '-')
    {
        char op_char = GetCurrentChar (context);
        int op = (op_char == '+') ? ADD : SUB;

        Advance(context, 1);
        SkipSpaces(context);

        Node_t* val2 = GetT (context, current_level + 1);

        if (!val2)
        {
            DeleteSubtree (context->tree, &val);
            return NULL;
        }

        Node_t* new_node = CreateOperatorNode (context->tree, op, val, val2);

        if (!new_node)
        {
            DeleteSubtree (context->tree, &val);
            DeleteSubtree (context->tree, &val2);
            return NULL;
        }

        AddNodeToMathParser (context, new_node, current_level);

        #ifdef DEBUG
            //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos,"Created binary operator", (LoadProgress*)context, context->original);
        #endif

        val = new_node;
    }

    return val;
}

static Node_t* GetF (MathParserContext* context, int current_level)
{
    Node_t* val = GetP (context, current_level + 1);

    if (!val)
        return NULL;

    while (GetCurrentChar(context) == '^')
    {
        Advance (context, 1);
        SkipSpaces (context);

        Node_t* exponent = GetP(context, current_level + 1);

        if (!exponent)
        {
            DeleteSubtree (context->tree, &val);
            return NULL;
        }

        Node_t* new_node = CreateOperatorNode(context->tree, POW, val, exponent);

        if (!new_node)
        {
            DeleteSubtree (context->tree, &val);
            DeleteSubtree (context->tree, &exponent);
            return NULL;
        }

        AddNodeToMathParser (context, new_node, current_level);

        #ifdef DEBUG
            //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos, "Created power operator",(LoadProgress*)context, context->original);
        #endif

        val = new_node;
    }

    return val;
}

static Node_t* GetT (MathParserContext* context, int current_level)
{
    Node_t* val = GetF (context, current_level + 1);

    if (!val)
        return NULL;

    while (GetCurrentChar (context) == '*' || GetCurrentChar (context) == '/')
    {
        char op_char = GetCurrentChar (context);
        int op = (op_char == '*') ? MUL : DIV;

        Advance (context, 1);
        SkipSpaces (context);

        Node_t* val2 = GetF (context, current_level + 1);
        if (!val2)
        {
            DeleteSubtree (context->tree, &val);
            return NULL;
        }

        Node_t* new_node = CreateOperatorNode (context->tree, op, val, val2);

        if (!new_node)
        {
            DeleteSubtree (context->tree, &val);
            DeleteSubtree (context->tree, &val2);
            return NULL;
        }

        AddNodeToMathParser (context, new_node, current_level);

        #ifdef DEBUG
            //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos,"Created MUL/DIV operator", (LoadProgress*)context, context->original);
        #endif

        val = new_node;
    }

    return val;
}

static Node_t* GetN (MathParserContext* context, int current_level)
{
    #ifdef DEBUG
        printf ("DEBUG: GetN at pos=%d, char='%c'\n", context->current_pos, GetCurrentChar(context));
    #endif

    if (!isdigit(GetCurrentChar (context)))
        return NULL;

    int val = 0;
    const char* start = context->buffer + context->current_pos;

    while (isdigit(GetCurrentChar (context)))
    {
        val = val * 10 + (GetCurrentChar (context) - '0');
        Advance (context, 1);
    }

    if (context->buffer + context->current_pos == start)
        return NULL;

    Node_t* num_node = CreateNumberNode (context->tree, val);
    if (num_node)
    {
        AddNodeToMathParser (context, num_node, current_level);
        //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos,"Created number node", (LoadProgress*)context, context->original);
    }

    return num_node;
}

static Node_t* GetV (MathParserContext* context, int current_level)
{
    #ifdef DEBUG
        printf ("DEBUG: GetV at pos=%d, char='%c'\n", context->current_pos, GetCurrentChar(context));
    #endif

    char current_char = GetCurrentChar (context);

    if (!isalpha(current_char))
        return NULL;

    int var_code = 0;
    switch (current_char)
    {
        case 'x': var_code = ARGX; break;
        case 'y': var_code = ARGY; break;
        case 'z': var_code = ARGZ; break;
        case 'A': var_code = ARGA; break;
        case 'B': var_code = ARGB; break;
        default:
            printf("DEBUG: ERROR: Unknown variable '%c'\n", current_char);
            var_code = ARGA;
            return NULL;
    }

    Advance(context, 1);

    Node_t* var_node = CreateVariableNode (context->tree, var_code);
    if (var_node)
    {
        AddNodeToMathParser (context, var_node, current_level);
        //LOADING_BASE_DUMP(context->tree, context->buffer, context->current_pos, "Created variable node", (LoadProgress*)context, context->original);
    }

    return var_node;
}

static Node_t* GetFunction (MathParserContext* context, int current_level)
{
    #ifdef DEBUG
        printf ("DEBUG: GetFunction at pos=%d\n", context->current_pos);
    #endif

    const char* original_pos = context->buffer + context->current_pos;

    char func_name[MAX_COMMAND_SIZE] = {};
    int i = 0;

    while (isalpha (GetCurrentChar (context)) && i < MAX_COMMAND_SIZE - 1)
    {
        func_name[i++] = GetCurrentChar (context);
        Advance (context, 1);
    }

    if (i == 0)
        return NULL;

    // Определяем тип функции
    int op_type = 0;
    if      (strcmp (func_name, "sin") == 0) op_type = SIN;
    else if (strcmp (func_name, "cos") == 0) op_type = COS;
    else if (strcmp (func_name, "tan") == 0) op_type = TAN;
    else if (strcmp (func_name, "ln" ) == 0) op_type = LN;
    else if (strcmp (func_name, "exp") == 0) op_type = EXP;
    else
    {
        context->current_pos = original_pos - context->buffer;
        return NULL;
    }

    SkipSpaces (context);

    if (GetCurrentChar(context) != '(')
    {
        context->current_pos = original_pos - context->buffer;
        return NULL;
    }

    Advance(context, 1);
    SkipSpaces(context);

    Node_t* arg = GetE (context, current_level + 1);

    if (!arg)
        return NULL;

    SkipSpaces (context);

    if (GetCurrentChar (context) != ')')
    {
        printf ("DEBUG: ERROR: Expected closing parenthesis after function %s\n", func_name);

        //LOADING_BASE_DUMP(context->tree, context->buffer, context->current_pos,"MathParser expected closing ')' after function",(LoadProgress*)context, context->original);

        DeleteSubtree (context->tree, &arg);
        return NULL;
    }

    Advance(context, 1);


    Node_t* func_node = CreateOperatorNode (context->tree, op_type, arg, NULL);

    if (func_node)
    {
        AddNodeToMathParser (context, func_node, current_level);
        //LOADING_BASE_DUMP(context->tree, context->buffer, context->current_pos,"Created function node",(LoadProgress*)context, context->original);
    }

    return func_node;
}

static Node_t* GetP (MathParserContext* context, int current_level)
{
    SkipSpaces (context);

    if (GetCurrentChar (context) == '(')
    {
        Advance (context, 1);
        SkipSpaces (context);

        Node_t* val = GetE (context, current_level + 1);

        if (!val)
            return NULL;

        SkipSpaces (context);

        if (GetCurrentChar (context) == ')')
        {
            Advance (context, 1);
            return val;
        }
        else
        {
            printf ("DEBUG: ERROR: Expected closing parenthesis\n");
            //LOADING_BASE_DUMP(context->tree, context->buffer, context->current_pos,"MathParser losted ')'", (LoadProgress*)context, context->original);

            DeleteSubtree (context->tree, &val);
            return NULL;
        }
    }

    Node_t* func_node = GetFunction (context, current_level + 1);

    if (func_node)
        return func_node;

    Node_t* num_node = GetN (context, current_level + 1);

    if (num_node)
        return num_node;

    Node_t* var_node = GetV (context, current_level + 1);

    if (var_node)
        return var_node;

    #ifdef DEBUG
        //LOADING_BASE_DUMP (context->tree, context->buffer, context->current_pos, "MathParser GetP failed",(LoadProgress*)context, context->original);
    #endif
    return NULL;
}

TreeErr_t LoadMathExpression (Tree_t* tree, const char* expression)
{
    assert (tree && expression);

    #ifdef DEBUG
        printf("DEBUG: LoadMathExpression called for: %s\n", expression);
    #endif

    MathParserContext context = {};
    CtorMathParser (&context, tree, expression);

    QUICK_DUMP (tree, "Before math expression parsing");

    tree->root = GetG(&context);

    #ifdef DEBUG
        printf("DEBUG: Math parsing completed, root: %p\n", (void*)tree->root);
    #endif

    DtorMathParser (&context);

    QUICK_DUMP (tree, "After math expression parsing");

    if (!tree->root)
    {
        printf ("Failed to parse math expression: %s\n", expression);
        return ERORRTREENULL;
    }

    printf ("Math expression parsed successfully: %s\n", expression);
    return NOERORR;
}

static char GetCurrentChar (MathParserContext* context)
{
    return context->buffer[context->current_pos];
}

static void Advance (MathParserContext* context, int count)
{
    context->current_pos += count;
}

static void SkipSpaces(MathParserContext* context)
{
    while (isspace (GetCurrentChar (context)) && GetCurrentChar (context) != '\0')
        Advance (context, 1);
}

void CreateDefaultTree (Tree_t* tree)
{
    assert (tree);

    TreeInsertRoot (tree, OPERATORTYPE, (union NodeValue){.operator_type = MUL});
    TreeInsertLeft (tree, tree->root, VARIABLETYPE, (union NodeValue){.variable_code = ARGX});
    TreeInsertRight (tree, tree->root, OPERATORTYPE, (union NodeValue){.operator_type = ADD});
    TreeInsertLeft (tree, tree->root->right, VARIABLETYPE, (union NodeValue){.variable_code = ARGY});
    TreeInsertRight (tree, tree->root->right, NUMBERTYPE, (union NodeValue){.number = 6});
}

long getFileSize (FILE* file)
{
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}
