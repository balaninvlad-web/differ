#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int GetG ();
int GetN ();
int GetT ();
int GetE ();
int GetP ();

//const char* *buffer = "10*2+5*(45+30/5)";
//size_t *pos_in_buffer = 0;

Node_t* LoadTreeFromFile(Tree_t* tree, const char** buffer, int* pos_in_buffer,
                         LoadProgress* progress, const char* buffer_start, int current_level)
{
    tree->root = GetG();

    printf("Результат выражения %s = %d\n", *buffer, result);

    return 0;
}

int GetG (Tree_t* tree, const char** buffer, int* pos_in_buffer,
          LoadProgress* progress, const char* buffer_start, int current_level)
{
    tree->root = GetE ();

    if (*buffer[pos_in_buffer] != '\0')
        printf ("ERORR: in GetG");
    pos_in_buffer++;

    return val;
}

int GetN (Tree_t* tree, const char** buffer, int* pos_in_buffer,
          LoadProgress* progress, const char* buffer_start, int current_level)
{
    int val = 0;

    while ('0' <= *buffer[*pos_in_buffer] && *buffer[*pos_in_buffer] <= '9')
    {
        val = val*10 + (*buffer[*pos_in_buffer]-'0');
        *pos_in_buffer++;
    }
    return val;

}

int GetE (Tree_t* tree, const char** buffer, int* pos_in_buffer,
          LoadProgress* progress, const char* buffer_start, int current_level)
{
    int val = GetT ();

    while (*buffer[*pos_in_buffer] == '+' || *buffer[*pos_in_buffer] == '-')
    {
        char op = *buffer[*pos_in_buffer];
        *pos_in_buffer++;
        int val2 = GetT();

        if (op == '+')
            val += val2;
        else
            val -= val2;
    }
    return val;
}

int GetT (Tree_t* tree, const char** buffer, int* pos_in_buffer,
          LoadProgress* progress, const char* buffer_start, int current_level)
{
    int val = GetP ();

    while (*buffer[*pos_in_buffer] == '*' || *buffer[*pos_in_buffer] == '/')
    {
        char op = *buffer[*pos_in_buffer];

        *pos_in_buffer++;

        int val2 = GetP ();

        if (op == '*')
            val = val * val2;
        else
            val = val / val2;
    }
    return val;
}

int GetP (Tree_t* tree, const char** buffer, int* pos_in_buffer,
          LoadProgress* progress, const char* buffer_start, int current_level)
{
    if (*buffer[*pos_in_buffer] == '(')
    {
        *pos_in_buffer++;

        int val = GetE ();

        if (*buffer[*pos_in_buffer] == ')')
            *pos_in_buffer++;
        else
            printf ("ERORR: in GetP\n");

        return val;
    }
    else
        return GetN ();
}
