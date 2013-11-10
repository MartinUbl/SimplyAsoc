#ifndef SIMPLYASOC_SAS_H
#define SIMPLYASOC_SAS_H

#include <map>

#pragma warning(disable:4996) // avoid safeness warning

#define SAS_MAGIC "SAS1"

enum AsocTypes
{
    TYPE_CHAR     = 'C',
    TYPE_SHORT    = 'S',
    TYPE_INT      = 'I',
    TYPE_LONG     = 'L',
    TYPE_TEXT     = 'T',
};

enum AsocFlags
{
    FLAG_UNSIGNED = 0x01,
};

#pragma pack(push)
#pragma pack(1)
struct SimpleAsoc
{
    char* magic;            // 4 chars
    AsocTypes asoctype : 8; // 1 char, type
    char  flags        : 8; // 1 char, content flags
    unsigned int count;     // 4 bytes, amount of asoc data
    char* data;
};
#pragma pack(pop)

class Asoc
{
public:
    bool IsUnsigned()
    {
        return src->flags & FLAG_UNSIGNED;
    }
    AsocTypes GetType()
    {
        return src->asoctype;
    }

    SimpleAsoc* src;

    std::map<unsigned int, void*> asocmap;
    std::map<unsigned int, char*> stringmap;

    bool IsPresent(unsigned int offset)
    {
        return (asocmap.find(offset) != asocmap.end());
    }

    template <class T>
    T GetNumValue(unsigned int offset)
    {
        return (T)*((T*)asocmap[offset]);
    }

    const char* GetStringValue(unsigned int offset)
    {
        int stringoff = GetNumValue<unsigned int>(offset);
        return stringmap[stringoff];
    }

    // true = exists, false = created
    bool SetNumValue(unsigned int offset, long value)
    {
        bool ret = true;

        if (asocmap.find(offset) == asocmap.end())
            ret = false;

        switch (GetType())
        {
            case TYPE_CHAR:
                asocmap[offset] = new char((char)value);
                break;
            case TYPE_SHORT:
                asocmap[offset] = new short((short)value);
                break;
            case TYPE_INT:
            case TYPE_TEXT:
                asocmap[offset] = new int((int)value);
                break;
            case TYPE_LONG:
                asocmap[offset] = new long((long)value);
                break;
            default:
                return false;
        }

        return ret;
    }

    bool SetStringValue(unsigned int offset, const char* value)
    {
        char* copyval = new char[strlen(value)+1];
        memset(copyval, 0, strlen(value)+1);
        strncpy(copyval, value, strlen(value));

        int stroff = 0;
        if (IsPresent(offset))
            stroff = GetNumValue<unsigned int>(offset);
        else
        {
            stroff = stringmap.size();
            SetNumValue(offset, stroff);
        }

        if (stringmap.find(stroff) == stringmap.end())
        {
            stringmap[stroff] = copyval;
            return false;
        }

        stringmap[stroff] = copyval;
        return true;
    }

    bool SetRawStringValue(unsigned int offset, const char* value)
    {
        char* copyval = new char[strlen(value)+1];
        memset(copyval, 0, strlen(value)+1);
        strncpy(copyval, value, strlen(value));

        stringmap[offset] = copyval;
        return true;
    }

    void PrintOut()
    {
        for (std::map<unsigned int, void*>::const_iterator itr = asocmap.begin(); itr != asocmap.end(); ++itr)
        {
            if (src->asoctype != TYPE_TEXT)
                printf("%d - %d\n", itr->first, GetNumValue<long>(itr->first));
            else
                printf("%d - %s\n", itr->first, GetStringValue(itr->first));
        }
    }
};

#endif
