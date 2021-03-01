/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include "Stiva_Coada.h"

/* STIVA */

void* InitS(size_t d) // inițializare stivă
{
    ASt s;
    s = (ASt)malloc(sizeof(TStiva));
    if(!s)
        return NULL;

    s->dim = d;
    s->vf = NULL;
    
    return (void*) s;
}

int Push(void* s, void* ae) // introducere în vârful stivei
{
    TLG aux = (TLG)malloc(sizeof(TCG));
    if(!aux)
        return 0;
    aux->info = malloc(DIMS(s));
    if(!aux->info)
    {
        free(aux);
        return 0;
    }
    memcpy(aux->info, ae, DIMS(s));
    aux->next = VF(s);
    VF(s) = aux;
    return 1;
}

int Pop(void* s, void* ae) // scoatere din vârful stivei
{
    if(VIDAS(s))
        return 0;
    memcpy(ae, VF(s)->info, DIMS(s));
    TLG aux = VF(s);
    VF(s) = aux->next;
    free(aux->info);
    free(aux);
    return 1;
}

int Top(void* s, void* ae) // copiere vârf
{
    if(VIDAS(s))
        return 0;
    memcpy(ae, VF(s)->info, DIMS(s));
    return 1;
}

void ResetS(void* s) // golire stivă
{
    TLG p = VF(s), aux;
    while(p)
    {
        aux = p;
        p = p->next;
        free(aux);
    }
    VF(s) = NULL;
}

void DistrS(void** s) // distrugere stivă
{
    ResetS(*s);
    free(*s);
    *s = NULL;
}

int MutaS(void* sd, void* ss) /* mută vârful din stiva sursă în stiva
destinație */
{
    if(DIMS(sd) != DIMS(ss))
        return -1;
    if(VIDAS(ss))
        return 0;
    TLG aux = VF(ss);
    VF(ss) = VF(ss)->next;
    aux->next = VF(sd);
    VF(sd) = aux;
    return 1; 
}

int Rastoarna(void* sd, void* ss) /* răstoarnă stiva sursă în stiva
destinație */
{
    if(DIMS(sd) != DIMS(ss))
        return -1;
    if(VIDAS(ss))
        return 0;
    while(VF(ss))
        MutaS(sd, ss);
    return 1;
}

int Suprapune(void* sd, void* ss) /* suprapune conținutul din stiva sursă în
stiva destinație */
{
    if(DIMS(sd) != DIMS(ss))
        return -1;
    if(VIDAS(ss))
        return 0;
    TLG aux, p;
    for(p = VF(ss); p->next != NULL; p = p->next)
    {}
    aux = VF(sd);
    VF(sd) = VF(ss);
    VF(ss) = NULL;
    p->next = aux;
    return 1;
}

/* COADA */

void* InitQ(size_t d) // inițializare coadă
{
    AQ a = (AQ)malloc(sizeof(TCoada));
    if(!a)
        return NULL;
    
    a->dim = d;
    a->ic = a->sc = NULL;

    return (void* )a;
}

int IntrQ(void *c, void *ae) // introducere în coadă
{
    TLG aux = (TLG)malloc(sizeof(TCG));
    if(!aux)
        return 0;
    aux->info = malloc(DIMQ(c));
    if(!aux->info)
    {
        free(aux);
        return 0;
    }
    aux->next = NULL;
    memcpy(aux->info, ae, DIMQ(c));
    if(VIDAQ(c))
        IC(c) = SC(c) = aux;
    else
    {
        SC(c)->next = aux;
        SC(c) = aux;
    }
    return 1;
}

int ExtrQ(void *c, void *ae) // extragere din coadă
{
    if(VIDAQ(c))
        return 0;
    TLG aux = IC(c);
    IC(c) = aux->next;
    memcpy(ae, aux->info, DIMQ(c));
    free(aux->info);
    free(aux);
    if(IC(c) == NULL)
        SC(c) = NULL;
    return 1;
}

int PrimQ(void *c, void *ae) // copiere primul element din coadă
{
    if(VIDAQ(c))
        return 0;
    memcpy(ae, IC(c)->info, DIMQ(c));
    return 1;
}

void ResetQ(void *c) // golire coadă
{
    TLG p = IC(c), aux;
    while(p)
    {
        aux = p;
        p = p->next;
        free(aux->info);
        free(aux);
    }
    IC(c) = SC(c) = NULL;
}

void DistrQ(void **c) // distrugere coadă
{
    ResetQ(*c);
    free(*c);
    *c = NULL;
}

int MutaQ(void *cd, void *cs) /* mută primul element din coada sursă în coada
destinație */
{
    if(DIMQ(cd) != DIMQ(cs))
        return -1;
    if(VIDAQ(cs))
        return 0;
    if(VIDAQ(cd))
        IC(cd) = SC(cd) = IC(cs);
    else
    {
        SC(cd)->next = IC(cs);
        SC(cd) = IC(cs);
    }
    IC(cs) = IC(cs)->next;
    return 1;
}

int ConcatQ(void *cd, void *cs) /* concatenează informația din coada sursă în
coada destinație */
{
    if(DIMQ(cd) != DIMQ(cs))
        return -1;
    if(VIDAQ(cs))
        return 0;
    if(VIDAQ(cd))
    {
        IC(cd) = IC(cs);
        SC(cd) = SC(cs);
    }
    else
    {
        SC(cd)->next = IC(cs);
        SC(cd) = SC(cs);
    }
    IC(cs) = SC(cs) = NULL;
    return 1;
}