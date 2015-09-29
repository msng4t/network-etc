#include "Dir.h"
MINA_NAMESPACE_START

int CDir::Make(char const* apcDirPath, int aiMode)
{
    char const* lpcDirPath = apcDirPath;
    char        lacDir[1024];
    if (apcDirPath == NULL || strlen(apcDirPath) >= 1024)
    {
        errno = EINVAL;
        return -1;
    }

    while((lpcDirPath = strchr(lpcDirPath, '/')) != NULL)
    {
        if ((uint32_t)(lpcDirPath - apcDirPath) >= (uint32_t)1024)
        {
            errno = EINVAL;
            return -1;
        }
        else if ((uint32_t)(lpcDirPath - apcDirPath) > 0)
        {
            memcpy(lacDir, apcDirPath, lpcDirPath - apcDirPath);
            lacDir[lpcDirPath - apcDirPath] = '\0';

            errno = 0;
            if (mkdir(lacDir, aiMode) != 0 && errno != EEXIST)
                return -1;
        }
        lpcDirPath += 1;
    }
    errno = 0;
    if (mkdir(apcDirPath, aiMode) != 0 && errno != EEXIST)
        return -1;
    return 0;
}

int Remove(char const* apcDirPath)
{
    //not implementation
    return 0;
}
MINA_NAMESPACE_END
