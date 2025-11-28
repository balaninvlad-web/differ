#include <math.h>
#include "differenciator.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"
#include "create_dump_files.h"

bool SimplificationTree (Node_t* node, Tree_t* tree, bool* changed)
{
    if (!node) return false;

    bool local_changed = false;

    QUICK_DUMP (tree, "Simplification tree in progress");

    if (node->left)
    {
        bool child_changed = SimplificationTree (node->left, tree, changed);
        local_changed = local_changed || child_changed;
    }
    if (node->right)
    {
        bool child_changed = SimplificationTree (node->right, tree, changed);
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

    QUICK_DUMP (tree, "Simplification tree in progress");

    SimplificatoinPrimeNumbers (node, tree, &local_changed);

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
    {
        return (double)node->value.number;
    }
    if (node->type == OPERATORTYPE && node->left && node->right)
    {
        double left_val  = TryCalculateNode (node->left);
        double right_val = TryCalculateNode (node->right);

        if (!isnan(left_val) && !isnan(right_val))
        {
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
                default: return NAN;
            }
        }
    }
    return NAN;
}

void SimplificatoinPrimeNumbers (Node_t* node, Tree_t* tree, bool* changed)
{
    if (!node || node->type != OPERATORTYPE) return;

    switch (node->value.operator_type)
    {
        case ADD:
        {
            if (IsZero (node->right)) // A+0
            {
                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A + 0 = A");
            }
            else if (IsZero (node->left)) // 0+A
            {
                ReplaceWithRightChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "0 + A = A");
            }
            break;
        }
        case SUB:
        {
            if (IsZero (node->right)) // A-0
            {
                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A - 0 = A");
            }
            else if (AreNodesEqual (node->left, node->right)) // A-A
            {
                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A - A = 0");
            }
            else if (IsZero (node->left)) // 0-A
            {
                // Заменяем на унарный минус
                Node_t* temp = node->right;
                node->type = OPERATORTYPE;
                node->value.operator_type = SUB;
                node->left = NULL;
                node->right = temp;
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "0 - A = -A");
            }
            break;
        }
        case MUL:
        {
            if (IsZero (node->left) || IsZero (node->right))
            {
                ChangeNodeToNumber (node, 0, tree); // 0*A || A*0
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "0 * A = 0 или A * 0 = 0");
            }
            else if (IsOne (node->right)) // A*1
            {
                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A * 1 = A");
            }
            else if (IsOne (node->left)) // 1*A
            {
                ReplaceWithRightChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "1 * A = A");
            }
            else if (node->left && node->left->type == NUMBERTYPE && node->left->value.number == -1)
            {
                // -1 * A = -A
                ReplaceWithRightChild(node);
                node->type = OPERATORTYPE;
                node->value.operator_type = SUB;
                node->left = NULL;
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "-1 * A = -A");
            }
            break;
        }
        case DIV:
        {
            if (IsZero (node->left) && !IsZero (node->right)) // 0/A
            {
                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "0 / A = 0");
            }
            else if (IsOne (node->right)) // A/1
            {
                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A / 1 = A");
            }
            else if (AreNodesEqual (node->left, node->right)) // A/A
            {
                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A / A = 1");
            }
            break;
        }
        case POW:
        {
            if (IsZero (node->right)) // A^0
            {
                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A^0 = 1");
            }
            else if (IsOne (node->left)) // 1^A
            {
                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "1^A = 1");
            }
            else if (IsOne (node->right)) // A^1
            {
                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "A^1 = A");
            }
            else if (IsZero(node->left) && !IsZero(node->right)) // 0^A (где A ≠ 0)
            {
                ChangeNodeToNumber(node, 0, tree);
                if (changed) *changed = true;
                LatexDumpSimplifyStep(tree, "0^A = 0 (A ≠ 0)");
            }
            break;
        }
        default:
            break;
    }

    // Дополнительные упрощения для вложенных выражений
    if (node->type == OPERATORTYPE)
    {
        // Упрощение двойного отрицания: -(-A) = A
        if (node->value.operator_type == SUB && node->left == NULL &&
            node->right && node->right->type == OPERATORTYPE &&
            node->right->value.operator_type == SUB && node->right->left == NULL)
        {
            ReplaceWithRightChild(node->right);
            if (changed) *changed = true;
            LatexDumpSimplifyStep(tree, "-(-A) = A");
        }
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

    free(right_child);
}

int IsZero(Node_t* node)
{
    if (!node) return 0;
    if (node->type == NUMBERTYPE && node->value.number == 0)
        return 1;

    double val = TryCalculateNode (node);
    return !isnan(val) && fabs(val) < 1e-10;
}

int IsOne(Node_t* node)
{
    if (!node) return 0;
    if (node->type == NUMBERTYPE && node->value.number == 1)
        return 1;

    double val = TryCalculateNode (node);

    return !isnan(val) && fabs(val - 1.0) < 1e-10;
}

int AreNodesEqual(Node_t* a, Node_t* b)
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
                   AreNodesEqual(a->left, b->left) &&
                   AreNodesEqual(a->right, b->right);
        default:
            return 0;
    }
}

bool SimplifyUntilStable (Tree_t* tree, int max_iterations)
{
    if (!tree || !tree->root) return false;

    printf ("\n=== STARTING SIMPLIFICATION LOOP ===\n");

    bool changed = true;
    int iteration = 0;

    while (changed && iteration < max_iterations)
    {
        changed = false;

        SimplificationTree (tree->root, tree, &changed);

        iteration++;

        printf ("Simplification iteration %d: %s\n",
               iteration, changed ? "changed" : "stable");

        if (changed)
            QUICK_DUMP (tree, "After simplification iteration");
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
