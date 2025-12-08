#include "differenciator.h"
#include "create_latex_dump.h"
#include "differentiation.h"
#include "create_dump_files.h"
#include "simplifying_the_equation.h"
#include "make_teilor.h"

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
            printf ("Failed to load from %s, creating default tree...\n", filename);
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

    int teilor_order = 1;
    printf("\nВведите порядок тейлора (n): ");
    scanf("%d", &teilor_order);
    double x0 = 0;
    printf("\nВведите вокруг какой точки раскладывать (n): ");
    scanf("%lf", &x0);

    TaylorSeries* Series = {};
    Series = ComputeTaylorSeries (Tree,'x', x0, teilor_order, &latex_dump);

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
    printf ("Tree simplified: %s\n", simplified1 ? "YES" : "NO");
    QUICK_DUMP (Tree, "Tree after Simplification");
    AddLatexStep (&latex_dump, "Результат упрощения исходного выражения", Tree);


    bool simplified2 = SimplifyUntilStable (DiffTree, MAX_LOOP_SIMPLE, &latex_dump);
    printf ("DiffTree simplified: %s\n", simplified2 ? "YES" : "NO");
    QUICK_DUMP (DiffTree, "DiffTree after Simplification");
    AddLatexStep (&latex_dump, "Исходное выражение", Tree);
    AddLatexStep (&latex_dump, "Результат упрощения производной исходного выражения", DiffTree);

    int derivative_order = 1;
    printf("\nВведите порядок производной (n): ");
    scanf("%d", &derivative_order);

    if (derivative_order < 0)
    {
        printf ("ERROR: Derivative order must be non-negative\n");
        derivative_order = 1;
    }

    printf("\n=== CALCULATING %d-TH DERIVATIVE with respect to '%c' ===\n",
           derivative_order, 'x');

    Tree_t* DerivativeTree = NULL;
    TreeCtor (&DerivativeTree);
    DerivativeTree->root = DifferentiateTreeN (Tree, 'x', derivative_order, &latex_dump);

    printf("\n=== CREATING STEP-BY-STEP DUMP ===\n");

    Create_log_file (Tree, "tree_dump.dot", DUMP_NORMAL, NULL);

    if (Series) LatexDumpTaylorSeries(&latex_dump, Series, Tree);

    system ("dot -V");
    Close_html_file ();
    if (Tree) TreeDtor (Tree);
    if (DerivativeTree) TreeDtor (DerivativeTree);
    if (DiffTree) TreeDtor (DiffTree);
    if (Series) free (Series);
    DtorLatexDump (&latex_dump);
}
