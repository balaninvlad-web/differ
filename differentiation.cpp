#include "differenciator.h"
#include "differentiation.h"
#include "create_dump_files.h"
#include "create_latex_dump.h"

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
            LatexDumpDiffStep(tree, "Правило суммы: (u+v)' = u' + v'");
            break;
        }
        case SUB:
        {
            result = SUB_(dL, dR);
            LatexDumpDiffStep(tree, "Правило разности: (u-v)' = u' - v'");
            break;
        }
        case MUL:
        {
            result = ADD_(MUL_(dL, cR), MUL_(cL, dR));
            LatexDumpDiffStep(tree, "Правило произведения: (uv)' = u'v + uv'");
            break;
        }
        case DIV:
        {
            result = DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), POW_(cR, NUM_(2)));
            LatexDumpDiffStep(tree, "Правило частного: (u/v)' = (u'v - uv')/v²");
            break;
        }
        case POW:
        {
            if (node->right->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(NUM_(node->right->value.number), POW_(cL, NUM_(node->right->value.number - 1))), dL);
                LatexDumpDiffStep(tree, "Степенное правило: (uⁿ)' = n·uⁿ⁻¹·u'");
            }
            else if (node->left->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR);
                LatexDumpDiffStep(tree, "Производная экспоненты: (aᵘ)' = aᵘ·ln(a)·u'");
            }
            else
            {
                Node_t* y = POW_(cL, cR);
                Node_t* term1 = MUL_(dR, LN_(cL));
                Node_t* term2 = MUL_(cR, DIV_(dL, cL));
                result = MUL_(y, ADD_(term1, term2));
                LatexDumpDiffStep(tree, "Общее степенное правило");
            }
            break;
        }
        case SIN:
        {
            result = MUL_(COS_(cL), dL);
            LatexDumpDiffStep(tree, "Производная синуса: (sin u)' = cos u · u'");
            break;
        }
        case COS:
        {
            result = MUL_(MUL_(NUM_(-1), SIN_(cL)), dL);
            LatexDumpDiffStep(tree, "Производная косинуса: (cos u)' = -sin u · u'");
            break;
        }
        case TAN:
        {
            result = MUL_(DIV_(NUM_(1), POW_(COS_(cL), NUM_(2))), dL);
            LatexDumpDiffStep(tree, "Производная тангенса: (tan u)' = u'/cos² u");
            break;
        }
        case LN:
        {
            result = DIV_( dL, cL);
            LatexDumpDiffStep(tree, "Производная логарифма: (ln u)' = u'/u");
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
