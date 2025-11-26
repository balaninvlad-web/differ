#include "differenciator.h"
#include "create_latex_dump.h"

int CreateLaTeXDumpFile (const char* tex_filename, Tree_t* tree, Tree_t* diff_tree)
{
    FILE* latex_file = NULL;

    Create_head_tex (&latex_file, tex_filename);

    if (!latex_file) return -1;

    Add_expression_to_latex(latex_file, tree, "exp:");

    if (diff_tree && diff_tree->root)
        Add_derivative_to_latex (latex_file, diff_tree, "exp:");

    Close_latex_file (latex_file);

    CreateLaTeXDumpPdf (tex_filename);

    return 0;
}

void Create_head_tex (FILE** latex_file, const char* tex_filename)
{
    *latex_file = fopen (tex_filename, "w");

    if (*latex_file == NULL)
    {
        printf("ERROR: Cannot create HTML file\n");
        return;
    }

    fprintf(*latex_file, "\\documentclass{article}\n"
                         "\\usepackage[utf8]{inputenc}\n"
                         "\\usepackage[T2A]{fontenc}\n"
                         "\\usepackage{amsmath}\n"
                         "\\begin{document}\n"
                         "\\section*{Дифференцирование}\n\n");
}

void Close_latex_file (FILE* latex_file)
{
    if (latex_file)
    {
        fprintf(latex_file, "\\end{document}\n");
        fclose(latex_file);
        printf("LaTeX is ready bitch\n");
    }
}

void Add_expression_to_latex (FILE* latex_file, Tree_t* tree, const char* title)
{
    if (!tree || !tree->root) return;

    fprintf(latex_file, "\\subsection*{%s}\n", title);
    fprintf(latex_file, "\\[\n");
    print_tree_latex(latex_file, tree->root);
    fprintf(latex_file, "\n\\]\n\n");
}

void Add_derivative_to_latex(FILE* tex_file, Tree_t* tree, const char* variable)
{
    if (!tree || !tree->root) return;

    fprintf(tex_file, "\\subsection*{Производная по $%s$}\n", variable);
    fprintf(tex_file, "\\[\n");
    fprintf(tex_file, "\\frac{d}{d%s} = ", variable);
    print_tree_latex(tex_file, tree->root);
    fprintf(tex_file, "\n\\]\n\n");
}

void print_tree_latex (FILE* file, Node_t* node)
{
    if (!node) return;

    switch (node->type)
    {
        case NUMBERTYPE:
        {
            fprintf (file, "%d", node->value.number);
            break;
        }
        case VARIABLETYPE:
        {
            char var_name = GetterNameVariable (node->value.variable_code);

            fprintf (file, "%c", var_name);
            break;
        }
        case OPERATORTYPE:
        {
            switch (node->value.operator_type)
            {
                case ADD:
                case SUB:
                {
                    if (node->left) print_tree_latex (file, node->left);

                    fprintf (file, " %s ", (node->value.operator_type == ADD) ? " + " : " - ");

                    if (node->right) print_tree_latex (file, node->right);

                    break;
                }
                case MUL:
                {
                    print_operand_with_brackets(file, node->left, true);
                    fprintf (file, " \\cdot ");
                    print_operand_with_brackets(file, node->right, true);
                    break;
                }
                case POW:
                {
                    print_operand_with_brackets(file, node->left, false);
                    fprintf (file, "^");
                    print_operand_with_brackets(file, node->right, true);
                    break;
                }
                case SIN:
                {
                    print_unary_operator(file, "\\sin", node->left);
                    break;
                }
                case COS:
                {
                    print_unary_operator(file, "\\cos", node->left);
                    break;
                }
                case TAN:
                {
                    print_unary_operator(file, "\\tan", node->left);
                    break;
                }
                case LN:
                {
                    print_unary_operator(file, "\\ln", node->left);
                    break;
                }
                case DIV:
                {
                    fprintf (file, "\\frac{");

                    if (node->left) print_tree_latex (file, node->left);

                    fprintf (file, "}{");

                    if (node->right) print_tree_latex (file, node->right);

                    fprintf (file, "}");
                    return;
                }
                default:
                    printf ("DEBUG: ERORR: invalid operator type in print_tree_latex\n");
            }
            break;
        }
        default:
            printf ("DEBUG: ERORR: invalid type in print_tree_latex\n");
    }
}

void print_unary_operator (FILE* file, const char* op_name, Node_t* operand)
{
    fprintf (file, "%s(", op_name);
    if (operand) print_tree_latex (file, operand);
    fprintf (file, ")");
}

void print_operand_with_brackets (FILE* file, Node_t* operand, bool for_multiplication)
{
    if (!operand) return;

    bool need_brackets = false;

    if (operand->type == OPERATORTYPE)
    {
        if (for_multiplication)
        {
            need_brackets = (operand->value.operator_type == ADD || operand->value.operator_type == SUB);
        }
        else
        {
            need_brackets = true;
        }
    }

    if (need_brackets)
    {
        fprintf (file, "(");
        print_tree_latex (file, operand);
        fprintf (file, ")");
    }
    else
    {
        print_tree_latex (file, operand);
    }
}

int CreateLaTeXDumpPdf (const char* tex_filename)
{
    char command[MAX_STR_SIZE] = {};

    sprintf(command, "pdflatex -interaction=nonstopmode \"%s\"", tex_filename);

    int result = system(command);

    if (result == 0)
    {
        printf ("PDF is ready bitch!\\n");
        printf ("file: %s.pdf\\n", tex_filename);
    }
    else
        printf ("Erorr in making PDF\n");

    return result;
}
