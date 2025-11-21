#include "differentiation.h"

Node_t* CopySubtree(Tree_t* tree, Node_t* node)
{
    if (!node) return NULL;

    Node_t* new_node = NodeCtor (tree, node->type, node->value);

    new_node->left =  CopySubtree (tree, node->left);
    new_node->right = CopySubtree (tree, node->right);

    return new_node;
}

Node_t* DifferentiateNode (Tree_t* tree, Node_t* node, char variable)
{
    if (!node) return NULL;

    switch (node->type)
    {
        case NUMBERTYPE:
        {
            return NodeCtor (tree, NUMBERTYPE, 0);
        }
        case VARIABLETYPE:
        {
            char node_var_name = GetterNameVariable(node->value);

            if (node_var_name == variable)
            {
                return NodeCtor (tree, NUMBERTYPE, 1);
            }
            else
            {
                return NodeCtor (tree, NUMBERTYPE, 0);
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
    switch (node->value)
    {
        case ADD:
        {
            return NewNode (tree, OPERATORTYPE, ADD,
                            DifferentiateNode (tree, node->left, variable),
                            DifferentiateNode (tree, node->right, variable));
        }
        case SUB:
        {
            return NewNode (tree, OPERATORTYPE, SUB,
                            DifferentiateNode (tree, node->left, variable),
                            DifferentiateNode (tree, node->right, variable));
        }
        case MUL:
        {
            return NewNode (tree, OPERATORTYPE, ADD,
                            NewNode (tree, MUL,
                                     DifferentiateNode (tree, node->left, variable),
                                     CopySubtree (tree, node->right),
                            NewNode (tree, MUL,
                                     CopySubtree (tree, node->left),
                                     DifferentiateNode (tree, node->right, variable));
        }
        default:
            return NULL;
    }
}

Node_t* NewNode (Tree_t* tree, int type, int value, Node_t* left, Node_t* right)
{
    Node_t* new_node = NodeCtor (tree, type, value);
    new_node->left = left;
    new_node->right = right;
    return new_node;
}
