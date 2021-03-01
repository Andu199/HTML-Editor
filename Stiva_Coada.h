/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct cell
{
    void* info;
    struct cell* next;
} TCG, *TLG;

/* STIVA */

typedef struct stiva
{
    size_t dim;
    TLG vf;
} TStiva, *ASt; // definiție

#define VF(a) (((ASt)(a))->vf)
#define DIMS(a) (((ASt)(a))->dim)
#define VIDAS(a) (VF(a) == NULL)  // macro-uri utile

void* InitS(size_t d);
int Push(void* s, void* ae);
int Pop(void* s, void* ae);
int Top(void* s, void* ae);
void ResetS(void* s);
void DistrS(void** s);
int MutaS(void* sd, void* ss);
int Rastoarna(void* sd, void* ss);
int Suprapune(void* sd, void* ss); // funcții specifice

/* COADA */

typedef struct coada
{
    size_t dim;
    TLG ic, sc;
} TCoada, *AQ; // definiție

#define IC(a) (((AQ)(a))->ic)
#define SC(a) (((AQ)(a))->sc)
#define DIMQ(a) (((AQ)(a))->dim)
#define VIDAQ(a) (IC(a) == NULL && SC(a) == NULL) // macro-uri utile

void* InitQ(size_t d);
int IntrQ(void *c, void *ae);
int ExtrQ(void *c, void *ae);
int PrimQ(void *c, void *ae);
void ResetQ(void *c);
void DistrQ(void **c);
int MutaQ(void *cd, void *cs);
int ConcatQ(void *cd, void *cs); // funcții specifice