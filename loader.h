#ifndef SIMPLYASOC_LOADER_H
#define SIMPLYASOC_LOADER_H

#include "sas.h"

class SASLoader
{
    public:
        static void Load(Asoc* dst, const char* file);
        static void Save(Asoc* src, const char* file);

        static const char* GetStringType(Asoc* src);
};

#endif
