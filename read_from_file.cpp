#include "differenciator.h"
#include "create_dump_files.h"

const char* SkipSpaces (const char* str, int* pos_in_buffer)
{
    assert(str);
    assert(pos_in_buffer);

    while (*str && isspace((unsigned char)*str))
    {
        str++;
        (*pos_in_buffer)++;
    }

    return str;
}

const char* ReadCommand(const char* current, int* pos_in_buffer, int* type, union NodeValue* value)
{
    assert(current);
    assert(pos_in_buffer);
    assert(type);
    assert(value);

    char data[MAX_STR_SIZE] = {};
    int i = 0;

    while (*current && !isspace((unsigned char)*current) &&
           *current != ')' && *current != '(' && i < MAX_STR_SIZE - 1)
    {
        data[i++] = *current++;
        (*pos_in_buffer)++;
    }
    data[i] = '\0';

    #ifdef DEBUG
        printf ("DEBUG: ReadCommand type: '%d'\n", *type);
    #endif

    DetermineType(data, type, value);

    #ifdef DEBUG
        printf ("DEBUG: ReadCommand result: type=%d", *type);
    #endif

    return current;
}

Node_t* LoadTreeFromFile(Tree_t* tree, const char** buffer, int* pos_in_buffer,
                         LoadProgress* progress, const char* buffer_start, int current_level)
{
    assert(tree);
    assert(buffer);
    assert(pos_in_buffer);

    #ifdef DEBUG
        printf ("DEBUG: LoadTreeFromFile called, buffer starts with: '%.10s...'\n", *buffer);
    #endif

    const char* current = SkipSpaces (*buffer, pos_in_buffer);

    #ifdef DEBUG
        printf ("DEBUG: After SkipSpaces: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);

        LOADING_BASE_DUMP (tree, *buffer, *pos_in_buffer, "Start making node", progress, buffer_start);
    #endif

    if (*current == '(')
    {
        #ifdef DEBUG
            printf ("DEBUG: Found opening bracket, starting new node\n");
        #endif

        current = SkipSpaces(current + 1, pos_in_buffer);

        #ifdef DEBUG
            printf ("DEBUG: After skipping spaces after '(': '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        int type  = 0;
        union NodeValue value = {};

        current = ReadCommand (current, pos_in_buffer, &type, &value);

        Node_t* node = NodeCtor (tree, type, value);

        if (!node)
        {
            printf("DEBUG: ERROR: NodeCtor failed!\n");
            return NULL;
        }

        #ifdef DEBUG
            printf ("DEBUG: Node created successfully at %p\n", (void*)node);
            LOADING_BASE_DUMP (tree, *buffer, *pos_in_buffer, "Node created", progress, buffer_start);
        #endif

        if (progress) AddNodeToLoadProgress (progress, node, current_level);

        // TODO: GetNodeInfo

        current = SkipSpaces (current, pos_in_buffer);

        #ifdef DEBUG
            printf ("DEBUG: Before left subtree: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        if (*current == '(')
        {
            #ifdef DEBUG
                printf("DEBUG: Left subtree starts with '('\n");
                LOADING_BASE_DUMP(tree, *buffer, *pos_in_buffer, "before making LEFT subtree", progress, buffer_start);
            #endif

            node->left = LoadTreeFromFile (tree, &current, pos_in_buffer, progress, buffer_start, current_level + 1);

            #ifdef DEBUG
                printf("DEBUG: Left subtree, result: %p\n", (void*)node->left);
                LOADING_BASE_DUMP(tree, *buffer, *pos_in_buffer, "LEFT subtree completed", progress, buffer_start);
            #endif
        }
        else if (strncmp(current, "nil", 3) == 0)
        {
            #ifdef DEBUG
                printf("DEBUG: Left subtree is nil\n");
            #endif

            current += 3;
            (*pos_in_buffer) += 3;

            node->left = NULL;
        }

        current = SkipSpaces (current, pos_in_buffer);

        if (*current == '(')
        {
            #ifdef DEBUG
                printf ("DEBUG: Right subtree starts with '(',\n");
                LOADING_BASE_DUMP (tree, *buffer, *pos_in_buffer, "before making RIGHT subtree", progress, buffer_start);
            #endif

            node->right = LoadTreeFromFile (tree, &current, pos_in_buffer, progress, buffer_start, current_level + 1);

            #ifdef DEBUG
                printf("DEBUG: Right subtree, result: %p\n", (void*)node->right);
                LOADING_BASE_DUMP (tree, *buffer, *pos_in_buffer, "RIGHT subtree completed", progress, buffer_start);
            #endif
        }
        else if (strncmp(current, "nil", 3) == 0)
        {
            #ifdef DEBUG
            printf("DEBUG: Right subtree is nil\n");
            #endif

            current += 3;
            (*pos_in_buffer) += 3;
            node->right = NULL;
        }
        else
        {
            #ifdef DEBUG
                printf("DEBUG: ERROR: Expected open bracket but found: '%.10s...'\n", current);
            #endif

            return NULL;
        }

        current = SkipSpaces (current, pos_in_buffer);

        #ifdef DEBUG
        printf ("DEBUG: Before closing bracket: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
        #endif

        if (*current == ')')
        {
            current++;
            (*pos_in_buffer)++;

            #ifdef DEBUG
                printf ("DEBUG: Closing bracket found, node complete\n");
            #endif
        }
        else
        {
            #ifdef DEBUG
                printf ("DEBUG: ERROR: Expected closing bracket but found: '%.10s...', pos_in_buffer: %d\n", current, *pos_in_buffer);
            #endif

            return NULL;
        }

        *buffer = current;

        #ifdef DEBUG
            printf ("DEBUG: Returning node at %p\n", (void*)node);
            LOADING_BASE_DUMP (tree, *buffer, *pos_in_buffer, "Node FULL BUILT", progress, buffer_start);
        #endif

        return node;
    }
    else
    {
        #ifdef DEBUG
            printf ("DEBUG: No opening bracket found, returning NULL\n");
        #endif
    }

    return NULL;
}

TreeErr_t LoadDatabase(Tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    #ifdef DEBUG
        printf ("DEBUG: LoadDatabase called for file: %s\n", filename);
    #endif

    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        #ifdef DEBUG
            printf ("DEBUG: ERROR: Cannot open file %s\n", filename);
        #endif
        printf("No saved database found. Starting with empty knowledge.\n");
        return NOERORR;
    }

    #ifdef DEBUG
        printf ("DEBUG: File opened successfully\n");
    #endif

    long file_size = getFileSize(file);

    if (file_size <= 0)
    {
        fclose(file);
        return ERORRFILEOPEN;
    }

    #ifdef DEBUG
        printf ("DEBUG: File size: %ld bytes\n", file_size);
    #endif

    char* buffer = (char*) calloc (file_size + 1, sizeof(char));

    if (!buffer)
    {
        #ifdef DEBUG
            printf ("DEBUG: ERROR: Cannot allocate buffer of size %ld\n", file_size + 1);
        #endif

        fclose(file);
        return ERORRBUFFER;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    LoadProgress progress = {};
    CtorLoadProgress(&progress);

    const char* buffer_ptr = buffer;
    int position = 0;

    tree->root = LoadTreeFromFile (tree, &buffer_ptr, &position, &progress, buffer, 0);

    #ifdef DEBUG
        printf ("DEBUG: Tree parsing completed, root: %p\n", (void*)tree->root);
    #endif

    free(buffer);
    DtorLoadProgress(&progress);

    #ifdef DEBUG
        printf ("DEBUG: Buffer freed\n");
    #endif

    printf("Database loaded from %s\n", filename);

    return tree->root ? NOERORR : ERORRTREENULL;
}

long getFileSize (FILE* file)
{
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
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


void CtorLoadProgress (LoadProgress* progress)
{
    assert (progress);

    progress->capacity = 10;
    progress->size = 0;
    progress->nodes = (LoadNodeInfo*) calloc (progress->capacity, sizeof(LoadNodeInfo));
}

void AddNodeToLoadProgress(LoadProgress* progress, Node_t* node, int level)
{
    if (progress->size >= progress->capacity)
    {
        progress->capacity *= 2;
        progress->nodes = (LoadNodeInfo*) realloc (progress->nodes,
                                                   progress->capacity * sizeof(LoadNodeInfo));
    }
    progress->nodes[progress->size].node = node;
    progress->nodes[progress->size].rank = level;
    progress->size++;
}

void DtorLoadProgress (LoadProgress* progress)
{
    free (progress->nodes);
    progress->nodes = NULL;
    progress->capacity = 0;
    progress->size = 0;
    progress->current_rank = 0;
}
