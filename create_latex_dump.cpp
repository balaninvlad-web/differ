#include "differenciator.h"
#include "create_latex_dump.h"

int CreateLaTeXDumpFile (char* tex_filename, Tree_t* tree, Tree_t* Difftree)
{
    FILE* latex_file = NULL;

    Create_head_tex (&latex_file, tex_filename);

    if (!latex_file) return -1;

    Add_expression_to_latex(latex_file, tree, "exp:");

    if (diff_tree)
        Add_expression_to_latex(latex_file, Difftree, "exp:");

    Close_latex_file(latex_file);
    return 0;
}

void Create_head_tex (FILE** latex_file, char* tex_filename)
{
    *latex_file = fopen (tex_filename, "w");

    if (*latex_file == NULL)
    {
        printf("ERROR: Cannot create HTML file\n");
        return;
    }

    fprintf(*latex_file, "\documentclass{article}\n");
                       "\usepackage[utf8]{inputenc}\n"
                       "\usepackage[T2A]{fontenc}\n"
                       "\usepackage[english, russian]{babel}\n"
                       "\usepackage{amsmath, amssymb}\n"
                       "\usepackege{hypperref}\n"
                       "\usepasage[top=2cm, bottom=2cm, left=2.5cm, right=1.5cm]{geometry}\n"
                       "\begin{document}\n"
                       "\begin{center}\n"
                       "{\LARGE \textbf{Нахождение полной производной сложнейшей функции}}\n"
                       "\vspace{1cm}\n"
                       "{\large Баланин В. А., студент 1 курса ФРТК}\n"
                       "\vspace{0.5cm}\n"
                       "%s\n" //TODO avtomaticheski podstavliat' datu i vremia
                       "\end{center}\n"
                       "\vspace{1cm}\n"
                       "\maketitle"
                       "section*{Введение}\n", get_current_date());
}

char* get_current_date()
{
    time_t time = time(NULL):
    struct tm tm = *localtime(&time);
    char date = calloc (50, sizeof(char));
    snprinf(date, 50, "%d %s %d г.", tm.tm_mday, tm.tm_year + 1900);
    return date;
}

void Close_latex_file(FILE* latex_file)
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

void print_tree_latex (FILE* file, Node* node)
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
            char var_name = GetterNameVariable (node->value.vaiable_code);
            fprintf (file, "%c", var_name);
            break;
        }
        case OPERATORTYPE:
        {
            if (node->left)
            {
                print_tree_latex (file, node->left);
            }

            switch (node->value.operator_type)
            {
                case ADD: fprintf (file, "+"); break;
                case SUB: fprintf (file, "-"); break;
                case MUL: fprintf (file, "\\cdot "); break;
                case POW: fprintf (file, "^"); break;
                case SIN: fprintf (file, "\\sin("); break;
                case COS: fprintf (file, "\\cos("); break;
                case TAN: fprintf (file, "\\tan("); break;
                case LN: fprintf (file, "\\ln("); break;
                case DIV:
                {
                    fprintf (file, "\\frac{");

                    if (node->left) print_tree_latex (file, node->left);

                    fprintf (file, "}{");

                    if (node->right) print_tree_latex (file, node->right);

                    fprintf (file, "}");
                    return;
                }
            }
            if (node->value.operator_type != DIV)
            {
                if (node->right)
                {
                    if (node->value.operator_type == SIN || node->value.operator_type == COS ||
                        node->value.operator_type == TAN || node->value.operator_type == LN)
                    {
                        print_tree_latex (file, node->right);
                        fprintf(file, ")");
                    }
                    else
                    {
                        print_tree_latex (file, node->right);
                    }
                }

            }
            break;
        }

}
