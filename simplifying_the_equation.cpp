#include <math.h>
#include "differenciator.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"
#include "create_dump_files.h"

bool SimplificationTree (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump)
{
    if (!node) return false;

    bool local_changed = false;

    #ifdef DEBUG
        QUICK_DUMP (tree, "Simplification tree in progress");
    #endif

    if (node->left)
    {
        bool child_changed = SimplificationTree (node->left, tree, changed, latex_dump);
        local_changed = local_changed || child_changed;
    }
    if (node->right)
    {
        bool child_changed = SimplificationTree (node->right, tree, changed, latex_dump);
        local_changed = local_changed || child_changed;
    }

    Node_t* original_left = node->left;
    Node_t* original_right = node->right;
    int original_type = node->type;
    union NodeValue original_value = node->value;

    double num_value = TryCalculateNode (node);

    if (!isnan(num_value))
    {
        ChangeNodeToNumber (node, num_value, tree);

        if (node->type != original_type || node->value.number != original_value.number)
            local_changed = true;
    }

    #ifdef DEBUG
        QUICK_DUMP (tree, "Simplification tree in progress");
    #endif

    SimplificatoinPrimeNumbers (node, tree, &local_changed, latex_dump);

    if (node->left != original_left || node->right != original_right ||
        node->type != original_type || !AreNodeValuesEqual(node->value, original_value))
        local_changed = true;

    if (changed && local_changed)
        *changed = true;

    return local_changed;
}

void ChangeNodeToNumber (Node_t* node, double num_value, Tree_t* tree)
{
    if (!node) return;

    if (node->left)
    {
        DeleteSubtree (tree, &node->left);
        node->left = NULL;
    }
    if (node->right)
    {
        DeleteSubtree (tree, &node->right);
        node->right = NULL;
    }

    node->type = NUMBERTYPE;
    node->value.number = (int)num_value;
}

double TryCalculateNode (Node_t* node)
{
    if (!node) return NAN;

    if (node->type == NUMBERTYPE)
        return (double)node->value.number;

    if (node->type == OPERATORTYPE)
    {
        if (node->value.operator_type == SIN ||
            node->value.operator_type == COS ||
            node->value.operator_type == TAN ||
            node->value.operator_type == EXP ||
            node->value.operator_type == LN)
        {
            if (node->left)
            {
                double arg_val = TryCalculateNode(node->left);
                if (isnan(arg_val)) return NAN;
                switch (node->value.operator_type)
                {
                    case SIN: return sin (arg_val);
                    case COS: return cos (arg_val);
                    case TAN: return tan (arg_val);
                    case LN:  return log (arg_val);
                    case EXP:
                    {
                         double arg_val = TryCalculateNode(node->left);
                        if (isnan(arg_val)) return NAN;

                        if (arg_val > 700.0)
                        {
                            printf("WARNING: exp(%g) would overflow\n", arg_val);
                            return INFINITY;
                        }
                        return exp (arg_val);
                    }
                    default:  return NAN;
                }
            }
            return NAN;
        }
        else if (node->value.operator_type == SUB && node->left == NULL && node->right)
        {
            double right_val = TryCalculateNode(node->right);
            if (isnan(right_val)) return NAN;
            return -right_val;
        }
        else if (node->left && node->right)
        {
            double left_val  = TryCalculateNode(node->left);
            double right_val = TryCalculateNode(node->right);

            if (!isnan(left_val) && !isnan(right_val))
            {
                switch (node->value.operator_type)
                {
                    case ADD: return left_val + right_val;
                    case SUB: return left_val - right_val;
                    case MUL: return left_val * right_val;
                    case DIV:
                        if (fabs(right_val) < 1e-10) return NAN;
                        return left_val / right_val;
                    case POW: return pow(left_val, right_val);
                    default: return NAN;
                }
            }
        }
    }
    return NAN;
}

void SimplificatoinPrimeNumbers (Node_t* node, Tree_t* tree, bool* changed, LatexDumpState* latex_dump)
{
    if (!node || node->type != OPERATORTYPE) return;

    switch (node->value.operator_type)
    {
        case ADD:
        {
            if (IsZero (node->right)) // A+0
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A + 0 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left)) // 0+A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 0 + A = A", tree);

                ReplaceWithRightChild (node);
                if (changed) *changed = true;
            }
            break;
        }
        case SUB:
        {
            if (node->left == NULL)
                printf("������� �����!\n");

            if (IsZero (node->right)) // A-0
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A - 0 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left, node->right)) // A-A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep (latex_dump, "simplifying: A - A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left)) // 0-A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 0 - A = -A", tree);

                Node_t* temp = node->right;
                node->type = OPERATORTYPE;
                node->value.operator_type = SUB;
                node->left = NULL;
                node->right = temp;
                if (changed) *changed = true;
            }
            break;
        }
        case MUL:
        {
            if (IsZero (node->left) || IsZero (node->right))
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 0 * A = 0 || A * 0 = 0", tree);

                ChangeNodeToNumber (node, 0, tree); // 0*A || A*0
                if (changed) *changed = true;
            }
            else if (IsOne (node->right)) // A*1
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A * 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left)) // 1*A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 1 * A = A", tree);

                ReplaceWithRightChild (node);
                if (changed) *changed = true;
            }
            break;
        }
        case DIV:
        {
            if (IsZero (node->left) && !IsZero (node->right)) // 0/A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 0 / A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right)) // A/1
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A / 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left, node->right)) // A/A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A / A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            break;
        }
        case POW:
        {
            if (IsZero (node->right)) // A^0
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A^0 = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left)) // 1^A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 1^A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right)) // A^1
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: A^1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero(node->left) && !IsZero(node->right)) // 0^A
            {
                if (latex_dump && latex_dump->file)
                    AddLatexStep(latex_dump, "simplifying: 0^A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            break;
        }
        default:
            break;
    }
}

void ReplaceWithLeftChild (Node_t* node)
{
    if (!node || !node->left) return;

    Node_t* left_child = node->left;

    node->type = left_child->type;
    node->value = left_child->value;

    node->left = left_child->left;
    node->right = left_child->right;

    free(left_child);
}

void ReplaceWithRightChild (Node_t* node)
{
    if (!node || !node->right) return;

    Node_t* right_child = node->right;

    node->type =  right_child->type;
    node->value = right_child->value;

    node->left =  right_child->left;
    node->right = right_child->right;

    free (right_child);
}

int IsZero (Node_t* node)
{
    if (!node) return 0;
    if (node->type == NUMBERTYPE && node->value.number == 0)
        return 1;

    double val = TryCalculateNode (node);
    return !isnan(val) && fabs(val) < 1e-10;
}

int IsOne (Node_t* node)
{
    if (!node) return 0;
    if (node->type == NUMBERTYPE && node->value.number == 1)
        return 1;

    double val = TryCalculateNode (node);

    return !isnan(val) && fabs(val - 1.0) < 1e-10;
}

int AreNodesEqual (Node_t* a, Node_t* b)
{
    if (!a && !b) return 1;
    if (!a || !b) return 0;

    if (a->type != b->type) return 0;

    switch (a->type)
    {
        case NUMBERTYPE:
            return a->value.number == b->value.number;
        case VARIABLETYPE:
            return a->value.variable_code == b->value.variable_code;
        case OPERATORTYPE:
            return a->value.operator_type == b->value.operator_type &&
                   AreNodesEqual (a->left, b->left) &&
                   AreNodesEqual (a->right, b->right);
        default:
            return 0;
    }
}

bool SimplifyUntilStable (Tree_t* tree, int max_iterations, LatexDumpState* latex_dump)
{
    if (!tree || !tree->root) return false;

    printf ("\n=== STARTING SIMPLIFICATION LOOP ===\n");

    bool changed = true;
    int iteration = 0;

    while (changed && iteration < max_iterations)
    {
        changed = false;

        SimplificationTree (tree->root, tree, &changed, latex_dump);

        iteration++;

        printf ("Simplification iteration %d: %s\n",
               iteration, changed ? "changed" : "stable");

        if (changed && latex_dump && latex_dump->file)
        {
            char step_desc[MAX_STR_SIZE] = {};
            snprintf (step_desc, sizeof(step_desc), "After iteration of simplifying %d", iteration);
            AddLatexStep (latex_dump, step_desc, tree);
        }
    }

    printf ("=== SIMPLIFICATION COMPLETE ===\n");
    printf ("Total iterations: %d\n", iteration);

    return (iteration > 1);
}

int AreNodeValuesEqual(union NodeValue a, union NodeValue b)
{
    return a.number == b.number &&
           a.operator_type == b.operator_type &&
           a.variable_code == b.variable_code;
}
