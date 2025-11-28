#include "differenciator.h"

static StructCmd command_table[] =
{
     {"+"    , {.operator_type = ADD}, OPERATORTYPE},
    {"-"    , {.operator_type = SUB}, OPERATORTYPE},
    {"*"    , {.operator_type = MUL}, OPERATORTYPE},
    {"/"    , {.operator_type = DIV}, OPERATORTYPE},
    {"^"    , {.operator_type = POW}, OPERATORTYPE},
    {"sin"  , {.operator_type = SIN}, OPERATORTYPE},
    {"cos"  , {.operator_type = COS}, OPERATORTYPE},
    {"tan"  , {.operator_type = TAN}, OPERATORTYPE},
    {"ln"   , {.operator_type = LN},  OPERATORTYPE},

    // Переменные
    {"x"    , {.variable_code = ARGX}, VARIABLETYPE},
    {"y"    , {.variable_code = ARGY}, VARIABLETYPE},
    {"z"    , {.variable_code = ARGZ}, VARIABLETYPE},
    {"A"    , {.variable_code = ARGA}, VARIABLETYPE},
    {"B"    , {.variable_code = ARGB}, VARIABLETYPE},

    {NULL   , 0, 0}
};

const int COMMANDS_TABLE_SIZE = sizeof(command_table) / sizeof(StructCmd);

void DetermineType (const char* data, int* type, union NodeValue* value)
{
    for (int i = 0; i < MAX_COMMAND_SIZE && command_table[i].name != NULL; i++)
    {
        if (strcmp (data, command_table[i].name) == 0)
        {
            *type = command_table[i].type;
            *value = command_table[i].value;

            #ifdef DEBUG
                printf("DEBUG: Found in command table: '%s' -> type=%d\n",
                       data, *type);
            #endif
            return;
        }
    }

    if (isdigit((unsigned char)data[0]) || (data[0] == '-' && isdigit((unsigned char)data[1]))) // TODO: CheckIfNumber
    {
        *type = NUMBERTYPE;
        value->number = atoi(data);

        #ifdef DEBUG
            printf("DEBUG: Parsed as number: '%s' -> value=%d\n", data, value->number);
        #endif
    }
    else
    {
        *type = VARIABLETYPE;
        value->variable_code = 0;

        #ifdef DEBUG
            printf("DEBUG: Default to variable: '%s'\n", data);
        #endif
    }
}
