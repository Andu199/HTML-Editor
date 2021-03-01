/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_TAG_ELEMENTS 1000
#define MAX_LENGTH_LINE 10240

typedef struct TNodAttr // atribut din nod
{
    char *name;
    char *value;
    struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo // informatia unui nod (inclusiv atributele)
{
    char *type;
    char *id;
    TAttr style;
    TAttr otherAttributes;
    int isSelfClosing;
    char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb // un nod contine informatia aferenta
{
    TInfo info;
    struct TNodArb *nextSibling;
    struct TNodArb *firstChild;
} TNodArb, *TArb;