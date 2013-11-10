#include <iostream>
#include "sas.h"
#include "loader.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

Asoc my;

int main()
{
    my.src = new SimpleAsoc;
    my.src->data = NULL;
    my.src->asoctype = TYPE_TEXT;
    my.src->flags = 0;

    char* input = new char[2048];
    long keyval = 0, val = 0;

    bool intmode = false;

    printf("Mode: %s\n", SASLoader::GetStringType(&my));

    while (true)
    {
        printf("> ");

        fflush(stdin);
        scanf("%s", input);

        if (strcmp(input, "exit") == 0)
        {
            printf("Goodbye.");
            break;
        }

        if (strcmp(input, "save") == 0)
        {
            char* dst = new char[2048];
            if (scanf(" %[^\n]s", dst) == 1)
                SASLoader::Save(&my, dst);
        }

        if (strcmp(input, "load") == 0)
        {
            char* dst = new char[2048];
            if (scanf(" %[^\n]s", dst) == 1)
                SASLoader::Load(&my, dst);
        }

        if (strcmp(input, "print") == 0)
            my.PrintOut();

        if (strcmp(input, "mode") == 0)
        {
            char* dst = new char[128];
            if (scanf(" %[^\n]s", dst) == 1)
            {
                if (strcmp(dst, "int") == 0)
                {
                    my.asocmap.clear();
                    my.stringmap.clear();
                    my.src->asoctype = TYPE_INT;
                    intmode = true;
                }
                else if (strcmp(dst, "short") == 0)
                {
                    my.asocmap.clear();
                    my.stringmap.clear();
                    my.src->asoctype = TYPE_SHORT;
                    intmode = true;
                }
                else if (strcmp(dst, "char") == 0)
                {
                    my.asocmap.clear();
                    my.stringmap.clear();
                    my.src->asoctype = TYPE_CHAR;
                    intmode = true;
                }
                else if (strcmp(dst, "long") == 0)
                {
                    my.asocmap.clear();
                    my.stringmap.clear();
                    my.src->asoctype = TYPE_LONG;
                    intmode = true;
                }
                else if (strcmp(dst, "text") == 0 || strcmp(dst, "string") == 0)
                {
                    my.asocmap.clear();
                    my.stringmap.clear();
                    my.src->asoctype = TYPE_TEXT;
                    intmode = false;
                }
                else
                {
                    printf("Unknown mode: %s", dst);
                }
            }
        }

        if (intmode)
        {
            if (strcmp(input, "set") == 0)
            {
                if (scanf("%d", &keyval) == 1)
                {
                    if (scanf("%d", &val) == 1)
                    {
                        if (!my.SetNumValue(keyval, val))
                            printf("Key value %d created\n",keyval);

                        printf("%d - %d\n", keyval, val);
                    }
                }
            }
            else if (strcmp(input, "get") == 0)
            {
                if (scanf("%d", &keyval) == 1)
                {
                    long a = my.GetNumValue<long>(keyval);
                    printf("%d - %d\n",keyval,a);
                }
            }
        }
        else
        {
            if (strcmp(input, "set") == 0)
            {
                if (scanf("%d", &keyval) == 1)
                {
                    char* dst = new char[2048];

                    // note the space at the beginning of expression
                    if (scanf(" %[^\n]s", dst) == 1)
                    {
                        if (!my.SetStringValue(keyval, dst))
                            printf("Key value %d created\n",keyval);

                        printf("%d - %s\n", keyval, dst);
                    }
                }
            }
            else if (strcmp(input, "get") == 0)
            {
                if (scanf("%d", &keyval) == 1)
                {
                    if (!my.IsPresent(keyval))
                    {
                        printf("Key value %d not found!\n", keyval);
                    }
                    else
                    {
                        const char* str = my.GetStringValue(keyval);
                        printf("%d - %s\n",keyval,str);
                    }
                }
            }
        }
    }

    return 1;
}
