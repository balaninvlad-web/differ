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

const char* str = "10*2+5*(45+30/5)";
size_t pos = 0;

int main()
{
    int result = GetG();

    printf("Результат выражения %s = %d\n", str, result);

    return 0;
}

int GetG ()
{
    int val = GetE ();

    if (str[pos] != '\0')
        printf ("ERORR: in GetG");
    pos++;

    return val;
}

int GetN ()
{
    int val = 0;

    while ('0' <= str[pos] && str[pos] <= '9')
    {
        val = val*10 + (str[pos]-'0');
        pos++;
    }
    return val;

}

int GetE ()
{
    int val = GetT ();

    while (str[pos] == '+' || str[pos] == '-')
    {
        char op = str[pos];
        pos++;
        int val2 = GetT();

        if (op == '+')
            val += val2;
        else
            val -= val2;
    }
    return val;
}

int GetT ()
{
    int val = GetP ();

    while (str[pos] == '*' || str[pos] == '/')
    {
        char op = str[pos];

        pos++;

        int val2 = GetP ();

        if (op == '*')
            val = val * val2;
        else
            val = val / val2;
    }
    return val;
}

int GetP ()
{
    if (str[pos] == '(')
    {
        pos++;

        int val = GetE ();

        if (str[pos] == ')')
            pos++;
        else
            printf ("ERORR: in GetP\n");

        return val;
    }
    else
        return GetN ();
}
