/// \file Simple and not-handy version of MemCheck for C++
/// - works by "new" and "delete"
/// - at least 2 calls needed for the whole root

#ifndef MEMCHECK_HPP_INCLUDED
#define MEMCHECK_HPP_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#define MAX_LOG_SIZE (1000)

static FILE* memlog;    ///< File to write log into

void logAdd(const char* type, void* ptr, const char* filename, int linepos);

/// Redefinition of new[]
///
void* operator new[](size_t size, const char* filename, int linepos)
{
    void* tmp = new char[size];
    logAdd("new", tmp, filename, linepos);
    return tmp;
}

/// Redefinition of new
///
void* operator new(size_t size, const char* filename, int linepos)
{
    return new(filename, linepos) char[size];
}

/// Redefinition of delete[]
///
void operator delete[](void* ptr)
{
    logAdd("del", ptr, nullptr, 0);
    free(ptr);
    return;
}

/// Redefinition of delete
///
void operator delete(void* ptr)
{
    logAdd("del", ptr, nullptr, 0);
    free(ptr);
    return;
}

#define new new(__FILE__, __LINE__) ///< Some secret line

typedef char spointer[13];          ///< \typedef Memory carrying string-written pointer


/// Add new memory call to log
///
/// \param type const char*     - either "del" or "new" - the type of call
/// \param ptr void*            - the position of memory that has been called
/// \param filename const char* - the file where the call appeared
/// \param linepos int          - the line in file where the call appeared
///
void logAdd(const char* type, void* ptr, const char* filename, int linepos)
{
    if(strcmp(type, "del") && strcmp(type, "new"))
        return;
    static int firstcall = 1;
    memlog = fopen("memolog.txt", (firstcall) ? "w" : "a");
    fprintf(memlog, "%s:[%p]%s", type, ptr, (type[0] == 'd') ? "x" : " ");
    if(filename != nullptr)
        fprintf(memlog, " in \"%s\" line %d", filename, linepos);
    fprintf(memlog, "\n");
    fclose(memlog);
    firstcall = 0;
    return;
}

/// Analyze the MemChick log information
///
/// \param filename const char* - input log file ("memolog.txt" by default)
/// \return int - 0 if everything is OK; 1 - if memory calls were in a trouble; 2 - if there was no file with such name
///
///
int logCheck(const char* filename)
{
    int ferr = 0;
    FILE* inlog = fopen((filename != nullptr) ? filename : "memolog.txt", "r");
    if(!inlog)
        return 2;

    const char outfilename[] = "memologolog.txt";
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
                {
                    fputs("[FAILED]: an attempt to delete not-existing data\n", outlog);
                    ferr = 1;
                }
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
                    printf("MemChick: double-setting at [0x%s]\n", tmp);
                    ferr = 1;
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
        puts("MemChick: not-deleted memory left");
        ferr = 1;
    }

    printf("See %s for detailed information on MemChick work\n", outfilename);

    fclose(inlog);
    fclose(outlog);
    return ferr;
}

#endif // MEMCHECK_HPP_INCLUDED
