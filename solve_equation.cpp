#include <math.h>
#include "differenciator.h"

double EvaluateTreeAdvanced (Tree_t* tree)
{
    if (!tree || !tree->root) return NAN;

    int variables[MAX_VARIABLES] = {};

    int var_count = CollectVariables (tree->root, variables);

    Variable_t var_values[MAX_VARIABLES] = {};

    for (int i = 0; i < var_count; i++)
    {
        char var_name = GetterNameVariable (variables[i]);

        int already_entered = 0;

        for (int j = 0; j < i; j++)
        {
            if (variables[j] == variables[i])
            {
                already_entered = 1;
                break;
            }
        }

        if (already_entered)
        {
            printf("Variable %c already entered. Skipping...\n", var_name);
            continue;
        }

        printf("Enter value for %c: ", var_name);
        scanf("%lf", &var_values[i].value);
        var_values[i].name = var_name;
    }

    return EvaluateNodeAdvanced (tree->root, var_values, var_count);
}

int CollectVariables (Node_t* node, int* variables)
{
    assert (variables);
    assert (node);

    int count = 0;

    // TODO: check if variable exists
    if (node->type == VARIABLETYPE)
    {
        int var_code = node->value.variable_code; //TODO function for return name

        int found = 0;
        for (int i = 0; i < count; i++)
        {
            if (variables[i] == var_code)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
             if (!CheckVariableExists(var_code, variables, count))
            {
                variables[count] = var_code;
                count++;
            }
        }
    }

    if (node->left)  count += CollectVariables (node->left,  variables + count);
    if (node->right) count += CollectVariables (node->right, variables + count);

    return count;
}

double EvaluateNodeAdvanced (Node_t* node, Variable_t* variables, int var_count)
{
    assert (variables);

    if (!node) return NAN;

    switch (node->type)
    {
        case NUMBERTYPE:
            return (double)node->value.number;
        case VARIABLETYPE:
        {
            char var_name = GetterNameVariable (node->value.variable_code);

            // TODO: GetVariableValue
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
    assert (node);
    assert (variables);

    double left_val = node->left ? EvaluateNodeAdvanced (node->left, variables, var_count) : 0;
    double right_val = node->right ? EvaluateNodeAdvanced (node->right, variables, var_count) : 0;

    switch (node->value.operator_type)
    {
        case ADD: return left_val + right_val;
        case SUB: return left_val - right_val;
        case MUL: return left_val * right_val;
        case DIV:
        {
            if (fabs(right_val) < 1e-10)
                return NAN;
            else
                return left_val / right_val;
        }
        case POW: return pow (left_val, right_val);
        case SIN: return sin (left_val);
        case COS: return cos (left_val);
        case TAN: return tan (left_val);
        case LN:
        {
            if (left_val <= 0)
            {
                printf ("ERORR: ln only for positive number%f\n", left_val);
                return NAN;
            }
            return log (left_val);
        }
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

double GetVariableValue (char var_name, Variable_t* variables, int var_count)
{
    for (int i = 0; i < var_count; i++)
    {
        if (variables[i].name == var_name)
            return variables[i].value;
    }
    printf ("Variable '%c' not found!\n", var_name);
    return NAN;
}

int CheckVariableExists(int var_code, int* variables, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (variables[i] == var_code)
        {
            return 1;
        }
    }
    return 0;
}
