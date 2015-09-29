#ifndef _MINA_GETOPT_H_INCLUDE_
#define _MINA_GETOPT_H_INCLUDE_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "Define.h"

MINA_NAMESPACE_START
#define MAX_ARG_LEN  64
class CGetOpt
{
public:
    enum
    {
         REQUIRE_ORDER = 1,
         PERMUTE_ARGS,
         RETURN_IN_ORDER
    };

    enum OPTION_ARG_MODE
    {
         NO_ARG = 0,
         ARG_REQUIRED,
         ARG_OPTIONAL
    };

public:
    CGetOpt(
        int aiAgrc, 
        char* apcAgrv[],
        char const* apcOptString = "",
        int   aiSkipArg = 0,
        bool  abReportErrors = false,
        int   aiOrdering = PERMUTE_ARGS,
        int   aiLongOnly = 0 );
    ~CGetOpt(void);

    int operator () (void);

    int LongOption(
        char const* apcName,
        OPTION_ARG_MODE aeHasArg = NO_ARG );

    int LongOption(
        char const* apcName,
        int aiShortOption,
        OPTION_ARG_MODE aeHasArg = NO_ARG );

    int Argc(void) const;

    char** Argv(void) const;

    char* OptArg(void) const;

    int OptOpt(void) const;

    int& OptInd (void);

    char const* LongOption(void) const;

    char const* LastOption(void) const;

    char const* OptString(void) const;

protected:
    struct CGetOptLongOption
    {
        CGetOptLongOption(
            char const* apcName,
            int aiHasArg,
            int aiVal = 0 );

        ~CGetOptLongOption(void);

        char* cpcName;
        int ciVal;
        int ciHasArg;


        CGetOptLongOption* cpoNext;
    };

protected:
    int Permute(void);

    void PermuteArgs(void);

    void LastOption( char acOpt );
    void LastOption( char const* apcLastOption, size_t aiLen );

    int NextChar(void);

    int LongOptionI(void);

    int ShortOption(void);

    unsigned long Gcb( unsigned long aiX, unsigned long aiY );

protected:
	bool mbOptErr;

    int miArgc;

    char** mppcArgv;

    char macOptString[MAX_ARG_LEN];

    char* mpcNextChar;

    int miOptInd;

    int miOptOpt;

    char* mpcOptArg;

    int miOrdering;

    int miLongOnly;

    int miHasColon;

    int miNonoptStart;

    int miNonoptEnd;

    char macLastOption[MAX_ARG_LEN];

    CGetOptLongOption* mpoLongOption;

    CGetOptLongOption* mpoLongOpts;
};

MINA_NAMESPACE_END
#endif
