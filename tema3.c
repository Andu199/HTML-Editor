/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void solve(TArb *arb, FILE *in, FILE *out) /* functie care citeste comenziile
si executa */
{
    char *buffer = malloc((MAX_LENGTH_LINE + 1) * sizeof(char));
    if(!buffer)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    int commands_no, i;
    fscanf(in, "%d", &commands_no);
    fgets(buffer, MAX_LENGTH_LINE, in); // restul liniei
    for(i = 0; i < commands_no; ++i)
    {
        fgets(buffer, MAX_LENGTH_LINE, in);
        if(buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';
        if(!strcmp(buffer, "format"))
            formatText(*arb, out, 0);
        else if (buffer[1] == 'd') // adica comanda add
            addTag(*arb, buffer, out);
        else
            findNode(*arb, buffer, out);
    }
    free(buffer);
}

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        fprintf(stderr, "ERROR! Not enough parameters\n");
        return 1;
    }
    FILE *in = fopen(argv[1], "rt");
    if(!in)
    {
        fprintf(stderr, "ERROR! File opening\n");
        return 1;
    }
    TArb arb = Begin(in); // functie de pornire
    if(!arb)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return 1;
    }
    fclose(in);
    in = fopen(argv[2], "rt");
    if(!in)
    {
        fprintf(stderr, "ERROR! File opening\n");
        End(&arb);
        return 1;
    }
    FILE *out = fopen(argv[3], "wt");
    if(!out)
    {
        fprintf(stderr, "ERROR! File opening\n");
        End(&arb);
        fclose(in);
        return 1;
    }
    solve(&arb, in, out);
    fclose(in); fclose(out);
    End(&arb); // functia de sfarsit (dezaloca tot)
    return 0;
}