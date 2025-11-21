#include <math.h>
#include "differenciator.h"

double EvaluateTreeAdvanced (Tree_t* tree)
{
    if (!tree || !tree->root) return NAN;

    int variables[MAX_VARIABLES] = {};

    int var_count = CollectVariables (tree->root, variables);

    Variable_t var_values[MAX_VARIABLES];

    for (int i = 0; i < var_count; i++)
    {
        char var_name = GetterNameVariable(variables[i]);
        printf ("Enter value for %c: ", var_name);
        scanf ("%lf", &var_values[i].value);
        var_values[i].name = GetterNameVariable (variables[i]);
    }

    return EvaluateNodeAdvanced (tree->root, var_values, var_count);
}

int CollectVariables (Node_t* node, int* variables)
{
    if (!node) return 0;

    int count = 0;

    if (node->type == VARIABLETYPE)
    {
        int var_name = node->value; //TODO function for return name

        int found = 0;

        for (int i = 0; i < count; i++)
        {
            if (variables[i] == var_name) found = 1;
        }

        if (!found) variables[count++] = var_name;
    }

    if (node->left)  count += CollectVariables (node->left,  variables + count);
    if (node->right) count += CollectVariables (node->right, variables + count);

    return count;
}

double EvaluateNodeAdvanced (Node_t* node, Variable_t* variables, int var_count)
{
    if (!node) return NAN;

    switch (node->type)
    {
        case NUMBERTYPE:
            return (double)node->value;
        case VARIABLETYPE:
        {
            char var_name = GetterNameVariable(node->value);

            for (int i = 0; i < var_count; i++)
            {
                if (variables[i].name == var_name)
                    return variables[i].value;
            }
            printf ("Variable '%c' not found!\n", var_name);
            return NAN;
        }
        case OPERATORTYPE:
            return EvaluateOperator (node, variables, var_count);
        default:
            return NAN;
    }
}

double EvaluateOperator (Node_t* node, Variable_t* variables, int var_count)
{
    double left_val = node->left ? EvaluateNodeAdvanced (node->left, variables, var_count) : 0;
    double right_val = node->right ? EvaluateNodeAdvanced (node->right, variables, var_count) : 0;

    switch (node->value)
    {
        case ADD: return left_val + right_val;
        case SUB: return left_val - right_val;
        case MUL: return left_val * right_val;
        case DIV: return right_val != 0 ? left_val / right_val : NAN;
        case POW: return pow (left_val, right_val);
        default: return NAN;
    }
}

char GetterNameVariable (int variable_code)
{
    switch (variable_code)
    {
        case ARGX: return 'x';
        case ARGY: return 'y';
        case ARGZ: return 'z';
        default  : return 'A';
    }
}
