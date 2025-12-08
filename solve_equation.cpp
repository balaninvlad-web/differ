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

            return GetVariableValue (var_name, variables, var_count);
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
        case EXP:
        {
            if (left_val > 700.0)
            {
                printf("WARNING: exp(%g) would overflow, returning INF\n", left_val);
                return INFINITY;
            }
            else if (left_val < -700.0)
                return 0.0;
            return exp(left_val);
        }
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
        case ARGA: return 'A';
        case ARGB: return 'B';
        default  : return 'x';
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
    return ARGX;
}

int CheckVariableExists (int var_code, int* variables, int count)
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

void SubstitudeVariableWithValue(Node_t* node, char variable, double value)
{
    if (!node) return;

    if (node->type == VARIABLETYPE)
    {
        char node_var = GetterNameVariable(node->value.variable_code);
        if (node_var == variable)
        {
            node->type = NUMBERTYPE;
            node->value.number = (int)round(value);
        }
    }

    if (node->left) SubstitudeVariableWithValue(node->left, variable, value);
    if (node->right) SubstitudeVariableWithValue(node->right, variable, value);
}

double EvaluateAtPoint(Tree_t* tree, char variable, double value)
{
    if (!tree || !tree->root) return NAN;

    Tree_t* temp_tree = NULL;
    TreeCtor(&temp_tree);
    if (!temp_tree) return NAN;

    temp_tree->root = CopySubtree(tree, tree->root);
    if (!temp_tree->root)
    {
        TreeDtor(temp_tree);
        return NAN;
    }

    SubstitudeVariableWithValue (temp_tree->root, variable, value);

    int variable_code = 0;
    switch (variable)
    {
        case 'x': variable_code = ARGX; break;
        case 'y': variable_code = ARGY; break;
        case 'z': variable_code = ARGZ; break;
        case 'A': variable_code = ARGA; break;
        case 'B': variable_code = ARGB; break;
        default:  variable_code = ARGX; break;
    }

    Variable_t var_values[1] = {{variable, value}};

    double result = EvaluateNodeAdvanced (temp_tree->root, var_values, 1);

    TreeDtor(temp_tree);
    return result;
}
