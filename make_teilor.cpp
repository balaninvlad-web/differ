#include "differenciator.h"
#include "create_latex_dump.h"
#include "differentiation.h"
#include "create_dump_files.h"
#include "simplifying_the_equation.h"
#include "make_teilor.h"

TaylorSeries* ComputeTaylorSeries(Tree_t* function_tree,
                                 char variable,
                                 double x0,
                                 int max_order,
                                 LatexDumpState* latex_dump)
{
    if (!function_tree || !function_tree->root)
    {
        printf("ERROR: Invalid function tree\n");
        return NULL;
    }

    TaylorSeries* series = (TaylorSeries*)calloc(1, sizeof(TaylorSeries));
    if (!series)
    {
        printf("ERROR: Memory allocation failed for TaylorSeries\n");
        return NULL;
    }

    series->coefficients = (TaylorCoefficient*)calloc(max_order + 1, sizeof(TaylorCoefficient));
    if (!series->coefficients)
    {
        free(series);
        printf("ERROR: Memory allocation failed for coefficients\n");
        return NULL;
    }

    series->count = max_order + 1;
    series->x0 = x0;
    series->variable = variable;
    series->max_order = max_order;

    double* factorials = (double*)calloc(max_order + 1, sizeof(double));
    if (!factorials) {
        free(series->coefficients);
        free(series);
        printf("ERROR: Memory allocation failed for factorials\n");
        return NULL;
    }

    factorials[0] = 1;
    for (int i = 1; i <= max_order; i++)
    {
        factorials[i] = factorials[i-1] * i;
    }

    printf("\n=== РАЗЛОЖЕНИЕ В РЯД ТЕЙЛОРА В ТОЧКЕ %g ===\n", x0);

    printf ("Исходная функция: ");
    PrintNode (function_tree->root);
    printf ("\n");

    printf ("\n=== Вычисление 0-й производной ===\n");
    series->coefficients[0].coefficient = EvaluateAtPoint (function_tree, variable, x0);
    series->coefficients[0].indoor_value = series->coefficients[0].coefficient;
    series->coefficients[0].order = 0;

    printf("f(%g) = %g\n", x0, series->coefficients[0].indoor_value);
    printf("Коэффициент a0 = %g\n", series->coefficients[0].coefficient);


    Tree_t* current_derivative = NULL;
    TreeCtor(&current_derivative);
    if (!current_derivative)
    {
        free(factorials);
        free(series->coefficients);
        free(series);
        return NULL;
    }

    current_derivative->root = CopySubtree(function_tree, function_tree->root);

    for (int order = 1; order <= max_order; order++)
    {
        printf("\n=== Вычисление %d-й производной ===\n", order);

        Node_t* new_derivative = DifferentiateNode(function_tree, current_derivative->root, variable);
        if (!new_derivative) {
            printf("Ошибка при дифференцировании порядка %d\n", order);
            break;
        }

        DeleteSubtree(current_derivative, &current_derivative->root);
        current_derivative->root = new_derivative;

        SimplifyUntilStable(current_derivative, MAX_LOOP_SIMPLE, latex_dump);

        printf ("Производная %d-го порядка: ", order);
        PrintNode (current_derivative->root);
        printf ("\n");

        double value_at_x0 = EvaluateAtPoint(current_derivative, variable, x0);

        series->coefficients[order].coefficient = value_at_x0 / factorials[order];
        series->coefficients[order].indoor_value = value_at_x0;
        series->coefficients[order].order = order;

        printf ("f^(%d)(%g) = %g\n", order, x0, value_at_x0);
        printf ("Коэффициент a%d = %g / %g! = %g\n",
               order, value_at_x0, order, series->coefficients[order].coefficient);

        if (latex_dump && latex_dump->file)
        {
            char step_desc[MAX_STR_SIZE] = {};
            snprintf (step_desc, sizeof(step_desc),
                     "Производная %d-го порядка в точке %g: $f^{(%d)}(%g) = %g$",
                     order, x0, order, x0, value_at_x0);
            AddLatexStep(latex_dump, step_desc, current_derivative);
        }
    }

    TreeDtor (current_derivative);
    free (factorials);

    printf ("\n=== ИТОГОВЫЕ КОЭФФИЦИЕНТЫ ===\n");
    for (int i = 0; i <= max_order; i++)
    {
        printf ("a[%d] = %g (f^(%d)(%g) = %g)\n",
               i, series->coefficients[i].coefficient,
               i, x0, series->coefficients[i].indoor_value);
    }

    return series;
}
