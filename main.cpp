#include "differenciator.h"
#include "create_dump_files.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"

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
            printf ("Failed to load from %s, creating default tree...\n", input_filename);
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
    printf ("Result: %f\n", result);

    printf("\n=== DIFFERENTIATION ===\n");
    Tree_t* DiffTree = NULL;
    TreeCtor (&DiffTree);

    DiffTree->root = DifferentiateNode (Tree, Tree->root, 'A');

    printf ("Derivative tree: ");
    PrintNode (DiffTree->root);
    printf ("\n");

    QUICK_DUMP (Tree, "Original tree");
    QUICK_DUMP (DiffTree, "Derivative tree is ready");

    bool simplified1 = SimplifyUntilStable(Tree, MAX_LOOP_SIMPLE);
    printf("Tree simplified: %s\n", simplified1 ? "YES" : "NO");
    QUICK_DUMP (Tree, "Tree after Simplification");

    bool simplified2 = SimplifyUntilStable(DiffTree, MAX_LOOP_SIMPLE);
    printf("DiffTree simplified: %s\n", simplified2 ? "YES" : "NO");
    QUICK_DUMP (DiffTree, "DiffTree after Simplification");

    printf("\n=== CREATING STEP-BY-STEP DUMP ===\n");

    // Также создаем дамп после упрощения
    bool simplified = SimplifyUntilStable(DiffTree, MAX_LOOP_SIMPLE);

    CreateFullLatexDump("full_process.tex", Tree, DiffTree, simplified2, MAX_LOOP_SIMPLE);

    Create_log_file (Tree, "tree_dump.dot", DUMP_NORMAL, NULL);

    system ("dot -V");
    Close_html_file ();
    TreeDtor (Tree);
    TreeDtor (DiffTree);
}
