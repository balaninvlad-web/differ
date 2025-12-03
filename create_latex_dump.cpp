// create_latex_dump_adapted.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "differenciator.h"
#include "create_latex_dump.h"
#include "differentiation.h"
#include "simplifying_the_equation.h"

const char* funny_phrases[] = {
    "Очевидно, что:",
    "Каждому ежику понятно:",
    "Даже Бязит знает что:",
    "После недолгих размышлений:",
    "Как говорил Архимед:",
    "Не трудно заметить:",
    "Пытливый читатель уже мог догадаться что:",
    "Каждому бельчонку ясно:",
    "Стыдно было бы не знать:",
    "После третьего нервного срыва:",
    "После чашки кофе:",
    "ПОХУЙ УЖЕ:",
    "После пятой стопки ягеря наконец-то получаем:",
    "Вроде бы пока с ответами сходится:",
    "После употребления тяжелых наркотиков вам станет понятно что:",
    "Иди нахуй:",
    "Получаем полный пиздец:",
    "Ебанутая хуйня:",
    "Залупа, дальше нет смысла решать:",
    "Я ща все тут расхуячу, потому что:",
    "Ебало завалите кому матан нравится:",
    "Похоже ты мой дорогой читатель девственик до конца дней:",
    "Похоже ты мой дорогой читатель инцен ебаный если тебе это интересно:",
    "Ебать мне похуй уже:",
    "Нихуя уже не сходится:",
    "Ну и вот мы получаем полный пиздец:",
    "Только блять попробуй сука не сократиться:",
    "А ну вроде норм, ща подгоним:",
    "Еба тебе делать нехуй:",
    "Сук остановись уже даже мне похуй:",
    "Пиздез у нас залупа получается:",
    "Опа нихуя у нас не получилось опять:",
    "Ебать мне похуй уже:",
    "НИ ХУ Я не сходится с ответами:",
    "Ну оставлю так может быть ошибка в условии:",
    "Пиздец куда 2 часа просрал:",
    "А ну вроде норм, ща подгоним:"
};

const int PHRASES_COUNT = sizeof (funny_phrases) / sizeof (funny_phrases[0]);

static int latex_call_count = 0;

const char* get_funny_phrase ()
{
    return funny_phrases[latex_call_count++ % PHRASES_COUNT];
}

void CtorLatexDump (LatexDumpState* state, const char* filename)
{
    if (!state || !filename)
    {
        printf ("ERORR: invalid parametrs in CtorLatexDump\n");
        return;
    }

    state->filename = filename;
    state->file = fopen (filename, "w");
    if (!state->file)
    {
        printf("ERROR: Cannot create LaTeX file: %s\n", filename);
        return;
    }

    state->step_counter = 1;
    state->current_funny_index = 0;

    state->funny_phrases = funny_phrases;
    state->funny_phrases_count = PHRASES_COUNT;

    StartLatexDump (state->file);

    fprintf (state->file, "\\section*{Пошаговый процесс расчленения и анального насилия}\n");
    fprintf (state->file, "\\textbf{Дата:} %s\\\\\n", __DATE__);
    fprintf (state->file, "\\vspace{0.5cm}\n\n");

    printf ("LaTeX dump initialized: %s\n", filename);
}

void AddLatexStep (LatexDumpState* state, const char* description, Tree_t* tree)
{
    if (!state || !state->file || !description || !tree || !tree->root)
    {
        printf ("WARNING: Cannot add LaTeX step - invalid parameters\n");
        return;
    }

    const char* funny_comment = "";
    if (state->funny_phrases && state->funny_phrases_count > 0)
    {
        funny_comment = state->funny_phrases[state->current_funny_index % state->funny_phrases_count];
        state->current_funny_index++;
    }

    fprintf (state->file, "\\textit{%s}\\\\\n", funny_comment);
    fprintf (state->file, "\\begin{multline*}\n");

    print_tree_latex (state->file, tree->root);

    fprintf (state->file, "\n\\end{multline*}\n\n");

    state->step_counter++;

    //printf ("LaTeX dump simplify step (two params): %s\n", description);

    //printf ("Added LaTeX step %d: %s\n", state->step_counter - 1, description);
}

void print_tree_latex (FILE* file, Node_t* node)
{
    if (!file || !node) return;

    char buffer[MAX_STR_SIZE] = {};
    int pos = 0;

    NodeToLatexString (node, buffer, &pos, sizeof(buffer));

    fprintf(file, "%s", buffer);
}

void DtorLatexDump (LatexDumpState* state)
{
    if (!state || !state->file)
    {
        printf("WARNING: Cannot close LaTeX dump - invalid state\n");
        return;
    }

    fprintf (state->file, "\\section*{Заключение}\n");
    fprintf (state->file, "Процесс дифференцирования и упрощения нихуя не интересно если ты это читаешь то ты инцел сто проц.\\\\\n");
    fprintf (state->file, "Всего выполнено шагов: %d(ДОХУИЩА)\\\\\n", state->step_counter - 1);
    fprintf (state->file, "\\vspace{0.5cm}\n\n");

    EndLatexDump (state->file);
    fclose (state->file);
    state->file = NULL;

    printf ("LaTeX document completed. Total steps: %d\n", state->step_counter - 1);

    printf ("Compiling LaTeX to PDF...\n");
    char command[MAX_STR_SIZE] = {};
    snprintf (command, sizeof(command), "pdflatex -interaction=nonstopmode \"%s\"", state->filename);

    int result = system(command);

    if (result == 0)
        printf ("SUCCESS: PDF generated: %s.pdf\n", state->filename);
    else
        printf ("ERROR: Failed to compile LaTeX document\n");
}

void NodeToLatexString (Node_t* node, char* buffer, int* pos, int buffer_size)
{
    if (node == NULL || *pos >= buffer_size - 1)
        return;

    if (*pos > 100 && (*pos % 80) < 10)
    {
        *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\\\\n");
    }

    switch (node->type)
    {
        case NUMBERTYPE:
            *pos += snprintf (buffer + *pos, buffer_size - *pos, "%d", node->value.number);
            break;

        case VARIABLETYPE:
        {
            char var_name = GetterNameVariable (node->value.variable_code);
            *pos += snprintf (buffer + *pos, buffer_size - *pos, "%c", var_name);
            break;
        }

        case OPERATORTYPE:
            switch (node->value.operator_type)
            {
                case ADD:
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, " + ");
                    NodeToLatexString (node->right, buffer, pos, buffer_size);
                    break;

                case SUB:
                    if (node->left)
                    {
                        NodeToLatexString (node->left, buffer, pos, buffer_size);
                        *pos += snprintf (buffer + *pos, buffer_size - *pos, " - ");
                        NodeToLatexString (node->right, buffer, pos, buffer_size);
                    }
                    else
                    {
                        *pos += snprintf (buffer + *pos, buffer_size - *pos, "-");
                        NodeToLatexString (node->right, buffer, pos, buffer_size);
                    }
                    break;
                case MUL:
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, " \\cdot ");
                    NodeToLatexString (node->right, buffer, pos, buffer_size);
                    break;

                case DIV:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\frac{");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "}{");
                    NodeToLatexString (node->right, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "}");
                    break;

                case POW:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "{");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "}^{");
                    NodeToLatexString (node->right, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "}");
                    break;

                case SIN:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\sin(");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, ")");
                    break;

                case COS:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\cos(");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, ")");
                    break;

                case TAN:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\tan(");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, ")");
                    break;

                case LN:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\ln(");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, ")");
                    break;
                case EXP:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "\\exp(");
                    NodeToLatexString (node->left, buffer, pos, buffer_size);
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, ")");
                default:
                    *pos += snprintf (buffer + *pos, buffer_size - *pos, "?");
            }
            break;

        default:
            *pos += snprintf (buffer + *pos, buffer_size - *pos, "?");
    }
}

int StartLatexDump (FILE* file)
{
    if (file == NULL)
        return -1;

    fprintf(file, "\\documentclass[12pt]{article}\n");
    fprintf(file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(file, "\\usepackage[T2A]{fontenc}\n");
    fprintf(file, "\\usepackage[russian]{babel}\n");
    fprintf(file, "\\usepackage{amsmath}\n");
    fprintf(file, "\\usepackage{geometry}\n");
    fprintf(file, "\\geometry{a4paper, left=10mm, right=10mm, top=30mm, bottom=30mm}\n");
    fprintf(file, "\\begin{document}\n\n");

    fprintf(file, "\\section*{MATHANAL}\n\n");
    return 0;
}

int EndLatexDump (FILE* file)
{
    if (file == NULL)
        return -1;

    fprintf(file, "\\end{document}\n");
    return 0;
}

int DumpOriginalFunctionToFile (FILE* file, Tree_t* tree, double result_value)
{
    if (file == NULL || tree == NULL)
        return -1;

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {};
    int pos = 0;
    NodeToLatexString (tree->root, expression, &pos, sizeof(expression));

    fprintf (file, "\\subsection*{Original Expression}\n");
    fprintf (file, "Expression: \\[ %s \\]\n\n", expression);

    if (!isnan(result_value))
    {
        fprintf (file, "Evaluation result: \\[ %.6f \\]\n\n", result_value);
    }
    else
    {
        fprintf (file, "Evaluation result: \\[ \\text{NaN (requires variable values)} \\]\n\n");
    }

    return 0;
}

int DumpOptimizationStepToFile (FILE* file, const char* description, Tree_t* tree, double result_value)
{
    if (file == NULL || description == NULL || tree == NULL)
        return -1;

    fprintf(file, "\\subsubsection*{Optimization Step}\n");
    fprintf(file, "It is easy to see that %s:\n\n", description);

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(tree->root, expression, &pos, sizeof(expression));

    fprintf(file, "\\[ %s \\]\n\n", expression);

    if (!isnan (result_value))
    {
        fprintf (file, "Result after simplification: \\[ %.6f \\]\n\n", result_value);
    }
    else
    {
        fprintf (file, "Result after simplification: \\[ \\text{NaN (requires variable values)} \\]\n\n");
    }

    fprintf (file, "\\vspace{0.5em}\n");

    return 0;
}

int DumpDerivativeToFile (FILE* file, Tree_t* derivative_tree, double derivative_result, int derivative_order)
{
    if (file == NULL || derivative_tree == NULL)
        return -1;

    char derivative_expr[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString (derivative_tree->root, derivative_expr, &pos, sizeof(derivative_expr));

    const char* derivative_notation = NULL;
    char custom_notation[32] = {0};

    if (derivative_order == 1)
    {
        derivative_notation = "f'(x)";
    }
    else if (derivative_order == 2)
    {
        derivative_notation = "f''(x)";
    }
    else if (derivative_order == 3)
    {
        derivative_notation = "f'''(x)";
    }
    else
    {
        snprintf (custom_notation, sizeof(custom_notation), "f^{(%d)}(x)", derivative_order);
        derivative_notation = custom_notation;
    }

    fprintf (file, "\\subsection*{Derivative of Order %d}\n", derivative_order);
    fprintf (file, "Derivative: \\[ %s = %s \\]\n\n", derivative_notation, derivative_expr);

    if (!isnan (derivative_result))
    {
        fprintf (file, "Value of derivative at point: \\[ %s = %.6f \\]\n\n", derivative_notation, derivative_result);
    }
    else
    {
        fprintf (file, "Value of derivative at point: \\[ %s = \\text{NaN (requires variable values)} \\]\n\n", derivative_notation);
    }

    return 0;
}

void CreateAdaptedLatexDump (Tree_t* original_tree, Tree_t* diff_tree, int is_simplified)
{
    FILE* latex_file = fopen ("adapted_dump.tex", "w");
    if (!latex_file)
    {
        printf ("ERROR: Cannot create LaTeX file\n");
        return;
    }

    StartLatexDump (latex_file);

    // Дамп исходного выражения
    double original_result = EvaluateTreeAdvanced (original_tree);
    DumpOriginalFunctionToFile (latex_file, original_tree, original_result);

    // Дамп производной
    double derivative_result = EvaluateTreeAdvanced (diff_tree);
    DumpDerivativeToFile (latex_file, diff_tree, derivative_result, 1);

    // Дамп упрощенной версии
    if (is_simplified)
    {
        DumpOptimizationStepToFile (latex_file, "after simplification", diff_tree, derivative_result);
    }

    EndLatexDump(latex_file);
    fclose(latex_file);

    // Компилируем LaTeX
    printf("Compiling LaTeX document...\n");
    int result = system ("pdflatex -interaction=nonstopmode adapted_dump.tex");

    if (result == 0)
    {
        printf ("LaTeX PDF generated successfully: adapted_dump.pdf\n");
    }
    else
    {
        printf ("Error compiling LaTeX document\n");
    }
}

void LatexDumpDifferentiationStep (LatexDumpState* state, const char* rule_used, Tree_t* original, Tree_t* derivative)
{
    if (!state || !state->file || !rule_used || !original || !derivative) return;

    fprintf (state->file, "\\subsection*{Применение правила дифференцирования}\n");
    fprintf (state->file, "\\textbf{Правило:} %s\\\\\n", rule_used);

    fprintf (state->file, "\\textbf{Исходное выражение:}\n\\[ ");
    print_tree_latex (state->file, original->root);
    fprintf (state->file, " \\]\n\n");

    fprintf (state->file, "\\textbf{Производная:}\n\\[ ");
    print_tree_latex (state->file, derivative->root);
    fprintf (state->file, " \\]\n\n");

    const char* funny_comment = get_funny_phrase();

    fprintf (state->file, "\\textit{%s}\n\n", funny_comment);

    state->step_counter++;
}

void LatexDumpSimplifyStep (LatexDumpState* state, const char* rule_used, Tree_t* before, Tree_t* after)
{
    if (!state || !state->file || !rule_used || !before || !after) return;

    fprintf (state->file, "\\subsection*{Упрощение выражения}\n");
    fprintf (state->file, "\\textbf{Правило:} %s\\\\\n", rule_used);

    fprintf (state->file, "\\textbf{До упрощения:}\n\\[ ");
    print_tree_latex (state->file, before->root);
    fprintf (state->file, " \\]\n\n");

    fprintf (state->file, "\\textbf{После упрощения:}\n\\[ ");
    print_tree_latex (state->file, after->root);
    fprintf (state->file, " \\]\n\n");

    const char* funny_comment = get_funny_phrase();
    fprintf (state->file, "\\textit{%s}\n\n", funny_comment);

    state->step_counter++;
}

void LatexDumpDetailedProcess (Tree_t* original, Tree_t* derivative)
{
    LatexDumpState state = {};

    CtorLatexDump (&state, "detailed_process_full.tex");

    AddLatexStep (&state, "Исходное выражение", original);

    AddLatexStep (&state, "Применение правил дифференцирования", derivative);

    bool simplified = SimplifyUntilStable (derivative, MAX_LOOP_SIMPLE, &state);

    if (simplified)
    {
        AddLatexStep (&state, "Результат после упрощения", derivative);
    }

    double result = EvaluateTreeAdvanced (derivative);
    char eval_text[MAX_STR_SIZE] = {};
    snprintf (eval_text, sizeof(eval_text), "Вычисление производной в точке: %.6f", result);
    AddLatexStep(&state, eval_text, derivative);

    DtorLatexDump(&state);
}
