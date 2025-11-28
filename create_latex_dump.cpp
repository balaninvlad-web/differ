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

const int PHRASES_COUNT = sizeof(funny_phrases) / sizeof(funny_phrases[0]);

static int latex_call_count = 0;
const char* get_funny_phrase()
{
    return funny_phrases[latex_call_count++ % PHRASES_COUNT];
}

// Глобальная переменная для отслеживания текущего файла LaTeX
static FILE* current_latex_file = NULL;
static int step_counter = 1;

void NodeToLatexString(Node_t* node, char* buffer, int* pos, int buffer_size)
{
    if (node == NULL || *pos >= buffer_size - 1)
        return;

    switch (node->type)
    {
        case NUMBERTYPE:
            *pos += snprintf(buffer + *pos, buffer_size - *pos, "%d", node->value.number);
            break;

        case VARIABLETYPE:
        {
            char var_name = GetterNameVariable(node->value.variable_code);
            *pos += snprintf(buffer + *pos, buffer_size - *pos, "%c", var_name);
            break;
        }

        case OPERATORTYPE:
            switch (node->value.operator_type)
            {
                case ADD:
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, " + ");
                    NodeToLatexString(node->right, buffer, pos, buffer_size);
                    break;

                case SUB:
                    if (node->left == NULL && node->right != NULL)
                    {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "-");
                        if (node->right->type == OPERATORTYPE &&
                           (node->right->value.operator_type == ADD ||
                            node->right->value.operator_type == SUB))
                        {
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, "(");
                            NodeToLatexString(node->right, buffer, pos, buffer_size);
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                        }
                        else
                        {
                            NodeToLatexString(node->right, buffer, pos, buffer_size);
                        }
                    }
                    else
                    {
                        NodeToLatexString(node->left, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, " - ");
                        NodeToLatexString(node->right, buffer, pos, buffer_size);
                    }
                    break;

                case MUL:
                    if (node->left && node->left->type == NUMBERTYPE && node->left->value.number == -1)
                    {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "-");
                        NodeToLatexString(node->right, buffer, pos, buffer_size);
                    }
                    else if (node->right && node->right->type == NUMBERTYPE && node->right->value.number == -1)
                    {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "-");
                        NodeToLatexString(node->left, buffer, pos, buffer_size);
                    }
                    else
                    {
                        if (node->left && node->left->type == NUMBERTYPE &&
                            node->right && node->right->type == VARIABLETYPE)
                        {
                            NodeToLatexString(node->left, buffer, pos, buffer_size);
                            NodeToLatexString(node->right, buffer, pos, buffer_size);
                        }
                        else if (node->left && node->left->type == VARIABLETYPE &&
                                 node->right && node->right->type == NUMBERTYPE)
                        {
                            NodeToLatexString(node->right, buffer, pos, buffer_size);
                            NodeToLatexString(node->left, buffer, pos, buffer_size);
                        }
                        else
                        {
                            NodeToLatexString(node->left, buffer, pos, buffer_size);
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, " \\cdot ");
                            NodeToLatexString(node->right, buffer, pos, buffer_size);
                        }
                    }
                    break;

                case DIV:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\frac{");
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "}{");
                    NodeToLatexString(node->right, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "}");
                    break;

                case POW:
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "^{");
                    NodeToLatexString(node->right, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "}");
                    break;

                case SIN:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\sin(");
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    break;

                case COS:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\cos(");
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    break;

                case TAN:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\tan(");
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    break;

                case LN:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\ln(");
                    NodeToLatexString(node->left, buffer, pos, buffer_size);
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    break;

                default:
                    *pos += snprintf(buffer + *pos, buffer_size - *pos, "?");
            }
            break;

        default:
            *pos += snprintf(buffer + *pos, buffer_size - *pos, "?");
    }
}

int StartLatexDump(const char* filename)
{
    current_latex_file = fopen(filename, "w");
    if (!current_latex_file)
    {
        printf("ERROR: Cannot create LaTeX file: %s\n", filename);
        return -1;
    }

    step_counter = 1;

    fprintf(current_latex_file, "\\documentclass[12pt]{article}\n");
    fprintf(current_latex_file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(current_latex_file, "\\usepackage{amsmath}\n");
    fprintf(current_latex_file, "\\usepackage{geometry}\n");
    fprintf(current_latex_file, "\\geometry{a4paper, left=20mm, right=20mm, top=20mm, bottom=20mm}\n");
    fprintf(current_latex_file, "\\setlength{\\parindent}{0pt}\n");
    fprintf(current_latex_file, "\\setlength{\\parskip}{1em}\n");
    fprintf(current_latex_file, "\\begin{document}\n");

    fprintf(current_latex_file, "\\section*{Великое Упрощение 2024}\n");
    fprintf(current_latex_file, "\\large Дифференциатор 3000\\\\\n");
    fprintf(current_latex_file, "\\small %s\\\\\n", __DATE__);
    fprintf(current_latex_file, "\\vspace{0.5cm}\n\n");

    return 0;
}

int EndLatexDump()
{
    if (!current_latex_file) return -1;

    fprintf(current_latex_file, "\\section*{Заключение}\n");
    fprintf(current_latex_file, "Процесс дифференцирования и упрощения завершен. ");
    fprintf(current_latex_file, "Для улучшения результатов рекомендуется проверить структуру дерева и улучшить алгоритм упрощения.\\\\\n");

    fprintf(current_latex_file, "\\vspace{0.5cm}\n");
    fprintf(current_latex_file, "\\textbf{Всего шагов: %d}\n", step_counter - 1);

    fprintf(current_latex_file, "\\end{document}\n");
    fclose(current_latex_file);
    current_latex_file = NULL;

    return 0;
}

void DumpExpressionStep(const char* title, Tree_t* tree, const char* comment)
{
    if (!current_latex_file || !tree) return;

    fprintf(current_latex_file, "\\subsection*{%s}\n", title);

    if (comment && strlen(comment) > 0)
    {
        fprintf(current_latex_file, "\\textbf{%s}\\\\\n", comment);
    }

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(tree->root, expression, &pos, sizeof(expression));

    fprintf(current_latex_file, "\\[ %s \\]\n\n", expression);
    step_counter++;
}

void DumpDifferentiationStep(const char* rule_name, Tree_t* tree)
{
    if (!current_latex_file || !tree) return;

    fprintf(current_latex_file, "\\subsubsection*{Шаг %d: Дифференцирование}\n", step_counter);
    fprintf(current_latex_file, "\\textbf{Правило:} %s\\\\\n", rule_name);
    fprintf(current_latex_file, "\\textbf{Комментарий:} %s\\\\\n", get_funny_phrase());

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(tree->root, expression, &pos, sizeof(expression));

    fprintf(current_latex_file, "\\[ %s \\]\n\n", expression);
    step_counter++;
}

void DumpSimplificationStep(Tree_t* tree, const char* rule_applied)
{
    if (!current_latex_file || !tree) return;

    fprintf(current_latex_file, "\\subsubsection*{Шаг %d: Упрощение}\n", step_counter);
    fprintf(current_latex_file, "\\textbf{Применено:} %s\\\\\n", rule_applied);
    fprintf(current_latex_file, "\\textbf{Комментарий:} %s\\\\\n", get_funny_phrase());

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(tree->root, expression, &pos, sizeof(expression));

    fprintf(current_latex_file, "\\[ %s \\]\n\n", expression);
    step_counter++;
}

void DumpOriginalFunction(Tree_t* tree, double result_value)
{
    if (!current_latex_file || !tree) return;

    fprintf(current_latex_file, "\\section*{Исходная функция}\n");

    char expression[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(tree->root, expression, &pos, sizeof(expression));

    fprintf(current_latex_file, "Исходное выражение: \\[ %s \\]\n\n", expression);

    if (!isnan(result_value))
    {
        fprintf(current_latex_file, "Результат вычисления: \\[ %.6f \\]\n\n", result_value);
    }
    else
    {
        fprintf(current_latex_file, "Результат вычисления: \\[ \\text{Требуются значения переменных} \\]\n\n");
    }
}

void DumpDerivativeResult(Tree_t* derivative_tree, double derivative_result, char variable)
{
    if (!current_latex_file || !derivative_tree) return;

    fprintf(current_latex_file, "\\section*{Результат дифференцирования}\n");

    char derivative_expr[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;
    NodeToLatexString(derivative_tree->root, derivative_expr, &pos, sizeof(derivative_expr));

    fprintf(current_latex_file, "Производная по $%c$: \\[ \\frac{d}{d%c} = %s \\]\n\n",
            variable, variable, derivative_expr);

    if (!isnan(derivative_result))
    {
        fprintf(current_latex_file, "Значение производной: \\[ %.6f \\]\n\n", derivative_result);
    }
    else
    {
        fprintf(current_latex_file, "Значение производной: \\[ \\text{Требуются значения переменных} \\]\n\n");
    }
}

// Функция для создания полного дампа процесса
void CreateFullLatexDump(const char* filename, Tree_t* original_tree, Tree_t* diff_tree,
                        int is_simplified, int max_iterations)
{
    if (StartLatexDump(filename) != 0) return;

    // Исходная функция
    double original_result = EvaluateTreeAdvanced(original_tree);
    DumpOriginalFunction(original_tree, original_result);

    // Процесс дифференцирования
    fprintf(current_latex_file, "\\section*{Процесс дифференцирования}\n");

    // Здесь можно добавить вызовы DumpDifferentiationStep из функций дифференцирования
    // Для этого нужно модифицировать функции дифференцирования

    // Результат дифференцирования
    double derivative_result = EvaluateTreeAdvanced(diff_tree);
    DumpDerivativeResult(diff_tree, derivative_result, 'A');

    // Процесс упрощения
    if (is_simplified)
    {
        fprintf(current_latex_file, "\\section*{Процесс упрощения}\n");
        fprintf(current_latex_file, "Упрощение выполнено за %d итераций.\\\\\n", max_iterations);

        // Здесь можно добавить вызовы DumpSimplificationStep из функций упрощения
        // Для этого нужно модифицировать функции упрощения

        DumpExpressionStep("Упрощенная производная", diff_tree, "Финальный результат упрощения");
    }

    EndLatexDump();

    // Компилируем LaTeX
    printf("Компилируем LaTeX документ...\n");
    char command[256];
    snprintf(command, sizeof(command), "pdflatex -interaction=nonstopmode \"%s\"", filename);
    int result = system(command);

    if (result == 0)
    {
        printf("PDF успешно создан: %s.pdf\n", filename);
    }
    else
    {
        printf("Ошибка компиляции LaTeX документа\n");
    }
}

// Функции для вызова из дифференцирования и упрощения
void LatexDumpDiffStep(Tree_t* tree, const char* rule)
{
    DumpDifferentiationStep(rule, tree);
}

void LatexDumpSimplifyStep(Tree_t* tree, const char* rule)
{
    DumpSimplificationStep(tree, rule);
}

