#include "differenciator.h"
#include "create_dump_files.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"

int main (int argc, char* argv[])
{
    Tree_t* Tree = NULL;
    TreeCtor (&Tree);

    const char* filename = NULL;

    LatexDumpState latex_dump = {};
    CtorLatexDump (&latex_dump, "detailed_process.tex");

    if (argc  > 1)
    {
        filename = argv[1];
        printf ("Loading tree from file: %s\n", filename);

        TreeErr_t load_result = LoadMathExpressionFromFile (Tree, filename);

        if (load_result != NOERORR)
        {
            printf ("Failed to load from %s, creating default tree...\n", filename);
            //CreateDefaultTree (Tree);
        }
    }
    else
    {
        printf ("No input file specified. Creating default tree...\n");
        CreateDefaultTree (Tree);
    }

    QUICK_DUMP (Tree, "check tree after make tree nods");
    AddLatexStep (&latex_dump, "Исходное выражение", Tree);

    printf ("Tree structure: ");
    PrintNode (Tree->root);
    printf ("\n");

    double result = EvaluateTreeAdvanced (Tree);
    printf ("Result: %f\n", result);

    printf("\n=== DIFFERENTIATION ===\n");
    Tree_t* DiffTree = NULL;
    TreeCtor (&DiffTree);

    DiffTree->root = DifferentiateNode (Tree, Tree->root, 'x');

    printf ("Derivative tree: ");
    PrintNode (DiffTree->root);
    printf ("\n");

    QUICK_DUMP (Tree, "Original tree");
    QUICK_DUMP (DiffTree, "Derivative tree is ready");
    AddLatexStep (&latex_dump, "Результат дифференцирования", DiffTree);

    bool simplified1 = SimplifyUntilStable (Tree, MAX_LOOP_SIMPLE, &latex_dump);
    printf("Tree simplified: %s\n", simplified1 ? "YES" : "NO");
    QUICK_DUMP (Tree, "Tree after Simplification");
    AddLatexStep (&latex_dump, "Результат упрощения исходного выражения", Tree);


    bool simplified2 = SimplifyUntilStable (DiffTree, MAX_LOOP_SIMPLE, &latex_dump);
    printf("DiffTree simplified: %s\n", simplified2 ? "YES" : "NO");
    QUICK_DUMP (DiffTree, "DiffTree after Simplification");
    AddLatexStep (&latex_dump, "Результат упрощения производной исходного выражения", DiffTree);

    printf("\n=== CREATING STEP-BY-STEP DUMP ===\n");

    Create_log_file (Tree, "tree_dump.dot", DUMP_NORMAL, NULL);

    system ("dot -V");
    Close_html_file ();
    TreeDtor (Tree);
    TreeDtor (DiffTree);
    DtorLatexDump (&latex_dump);
}
