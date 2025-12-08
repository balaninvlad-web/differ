#ifndef MAKE_TEILOR_H
#define MAKE_TEILOR_H

#include "differenciator.h"
#include "create_latex_dump.h"

struct TaylorCoefficient
{
    double coefficient;
    double indoor_value;
    int order;
};

typedef struct TaylorSeries
{
    TaylorCoefficient* coefficients;
    int count;
    double x0;
    char variable;
    int max_order;
} TaylorSeries;

TaylorSeries* ComputeTaylorSeries(Tree_t* function_tree,
                                 char variable,
                                 double x0,
                                 int max_order,
                                 LatexDumpState* latex_dump);

#endif
