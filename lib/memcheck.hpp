#ifndef MEMCHECK_HPP_INCLUDED
#define MEMCHECK_HPP_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#define MAX_LOG_SIZE (1000)

static FILE* memlog;

void logAdd(const char* type, void* ptr, const char* filename, int linepos);

void* operator new[](size_t size, const char* filename, int linepos)
{
    void* tmp = new char[size];
    logAdd("new", tmp, filename, linepos);
    return tmp;
}

void* operator new(size_t size, const char* filename, int linepos)
{
    return new(filename, linepos) char[size];
}

void operator delete[](void* ptr)
{
    logAdd("delete", ptr, nullptr, 0);
    free(ptr);
    return;
}

void operator delete(void* ptr)
{
    logAdd("delete", ptr, nullptr, 0);
    free(ptr);
    return;
}

#define new new(__FILE__, __LINE__)

typedef char spointer[13];

void logAdd(const char* type, void* ptr, const char* filename, int linepos)
{
    static int firstcall = 1;
    memlog = fopen("memlog.txt", (firstcall) ? "w" : "a");
    fprintf(memlog, "%-6s:[%p]%s", type, ptr, (type[0] == 'd') ? "x" : " ");
    if(filename != nullptr)
        fprintf(memlog, " in \"%s\" line %d", filename, linepos);
    fprintf(memlog, "\n");
    fclose(memlog);
    firstcall = 0;
}

int logCheck(const char* filename)
{
    FILE* inlog = fopen((filename != nullptr) ? filename : "memlog.txt", "r");

    const char outfilename[] = "memloglog.txt";
    FILE* outlog = fopen(outfilename, "w");

    spointer pointers[MAX_LOG_SIZE] = {};
    int indexes[MAX_LOG_SIZE];
    for(int i = 0; i < MAX_LOG_SIZE; i++)
        indexes[i] = i + 1;
    indexes[MAX_LOG_SIZE - 1] = -1;
    int last = -1, first = 0;

    char c = 0;
    while((c = getc(inlog)) != EOF)
    {
        if(c == '[')
        {
            getc(inlog); // 0
            getc(inlog); // x

            spointer tmp = "";
            fscanf(inlog, "%[^]]s", tmp);

            getc(inlog); // ]

            c = getc(inlog);
            fprintf(outlog, "%9s memory at [0x%s]: ", (c == 'x') ? "deleting" : "setting", tmp);

            int found = 0;
            if(c == 'x')
            {
                for(int i = 0; i <= last; i++)
                    if(!strcmp(pointers[i], tmp))
                    {
                        pointers[i][0] = '\0';

                        indexes[i] = first;
                        first = i;

                        for( ; last >= 0 && pointers[last][0] == '\0' ; last--)
                            ;

                        found = 1;
                        break;
                    }
                if(!found)
                    fputs("[FAILED]: an attempt to delete not-existing data\n", outlog);
                else
                    fputs("[  OK  ]\n", outlog);
            }
            else
            {
                for(int i = 0; i <= last; i++)
                    if(!strcmp(pointers[i], tmp))
                    {
                        found = 1;
                        break;
                    }
                if(found)
                {
                    fputs("[FAILED]: double-setting data in the same place\n", outlog);
                    printf("memcheck: double-setting at [0x%s]\n", tmp);
                }
                else
                    fputs("[  OK  ]\n", outlog);

                int place = first;

                first = indexes[place];
                strcpy(pointers[place], tmp);
                indexes[place] = -1;

                if(place > last)
                    last = place;
            }

            int width = 0;
            for(int i = last; i > 0; i /= 10)
                width++;

            fprintf(outlog, "\tfirst free space:  %d\n"
                            "\tlast pointer position: %d\n", first, last);
            if(last >= 0)
            {
                fprintf(outlog, "\t%*s| %-*s | %s\n", width, "#", width + 3, "free", "pointer");
                for(int i = 0; i <= last; i++)
                    fprintf(outlog, "\t%*d: (%*d) | 0x%s\n", width, i, width + 1, indexes[i], pointers[i]);
            }
            fputs("========================\n\n", outlog);
        }
    }
    if(last != -1)
    {
        fputs("ERROR: not-deleted memory left\n", outlog);
        puts("memcheck: not-deleted memory left");
    }

    printf("See %s for detailed information on memcheck work\n", outfilename);

    fclose(inlog);
    fclose(outlog);
    return 1;
}

#endif // MEMCHECK_HPP_INCLUDED
