#include "differenciator.h"
#include "create_dump_files.h"

int main (int argc, char* argv[])
{
    Tree_t* Tree = NULL;
    TreeCtor (&Tree);

    const char* input_filename = NULL;

    if (argc > 1)
    {
        input_filename = argv[1];
        printf ("Loading tree from file: %s\n", input_filename);

        TreeErr_t load_result = LoadDatabase (Tree, input_filename);
        if (load_result != NOERORR)
        {
            printf("Failed to load from %s, creating default tree...\n", input_filename);
            CreateDefaultTree (Tree);
        }
    }
    else
    {
        printf ("No input file specified. Creating default tree...\n");
        CreateDefaultTree (Tree);
    }

    QUICK_DUMP (Tree, "check tree after make tree nods");

    printf ("Tree structure: ");
    PrintNode (Tree->root);
    printf ("\n");

    double result = EvaluateTreeAdvanced(Tree);
    printf("Result: %f\n", result);

    printf("\n=== DIFFERENTIATION ===\n");
    Tree_t* DiffTree = NULL;
    TreeCtor(&DiffTree);

    DiffTree->root = DifferentiateNode (Tree, Tree->root, 'x');

    printf("Derivative tree: ");
    PrintNode(DiffTree->root);
    printf("\n");

    QUICK_DUMP (Tree, "Original tree");
    QUICK_DUMP (DiffTree, "Derivative tree");

    Create_log_file (Tree, "tree_dump.dot", DUMP_NORMAL, NULL);

    CreateLaTeXDumpFile ("dump.tex", Tree, DiffTree);

    system("dot -V");
    Close_html_file();
}
