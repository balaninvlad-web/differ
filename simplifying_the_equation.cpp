
Node_t* SimplificationTree (Node_t* node, Tree_t* tree)
{
    if (!node) return NULL;

    QUICK_DUMP (tree, "Simplification tree in progress");

    if (node->left)  SimplificationTree (node->left);
    if (node->right) SimplificationTree (node->right);

    double num_val = TryCalculateNode (node);

    if (!isnan(num_val))
    {
        ChangeNodeToNumber (node, num_value);
        return;
    }

    QUICK_DUMP (tree, "Simplification tree in progress");

    Simplificatoin
}


int CheckNodeStoresNumber (Node_t* node;)
{
    if (node->type == NUMBERTYPE)
    {
        return node->value;
    }
    if (node->type == OPERATORTYPE)
    {

        if (CheckNodeStoresNumber (node->left) != NAN)
        {
            if (CheckNodeStoresNumber (node->right) != NAN)
            {
                EvaluateOperator (node, Variable_t* variables, int var_count)
            }
        }
    }
}
