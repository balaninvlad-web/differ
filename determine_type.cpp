#include "differenciator.h"

StructCmd command_table[MAX_COUNT_COMMAND] =
{
    {"+"    , ADD, OPERATORTYPE},
    {"-"    , SUB, OPERATORTYPE},
    {"*"    , MUL, OPERATORTYPE},
    {"/"    , DIV, OPERATORTYPE},
    {"^"    , POW, OPERATORTYPE},
    //{"sin"  , SIN, OPERATORTYPE},
    //{"cos"  , COS, OPERATORTYPE},
    //{"ln"   , LN,  OPERATORTYPE},

    // Переменные
    {"x"    , ARGX, VARIABLETYPE},
    {"y"    , ARGY, VARIABLETYPE},
    {"z"    , ARGZ, VARIABLETYPE},

    {NULL   , 0, 0}
};

void DetermineType (const char* data, int* type, int* value)
{
    for (int i = 0; i < MAX_COMMAND_SIZE && command_table[i].name != NULL; i++)
    {
        if (strcmp (data, command_table[i].name) == 0)
        {
            *type = command_table[i].type;
            *value = command_table[i].value;

            #ifdef DEBUG
                printf("DEBUG: Found in command table: '%s' -> type=%d, value=%d\n",
                       data, *type, *value);
            #endif
            return;
        }
    }

    if (isdigit((unsigned char)data[0]) || (data[0] == '-' && isdigit((unsigned char)data[1])))
    {
        *type = NUMBERTYPE;
        *value = atoi(data);

        #ifdef DEBUG
            printf("DEBUG: Parsed as number: '%s' -> value=%d\n", data, *value);
        #endif
    }
    else
    {
        *type = VARIABLETYPE;
        *value = 0;

        #ifdef DEBUG
            printf("DEBUG: Default to variable: '%s'\n", data);
        #endif
    }
}
