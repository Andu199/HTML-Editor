/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "interpret.h"

/* functiile din utils.c care sunt apelate si in afara acestui fisier */

TArb Begin(FILE *in);
void End(TArb *a);
void formatText(TArb a, FILE *out, int niv);
void addTag(TArb a, char *command, FILE *out);
void findNode(TArb arb, char *buffer, FILE *out);