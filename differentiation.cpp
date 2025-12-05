#include "differenciator.h"
#include "differentiation.h"
#include "create_latex_dump.h"
#include "simplifying_the_equation.h"
#include "create_dump_files.h"

Node_t* CopySubtree (Tree_t* tree, Node_t* node)
{
    assert (tree);

    if (!node) return NULL;

    Node_t* new_node = NodeCtor (tree, node->type, node->value);

    new_node->left =  CopySubtree (tree, node->left);
    new_node->right = CopySubtree (tree, node->right);

    return new_node;
}

Node_t* DifferentiateNode (Tree_t* tree, Node_t* node, char variable)
{
    // TODO: tree_verify
    assert (tree);

    if (!node) return NULL;

    switch (node->type)
    {
        case NUMBERTYPE:
        {
            return NodeCtor (tree, NUMBERTYPE,  (union NodeValue){.number = 0});
        }
        case VARIABLETYPE:
        {
            char node_var_name = GetterNameVariable (node->value.variable_code);

            if (node_var_name == variable)
            {
                return NodeCtor (tree, NUMBERTYPE,  (union NodeValue){.number = 1});
            }
            else
            {
                return NodeCtor (tree, NUMBERTYPE,  (union NodeValue){.number = 0});
            }
        }
        case OPERATORTYPE:
        {
            return DifferentiateOperator (tree, node, variable);
        }
        default:
            return NULL;
    }
}

Node_t* DifferentiateOperator (Tree_t* tree, Node_t* node, char variable)
{
    assert (tree);

    Node_t* result = NULL;

    switch (node->value.operator_type)
    {
        case ADD:
        {
            result = ADD_(dL, dR);
            break;
        }
        case SUB:
        {
            result = SUB_(dL, dR);
            break;
        }
        case MUL:
        {
            if (node->left->type == NUMBERTYPE)
                return MUL_(cL, dR);
            else if (node->right->type == NUMBERTYPE)
                return MUL_(dL, cR);
            else
                return ADD_(MUL_(dL, cR), MUL_(cL, dR));
            break;
        }
        case DIV:
        {
            result = DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), POW_(cR, NUM_(2)));
            break;
        }
        case POW:
        {
            if (node->right->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(NUM_(node->right->value.number), POW_(cL, NUM_(node->right->value.number - 1))), dL);
            }
            else if (node->left->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR);
            }
            else
            {
                Node_t* y = POW_(cL, cR);
                Node_t* term1 = MUL_(dR, LN_(cL));
                Node_t* term2 = MUL_(cR, DIV_(dL, cL));
                result = MUL_(y, ADD_(term1, term2));
            }
            break;
        }
        case SIN:
        {
            result = MUL_(COS_(cL), dL);
            break;
        }
        case COS:
        {
            result = MUL_(MUL_(NUM_(-1), SIN_(cL)), dL);
            break;
        }
        case TAN:
        {
            result = MUL_(DIV_(NUM_(1), POW_(COS_(cL), NUM_(2))), dL);
            break;
        }
        case LN:
        {
            result = DIV_( dL, cL);
            break;
        }
        case EXP:
        {
            result = MUL_(EXP_(cL), dL);
            break;
        }
        default:
            result = NULL;
    }

    return result;
}

Node_t* NewNode (Tree_t* tree, int type, union NodeValue value, Node_t* left, Node_t* right)
{
    assert (tree);

    Node_t* new_node = NodeCtor (tree, type, value);
    new_node->left = left;
    new_node->right = right;
    return new_node;
}

Node_t* DifferentiateTreeN (Tree_t* tree, char variable, int n, LatexDumpState* latex_dump)
{
    assert (tree);
    assert (n >= 0);

    printf ("========== CALCULATE OF THE %d DERIVATIVE ===\n", n);

    if (n == 0)
    {
        if (latex_dump) AddLatexStep (latex_dump, "The first derivate of tree, it't simple tree", tree);
        return CopySubtree (tree, tree->root);
    }
    Tree_t* current_tree = NULL;
    TreeCtor(&current_tree);

    if (!current_tree)
    {
        printf("ERROR: Failed to create temporary tree\n");
        return NULL;
    }

    current_tree->root = CopySubtree (tree, tree->root);
    Node_t* result = NULL;

    for (int i = 1; i <= n; i++)
    {
        // TODO: take derivative
        printf ("=====Differentiation step %d/%d=====", i, n);

        if (latex_dump)
        {
            char before_title[MAX_STR_SIZE] = {};
            if (i == 1)
                snprintf(before_title, sizeof(before_title), "Исходная функция");
            else
                snprintf(before_title, sizeof(before_title), "Производная %d-го порядка (перед дифференцированием)", i-1);

            AddLatexStep(latex_dump, before_title, current_tree);
        }

        Node_t* derivative = DifferentiateNode (tree, current_tree->root, variable);

        if (!derivative)
        {
            printf ("ERORR: Differentiation failed at %d step", i+1);

            TreeDtor (current_tree);
            return NULL;
        }

        if (latex_dump)
        {
            char raw_title[MAX_STR_SIZE] = {};
            snprintf(raw_title, sizeof(raw_title),
                     "Производная шаг %d (после дифференцирования)", i);
            AddLatexStep(latex_dump, raw_title, current_tree);
        }

        DeleteSubtree (current_tree, &current_tree->root);
        current_tree->root = derivative;

        if (latex_dump)
        {
            char raw_title[MAX_STR_SIZE] = {};
            snprintf(raw_title, sizeof(raw_title),
                     "Сырая производная шаг %d (после дифференцирования)", i);
            AddLatexStep(latex_dump, raw_title, current_tree);
        }

        bool simplified = SimplifyUntilStable(current_tree, 30, latex_dump);

        AddLatexStep(latex_dump, "Результат дифференцирования после упрощения", current_tree);

        printf ("Simplifying derivative %d...\n", i+1);

        #ifdef DEBUG
            QUICK_DUMP(tree, "After differentiation step");
        #endif

        printf("Derivative %d calculated and simplified\n", i+1);
    }

    result = CopySubtree (tree, current_tree->root);

    printf ("\n=== %d DERIVATIVE COMPLETE ===\n", n);

    return result;
}
