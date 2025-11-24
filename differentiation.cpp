#include "differenciator.h"
#include "differentiation.h"

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

    switch (node->value.operator_type)
    {
        case ADD:
        {
            return ADD_(dL, dR);
        }
        case SUB:
        {
            return SUB_(dL, dR);
        }
        case MUL:
        {
            return ADD_(MUL_(dL, cR), MUL_(cL, dR));
        }
        case DIV:
        {
            return DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), MUL_(cL, cL));
        }
        case POW:
        {
            if (node->right->type == NUMBERTYPE)
            {
                return MUL_(MUL_(NUM_(node->right->value.number), POW_(cL, NUM_(node->right->value.number - 1))), dL);
            }
            else if (node->left->type == NUMBERTYPE)
            {
                return MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR);
            }
            else
            {
                Node_t* y = POW_(cL, cR);
                Node_t* term1 = MUL_(dR, LN_(cL));
                Node_t* term2 = MUL_(cR, DIV_(dL, cL));

                return MUL_(y, ADD_(term1, term2));
            }
        }
        case SIN:
        {
            return MUL_(COS_(cL), dL);
        }
        case COS:
        {
            return MUL_(MUL_(NUM_(-1), SIN_(cL)), dL);
        }
        case TAN:
        {
            return MUL_(DIV_(NUM_(1), POW_(COS_(cL), NUM_(2))), dL);
        }
        case LN:
        {
            return MUL_(DIV_(NUM_(-1), cL), dL);
        }
        default:
            return NULL;
    }
}

Node_t* NewNode (Tree_t* tree, int type, union NodeValue value, Node_t* left, Node_t* right)
{
    assert (tree);

    Node_t* new_node = NodeCtor (tree, type, value);
    new_node->left = left;
    new_node->right = right;
    return new_node;
}
