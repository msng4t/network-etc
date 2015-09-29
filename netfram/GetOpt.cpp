#include "GetOpt.h"

MINA_NAMESPACE_START

CGetOpt::CGetOpt(
    int aiAgrc, 
    char* apcAgrv[],
    char const* apcOptString,
    int   aiSkipArg,
    bool  abReportErrors,
    int   aiOrdering,
    int   aiLongOnly )
    : mbOptErr(abReportErrors)
    , miArgc(aiAgrc)
    , mppcArgv(apcAgrv)
    , mpcNextChar(0)
    , miOptInd(aiSkipArg)
    , miOptOpt(0)
    , mpcOptArg(0)
    , miOrdering(aiOrdering)
    , miLongOnly(aiLongOnly)
    , miHasColon(0)
    , miNonoptStart(miOptInd)
    , miNonoptEnd(miOptInd)
    , mpoLongOption(NULL)
    , mpoLongOpts(NULL)
{
    snprintf( this->macOptString, sizeof(this->macOptString), "%s", apcOptString );

    // Now, check to see if any or the following were passed at
    // the begining of optstring: '+' same as POSIXLY_CORRECT;
    // '-' turns off POSIXLY_CORRECT; or ':' which signifies we
    // should return ':' if a parameter is missing for an option.
    // We use a loop here, since a combination of "{+|-}:" in any
    // order should be legal.
    int liDone  = 0;
    int liOffset = 0;
    while ( !liDone )
    {
        switch (this->macOptString[liOffset++])
        {
        case '+':
            this->miOrdering = REQUIRE_ORDER;
            break;
        case '-':
            this->miOrdering = RETURN_IN_ORDER;
            break;
        case ':':
            this->miHasColon = 1;
            break;
        default:
            // Quit as soon as we see something else...
            liDone = 1;
            break;
        }
    }

    // Initialize other argument
    memset( this->macLastOption, 0, sizeof(this->macLastOption) );
}

CGetOpt::~CGetOpt(void)
{
    CGetOptLongOption* lpoOption = mpoLongOpts;
    while( lpoOption != NULL )
    {
        CGetOptLongOption* lpoTmp = lpoOption;
        lpoOption = lpoOption->cpoNext;
        delete lpoTmp;
    }
}

int CGetOpt::operator () (void)
{
    // First of all, make sure we reinitialize any pointers..
    this->miOptOpt = 0;
    this->mpoLongOption = 0;

    if ( this->mppcArgv == 0 )
    {
        // It can happen, e.g., on VxWorks.
        this->miOptInd = 0;
        return -1;
    }

    // We check this because we can string short options together if the
    // preceding one doesn't take an argument.
    if ( this->mpcNextChar == 0 || *this->mpcNextChar == '\0' )
    {
        int liRetval = this->NextChar();
        if ( liRetval != 0 )
        {
            return liRetval;
        }
    }

    if ( ( (this->mppcArgv[this->miOptInd][0] == '-')
        && (this->mppcArgv[this->miOptInd][1] == '-') ) || this->miLongOnly )
        return this->LongOptionI ();

    return this->ShortOption();
}

int CGetOpt::Argc(void) const
{
    return this->miArgc;
}

char** CGetOpt::Argv(void) const
{
    return this->mppcArgv;
}

char* CGetOpt::OptArg(void) const
{
    return this->mpcOptArg;
}

int CGetOpt::OptOpt(void) const
{
    return this->miOptOpt;
}

int& CGetOpt::OptInd(void)
{
    return this->miOptInd;
}

char const* CGetOpt::LongOption(void) const
{
    if ( this->mpoLongOption )
    {
        return this->mpoLongOption->cpcName;
    }
    
    return 0;
}

char const* CGetOpt::LastOption(void) const
{
    return this->macLastOption;
}

void CGetOpt::LastOption( char const* apcLastOption, size_t aiLen )
{
    strncpy( macLastOption, apcLastOption, aiLen );
    this->macLastOption[aiLen] = '\0';
}

void CGetOpt::LastOption( char acOpt )
{
    this->macLastOption[0] = acOpt;
    this->macLastOption[1] = '\0';
}

int CGetOpt::Permute(void)
{
    if( this->miNonoptStart != this->miNonoptEnd
        && this->miNonoptStart != this->miOptInd )
    {
        this->PermuteArgs();
    }

    this->miNonoptStart = this->miOptInd;

    // Skip over args untill we find the next option.
    while( this->miOptInd < this->miArgc
        && (this->mppcArgv[this->miOptInd][0] != '-'
        || this->mppcArgv[this->miOptInd][1] == '\0') )
    {
        this->miOptInd++;
    }

    // Got an option, so mark this as the end of the non options.
    this->miNonoptEnd = this->miOptInd;

    if ( this->miOptInd != this->miArgc
        && strcmp( this->mppcArgv[this->miOptInd], "--" ) == 0 )
    {
        // We found the marker for the end of the options.
        this->miOptInd++;

        if ( this->miNonoptStart != this->miNonoptEnd
            && this->miNonoptEnd != this->miOptInd )
        {
            this->PermuteArgs ();
        }
    }

    if ( this->miOptInd == this->miArgc )
    {
        if ( this->miNonoptStart != this->miNonoptEnd )
        {
            this->miOptInd = this->miNonoptStart;
        }
        return EOF;
    }

    return 0;
}

char const* CGetOpt::OptString( void ) const
{
    return this->macOptString;
}

CGetOpt::CGetOptLongOption::CGetOptLongOption (
    char const* apcName,
    int aeHasArg,
    int aiVal )
    : cpcName( strdup(apcName) )
    , ciVal(aiVal)
    , ciHasArg(aeHasArg)
{
}

CGetOpt::CGetOptLongOption::~CGetOptLongOption(void)
{
    free(this->cpcName);
}

int CGetOpt::LongOption( char const* apcName, OPTION_ARG_MODE aeHasArg )
{
    return this->LongOption( apcName, 0, aeHasArg );
}

int CGetOpt::LongOption( char const* apcName, int aiShortOption, OPTION_ARG_MODE aeHasArg )
{
    // We only allow valid alpha-numeric characters as short options.
    // If short_options is not a valid alpha-numeric, we can still return it
    // when the long option is found, but won't allow the caller to pass it on
    // the command line (how could they?).  The special case is 0, but since
    // we always return it, we let the caller worry about that.
    if ( aiShortOption > 0 &&
         aiShortOption < 256 &&
         isalnum( static_cast<char>(aiShortOption) ) != 0)
    {
        // If the short_option already exists, make sure it matches, otherwise
        // add it.
        char* lpS = 0;
        if ( ( lpS = const_cast<char*>(
             strchr(this->macOptString, aiShortOption) ) ) != 0)
        {
            // Short option exists, so verify the argument options
            if ( lpS[1] == ':')
            {
                if ( lpS[2] == ':')
                {
                    if ( aeHasArg != ARG_OPTIONAL )
                    {
                        if ( this->mbOptErr )
                        {
                            fprintf(stderr,
                                "Existing short option '%c' takes "
                                "optional argument; adding %s "
                                "requires ARG_OPTIONAL\n", (char)aiShortOption, apcName );
                        }
                        return -1;
                    }
                }
                else
                    if ( aeHasArg != ARG_REQUIRED )
                    {
                        if ( this->mbOptErr )
                        {
                            fprintf(stderr, 
                                "Existing short option '%c' takes "
                                "optional argument; adding %s "
                                "requires ARG_REQUIRED\n", (char)aiShortOption, apcName );
                        }
                        return -1;
                    }
            }
            else if ( aeHasArg != NO_ARG )
            {
                if ( this->mbOptErr )
                {
                    fprintf(stderr, 
                        "Existing short option '%c' does not "
                        "accept an argument; adding %s "
                        "requires NO_ARG\n",(char)aiShortOption, apcName );
                }
                return -1;
            }
        }
        else
        {
            // Didn't find short option, so add it...
            char liTmp[2] = { aiShortOption, 0 };
            strncat( this->macOptString, liTmp, 2 );

            if ( aeHasArg == ARG_REQUIRED )
            {
                strncat( this->macOptString, ":", 2 );
            }
            else if (aeHasArg == ARG_OPTIONAL)
            {
                strncat( this->macOptString, "::", 3 );
            }
        }
    }

    CGetOptLongOption* lpoOption = NULL;
    lpoOption = new CGetOptLongOption( apcName, aeHasArg, aiShortOption );
    assert (lpoOption != NULL);

    if ( !lpoOption )
    {
        return -1;
    }

    // Add to array
    if ( this->mpoLongOpts != NULL )
    {
        lpoOption->cpoNext = mpoLongOpts;
        this->mpoLongOpts = lpoOption;
    }
    else
    {
        lpoOption->cpoNext = NULL;
        this->mpoLongOpts = lpoOption;
    }

    return 0;
}

void CGetOpt::PermuteArgs( void )
{
    unsigned long liCycleLen = 0;
    unsigned long liNcycle = 0;
    unsigned long liNnonOpts = 0;
    unsigned long liNopts = 0;
    unsigned long liOptEnd = this->miOptInd;
    int liCStart, liPos = 0;
    char* lpsSwap = 0;

    liNnonOpts = this->miNonoptEnd - this->miNonoptStart;
    liNopts    = liOptEnd - this->miNonoptEnd;
    liNcycle   = this->Gcb(liNnonOpts, liNopts);
    liCycleLen = (liOptEnd - this->miNonoptStart) / liNcycle;

    this->miOptInd = this->miOptInd - liNnonOpts;

    for( unsigned long i = 0; i < liNcycle; i++ )
    {
        liCStart = this->miNonoptEnd + i;
        liPos = liCStart;
        for ( unsigned long j = 0; j < liCycleLen; j++ )
        {
            if ( liPos >= this->miNonoptEnd )
            {
                liPos -= liNnonOpts;
            }
            else
            {
                liPos += liNopts;
            }
            
            lpsSwap = this->mppcArgv[liPos];

            ((char**)this->mppcArgv)[liPos] = this->mppcArgv[liCStart];

            ((char**)this->mppcArgv)[liCStart] = lpsSwap;
        }
    }
}

int CGetOpt::NextChar (void)
{
    if ( this->miOrdering == PERMUTE_ARGS )
    {
        if ( this->Permute () == EOF )
            return EOF;
    }

    // Update scanning pointer.
    if ( this->miOptInd >= this->miArgc )
    {
        // We're done...
        this->mpcNextChar = 0;
        return EOF;
    }
    else if ( *( this->mpcNextChar = this->mppcArgv[this->miOptInd] ) != '-'
        || this->mpcNextChar[1] == '\0' )
    {
        // We didn't get an option.
        if ( this->miOrdering == REQUIRE_ORDER
            || this->miOrdering == PERMUTE_ARGS ) 
        {
            // If we permuted or require the options to be in order, we're done.
            return EOF;
        }

        // It must be RETURN_IN_ORDER...
        this->mpcOptArg = this->mppcArgv[this->miOptInd++];
        this->mpcNextChar = 0;
        return 1;
    }
    else if ( this->mpcNextChar[1] != 0
        && *++this->mpcNextChar == '-'
        && this->mpcNextChar[1] == 0 )
    {
        // Found "--" so we're done...
        ++this->miOptInd;
        this->mpcNextChar = 0;
        return EOF;
    }

    // If it's a long option, and we allow long options advance NextChar.
    if ( *this->mpcNextChar == '-' && this->mpoLongOpts != NULL )
    {
        this->mpcNextChar++;
    }

    return 0;
}

int CGetOpt::LongOptionI( void )
{
    CGetOptLongOption* lpoLongOpt = NULL;
    char* lpS = this->mpcNextChar;
    int liHits  = 0;
    int liExact = 0;
    CGetOptLongOption* lpoFound = 0;

    // Advance to the end of the long option name so we can use
    // it to get the length for a string compare.
    while ( *lpS && *lpS != '=' )
    {
        lpS++;
    }

    size_t liLen = lpS - this->mpcNextChar;
    // set LastOption to NextChar, up to the '='.
    this->LastOption( this->mpcNextChar, liLen );

    size_t liOptionIndex = 0;
    for( lpoLongOpt = this->mpoLongOpts; 
        lpoLongOpt != NULL ; lpoLongOpt = lpoLongOpt->cpoNext )
    {
        if ( !strncmp( lpoLongOpt->cpcName, this->mpcNextChar, liLen ) )
        {
            // Got at least a partial match.
            lpoFound = lpoLongOpt;
            liHits += 1;
            if ( liLen == strlen(lpoLongOpt->cpcName) )
            {
                // And in fact, it's an exact match, so let's use it.
                liExact = 1;
                break;
            }
        }
        liOptionIndex++;
    }

    if( (liHits > 1) && !liExact )
    {
        // Great, we found a match, but unfortunately we found more than
        // one and it wasn't exact.
        if ( this->mbOptErr )
        {
            fprintf(stderr, "%s: option `%s' is ambiguous\n",
                this->mppcArgv[0], this->mppcArgv[this->miOptInd] );
        }
        this->mpcNextChar = 0;
        this->miOptInd++;
        return '?';
    }

    if( lpoFound != 0 )
    {
        // Okay, we found a good one (either a single hit or an exact match).
        // OptionIndex = indfound;
        this->miOptInd++;
        if( *lpS )
        {
            // s must point to '=' which means there's an argument (well
            // close enough).
            if( lpoFound->ciHasArg != NO_ARG)
            {
                // Good, we want an argument and here it is.
                this->mpcOptArg = ++lpS;
            }
            else
            {
                // Whoops, we've got what looks like an argument, but we
                // don't want one.
                if ( this->mbOptErr )
                {
                    fprintf(stderr, 
                        "%s: long option `--%s' doesn't allow an argument\n",
                        this->mppcArgv[0], lpoFound->cpcName );
                }
                // The spec doesn't cover this, so we keep going and the program
                // doesn't know we ignored an argument if OptErr is off!!!
            }
        }
        else if( lpoFound->ciHasArg == ARG_REQUIRED )
        {
            // s didn't help us, but we need an argument. Note that
            // optional arguments for long options must use the "=" syntax,
            // so we won't get here in that case.
            if ( this->miOptInd < this->miArgc )
            {
                // We still have some elements left, so use the next one.
                this->mpcOptArg = this->mppcArgv[this->miOptInd++];
            }
            else
            {
                // All out of elements, so we have to punt...
                if ( this->mbOptErr )
                {
                    fprintf(stderr, 
                        "%s: long option '--%s' requires an argument\n",
                        this->mppcArgv[0], lpoFound->cpcName );
                }
                this->mpcNextChar = 0;
                this->miOptOpt = lpoFound->ciVal;   // Remember matching short equiv
                return this->miHasColon ? ':' : '?';
            }
        }
        this->mpcNextChar = 0;
        this->mpoLongOption = lpoFound;
        // Since val_ has to be either a valid short option or 0, this works
        // great.  If the user really wants to know if a long option was passed.
        this->miOptOpt = lpoFound->ciVal;
        return lpoFound->ciVal;
    }
    if ( !this->miLongOnly || this->mppcArgv[this->miOptInd][1] == '-'
        || strchr( this->macOptString, *this->mpcNextChar ) == NULL )
    {
        // Okay, we couldn't find a long option.  If it isn't long_only (which
        // means try the long first, and if not found try the short) or a long
        // signature was passed, e.g. "--", or it's not a short (not sure when
        // this could happen) it's an error.
        if ( this->mbOptErr )
        {
            fprintf(stderr, 
                "%s: illegal long option '--%s'\n", this->mppcArgv[0], this->mpcNextChar );
        }
        this->mpcNextChar = 0;
        this->miOptInd++;
        return '?';
    }
    return this->ShortOption();
}

int CGetOpt::ShortOption( void )
{
    /* Look at and handle the next option-character.  */
    char lcOpt = *this->mpcNextChar++;
    // Set LastOption to lcOpt
    this->LastOption(lcOpt);

    char* lpcOli = strchr( this->macOptString, lcOpt );

    /* Increment `optind' when we start to process its last character.  */
    if ( *this->mpcNextChar == '\0' )
        ++this->miOptInd;

    if ( lpcOli == 0 || lcOpt == ':' )
    {
        if ( this->mbOptErr )
        {
            fprintf(stderr, "%s: illegal short option -- %c\n", this->mppcArgv[0], lcOpt );
        }
        return '?';
    }

    if ( lcOpt == 'W' && lpcOli[1] == ';' )
    {
        if ( this->mpcNextChar[0] == 0 )
            this->mpcNextChar = this->mppcArgv[this->miOptInd];

        return this->LongOptionI();
    }

    this->miOptOpt = lpcOli[0];      // Remember the option that matched
    if ( lpcOli[1] == ':' )
    {
        if ( lpcOli[2] == ':' )
        {
            // Takes an optional argument, and since short option args must
            // must follow directly in the same argument, a NULL Nextchar
            // means we didn't get one.
            if ( *this->mpcNextChar != '\0' )
            {
                this->mpcOptArg = this->mpcNextChar;
                this->miOptInd++;
            }
            else
            {
                this->mpcOptArg = 0;
            }
            this->mpcNextChar = 0;
        }
        else
        {
            // Takes a required argument.
            if ( *this->mpcNextChar != '\0' )
            {
                // Found argument in same argv-element.
                this->mpcOptArg = this->mpcNextChar;
                this->miOptInd++;
            }
            else if ( this->miOptInd == this->miArgc )
            {
                // Ran out of arguments before finding required argument.
                if ( this->mbOptErr )
                {
                    fprintf(stderr,
                        "%s: short option requires an argument -- %c\n", this->mppcArgv[0], lcOpt );
                }
                lcOpt = this->miHasColon ? ':' : '?';
            }
            else
            {
                // Use the next argv-element as the argument.
                this->mpcOptArg = this->mppcArgv[this->miOptInd++];
            }
            this->mpcNextChar = 0;
        }
    }
    return lcOpt;
}

unsigned long CGetOpt::Gcb( unsigned long aiX, unsigned long aiY )
{
    while( aiY != 0 )
    {
        unsigned long liR = aiX % aiY;
        aiX = aiY;
        aiY = liR;
    }

    return aiX;
}


MINA_NAMESPACE_END
