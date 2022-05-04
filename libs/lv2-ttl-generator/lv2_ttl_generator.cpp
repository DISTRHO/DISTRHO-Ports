/*
 * JUCE LV2 *.ttl generator
 */

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
 #include <windows.h>
 #define TTL_GENERATOR_WINDOWS
#else
 #include <dlfcn.h>
#endif

#ifndef nullptr
 #define nullptr (0)
#endif

// Replicating some of the LV2 header here so that we don't have to set up any
// custom include paths for this file.
// Normally this would be a bad idea, but the LV2 API has to keep these definitions
// in order to remain backwards-compatible.

extern "C" {

typedef struct LV2_Descriptor {
    const void* a;
    const void* b;
    const void* c;
    const void* d;
    const void* e;
    const void* f;
    const void* g;
    const void* (*extension_data)(const char* uri);
} LV2_Descriptor;

typedef struct RecallFeature {
    int (*doRecall)(const char*);
} RecallFeature;

}

typedef void (*TTL_Generator_Function)(const char* basename);
typedef const LV2_Descriptor* (*LV2_Descriptor_Function)(unsigned index);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("usage: %s /path/to/plugin-DLL\n", argv[0]);
        return 1;
    }

#ifdef TTL_GENERATOR_WINDOWS
    const HMODULE handle = LoadLibraryA(argv[1]);
#else
    void* const handle = dlopen(argv[1], RTLD_LAZY);
#endif

    if (! handle)
    {
#ifdef TTL_GENERATOR_WINDOWS
        printf("Failed to open plugin DLL\n");
#else
        printf("Failed to open plugin DLL, error was:\n%s\n", dlerror());
#endif
        return 2;
    }

#ifdef TTL_GENERATOR_WINDOWS
    const TTL_Generator_Function ttlFn = (TTL_Generator_Function)GetProcAddress(handle, "lv2_generate_ttl");
    const LV2_Descriptor_Function lv2Fn = (LV2_Descriptor_Function)GetProcAddress(handle, "lv2_descriptor");
#else
    const TTL_Generator_Function ttlFn = (TTL_Generator_Function)dlsym(handle, "lv2_generate_ttl");
    const LV2_Descriptor_Function lv2Fn = (LV2_Descriptor_Function)dlsym(handle, "lv2_descriptor");
#endif

    if (ttlFn != NULL)
    {
        char basename[strlen(argv[1])+1];

#ifdef TTL_GENERATOR_WINDOWS
        if (char* base2 = strrchr(argv[1], '\\'))
#else
        if (char* base2 = strrchr(argv[1], '/'))
#endif
        {
            strcpy(basename, base2+1);
            basename[strrchr(base2, '.')-base2-1] = '\0';
        }
        else
        {
#ifdef TTL_GENERATOR_WINDOWS
            // Fix when running through wine
            if (char* base2 = strrchr(argv[1], '/'))
            {
                strcpy(basename, base2+1);
                basename[strrchr(base2, '.')-base2-1] = '\0';
            }
            else
#endif
            {
                strcpy(basename, argv[1]);
            }
        }

        printf("Generate ttl data for '%s', basename: '%s'\n", argv[1], basename);

        ttlFn(basename);
    }
    else if (lv2Fn != nullptr)
    {
        if (const LV2_Descriptor* const descriptor = lv2Fn(0))
            if (const RecallFeature* const recallFeature = (const RecallFeature*)descriptor->extension_data("https://lv2-extensions.juce.com/turtle_recall"))
                recallFeature->doRecall(argv[1]);
    }
    else
    {
        printf("Failed to find 'lv2_generate_ttl' function\n");
    }

#ifdef TTL_GENERATOR_WINDOWS
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif

    return 0;
}
