#include <iostream>
#include "loader.h"
#include "sas.h"

void SASLoader::Load(Asoc* dst, const char* file)
{
    FILE* f = fopen(file, "rb");
    if (!f)
    {
        printf("Error: File not found!\n");
        return;
    }

    dst->asocmap.clear();
    dst->stringmap.clear();

    dst->src->magic = new char[4];
    fread(dst->src->magic, sizeof(char), 4, f);

    for (unsigned int i = 0; i < 4; i++)
    {
        if (dst->src->magic[i] != SAS_MAGIC[i])
        {
            printf("Error: unknown file format!\n");
            return;
        }
    }

    char atype;
    char flags;

    unsigned int datacount, stringcount;

    fread(&atype, sizeof(char), 1, f);
    fread(&flags, sizeof(char), 1, f);
    fread(&datacount, sizeof(unsigned int), 1, f);
    fread(&stringcount, sizeof(unsigned int), 1, f);

    dst->src->asoctype = (AsocTypes)atype;
    dst->src->flags = flags;

    unsigned int typesize = 0;

    switch (dst->src->asoctype)
    {
        case TYPE_CHAR:
            typesize = sizeof(char);
            break;
        case TYPE_SHORT:
            typesize = sizeof(short);
            break;
        case TYPE_INT:
        case TYPE_TEXT:
            typesize = sizeof(int);
            break;
        case TYPE_LONG:
            typesize = sizeof(long);
            break;
    }

    unsigned int tmp;
    long val;
    for (unsigned int i = 0; i < datacount; i++)
    {
        val = 0;

        fread(&tmp, sizeof(unsigned int), 1, f);
        fread(&val, typesize, 1, f);

        dst->SetNumValue(tmp, val);
    }

    for (unsigned int i = 0; i < stringcount; i++)
    {
        char* buf = new char[2048];
        memset(buf, 0, 2048);

        for (unsigned int j = 0; j < 2048; j++)
        {
            buf[j] = fgetc(f);
            if (buf[j] == '\0')
                break;
        }

        dst->SetRawStringValue(i, buf);

        delete [] buf;
    }

    fclose(f);

    printf("Load successfull!\n");
}

void SASLoader::Save(Asoc* src, const char* file)
{
    unsigned int typesize = 0;

    switch (src->src->asoctype)
    {
        case TYPE_CHAR:
            typesize = sizeof(char);
            break;
        case TYPE_SHORT:
            typesize = sizeof(short);
            break;
        case TYPE_INT:
        case TYPE_TEXT:
            typesize = sizeof(int);
            break;
        case TYPE_LONG:
            typesize = sizeof(long);
            break;
    }

    // conditions for future? (instead of using std::map::size())
    unsigned int datacount = 0;
    for (std::map<unsigned int, void*>::const_iterator itr = src->asocmap.begin(); itr != src->asocmap.end(); ++itr)
        datacount++;

    unsigned int stringcount = 0;
    for (std::map<unsigned int, char*>::const_iterator itr = src->stringmap.begin(); itr != src->stringmap.end(); ++itr)
        stringcount++;

    //src->src->data = (char*)malloc(src->asocmap.size()*(sizeof(unsigned int)+typesize) + stringcount*stringsize);

    FILE* f = fopen(file, "wb");
    if (!f)
    {
        printf("Error: File not saved!\n");
        return;
    }

    char atype = src->src->asoctype;
    char flags = src->src->flags;

    fwrite(SAS_MAGIC, sizeof(char), strlen(SAS_MAGIC), f);
    fwrite(&atype, sizeof(char), 1, f);
    fwrite(&flags, sizeof(char), 1, f);
    fwrite(&datacount, sizeof(unsigned int), 1, f);
    fwrite(&stringcount, sizeof(unsigned int), 1, f);

    for (std::map<unsigned int, void*>::const_iterator itr = src->asocmap.begin(); itr != src->asocmap.end(); ++itr)
    {
        fwrite(&itr->first, sizeof(unsigned int), 1, f);
        fwrite(itr->second, typesize, 1, f);
    }

    for (std::map<unsigned int, char*>::const_iterator itr = src->stringmap.begin(); itr != src->stringmap.end(); ++itr)
    {
        fwrite(itr->second, sizeof(char), strlen(itr->second)+1, f);
    }

    fclose(f);

    printf("Save successfull!\n");
}

const char* SASLoader::GetStringType(Asoc *src)
{
    switch (src->src->asoctype)
    {
        case TYPE_CHAR:
            return "char";
        case TYPE_SHORT:
            return "short";
        case TYPE_INT:
            return "integer";
        case TYPE_TEXT:
            return "text";
        case TYPE_LONG:
            return "long";
    }

    return "unknown";
}
