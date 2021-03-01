/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "def_types.h"
#include "Stiva_Coada.h"

/*   automat de stari   */

typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;

/* functiile din interpret.c care sunt apelate si in afara acestui fisier */

TParseState Interpret(TParseState currentState, char c, TArb *a,
void *st_parinti, char *name, char *value);
void createStyle(TArb *a, char *name, char *value);
char *itoa(int x);