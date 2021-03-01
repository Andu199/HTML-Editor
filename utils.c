/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include "utils.h"

TArb Begin(FILE *in) /* functie de initializare a arborelui: initializeaza si
citeste cu ajutorul functiei Interpret. Returneaza radacina arborelui */
{
    TArb a = NULL;
    void *parinti = InitS(sizeof(TArb));
    if(!parinti)
        return NULL;
    char *buffer = malloc((MAX_LENGTH_LINE + 1) * sizeof(char));
    if(!buffer)
    {
        DistrS(parinti);
        return NULL;
    }
    char *name = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!name)
    {
        DistrS(parinti);
        free(buffer);
        return NULL;
    }
    char *value = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!value)
    {
        DistrS(parinti);
        free(name);
        free(buffer);
        return NULL;
    }
    TParseState curr = PARSE_CONTENTS;
    unsigned int i;
    while(fgets(buffer, MAX_LENGTH_LINE, in))
    {
        if(buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';
        for(i = 0; i < strlen(buffer); ++i)
            curr = Interpret(curr, buffer[i], &a, parinti, name, value);
    }
    free(buffer);
    free(name);
    free(value);
    DistrS(&parinti);
    return a;
}

void freeNode(TArb a)// functie auxiliara care dezaloca informatia dintr-un nod
{
    if(a->info->contents)
        free(a->info->contents);
    if(a->info->id)
        free(a->info->id);
    if(a->info->type)
        free(a->info->type);
    TAttr att = a->info->style, auxatt;
    while(att)
    {
        auxatt = att;
        free(att->name);
        free(att->value);
        att = att->next;
        free(auxatt);
    }
    att = a->info->otherAttributes;
    while(att)
    {
        auxatt = att;
        free(att->name);
        free(att->value);
        att = att->next;
        free(auxatt);
    }
    free(a->info);
}

void End(TArb *a)// functie de final, care elibereaza memoria alocata arborelui
{
    // dezalocarea se face cu ajutorul unei cozi (in latime)
    void *coada = InitQ(sizeof(TNodArb));
    if(!coada)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    IntrQ(coada, *a); // introducere radacina
    void *aux = malloc(sizeof(TNodArb));
    if(!aux)
    {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        if( ((TArb) aux)->firstChild )
        {
            IntrQ(coada, ((TArb) aux)->firstChild); // introduce primul copil
            TArb sibling = ((TArb) aux)->firstChild->nextSibling, auxsibling;
            free(((TArb) aux)->firstChild);
            while(sibling)
            {
                IntrQ(coada, sibling); // introduce restul copiilor
                auxsibling = sibling;
                sibling = sibling->nextSibling;
                free(auxsibling);
            }
        }
        freeNode((TArb) aux);
    }
    free(aux);
    DistrQ(&coada);
    free(*a);
    (*a) = NULL;
}

void formatText(TArb a, FILE *out, int niv) /* functie recursiva de parcurgere
in adancime folosita pentru afisare */
{
    int i = 0;
    for(i = 0; i < niv; ++i)
        fprintf(out, "\t");
    fprintf(out, "<%s", a->info->type);
    if(a->info->style)
    {
        fprintf(out, " style=\"");
        TAttr atr = a->info->style;
        for(; atr != NULL; atr = atr->next)
        {
            if(atr == a->info->style)
                fprintf(out, "%s: %s;", atr->name, atr->value);
            else
                fprintf(out, " %s: %s;", atr->name, atr->value);
        }
        fprintf(out, "\"");
    }
    if(a->info->otherAttributes)
    {
        TAttr atr = a->info->otherAttributes;
        for(; atr != NULL; atr = atr->next)
            fprintf(out, " %s=\"%s\"", atr->name, atr->value);
    }
    if(a->info->isSelfClosing == 0)
        fprintf(out, ">\n");
    else
        fprintf(out, "/>\n");
    if(a->info->contents[0] != '\0')
    {
        for(i = 0; i < niv; ++i)
            fprintf(out, "\t");
        fprintf(out, "\t%s\n", a->info->contents);
    }
    if(a->firstChild)
        formatText(a->firstChild, out, niv + 1); // pentru copil
    if(a->info->isSelfClosing == 0)
    {
        for(i = 0; i < niv; ++i)
            fprintf(out, "\t");
        fprintf(out, "</%s>\n", a->info->type);
    }
    if(a->nextSibling)
        formatText(a->nextSibling, out, niv); // pentru urmatorul frate
}

void addTag(TArb a, char *command, FILE *out) /* adauga un nou copil la tag-ul
cu id-ul specificat in comanda */
{
    char *token = strtok(command, "=");
    token = strtok(NULL, "=");
    char *id = calloc(strlen(token) - 1, sizeof(char));
    if(!id) {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    unsigned int i = 0;
    while(token[i] != 32) { // adica token[i] este diferit de spatiu
        id[i] = token[i];
        ++i;
    }
    token = strtok(NULL, "=");
    char *id_copy = calloc(strlen(token) - 1, sizeof(char));
    if(!id_copy) {
        free(id); fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    strcpy(id_copy, id);
    char *id_token = strtok(id_copy, "."); // extragem fiecare pozitie din id
    while(id_token) {
        int poz = atoi(id_token);
        if(!a->firstChild) {
            fprintf(out, "Add tag failed: node with id %s not found!\n",
            id); // caz in care nu gasim nod la pozitia dorita
            free(id_copy); free(id);
            return;
        }
        a = a->firstChild;
        int j;
        for(j = 0; j < poz - 1; ++j) {
            if(!a->nextSibling) {
                fprintf(out, "Add tag failed: node with id %s not found!\n",
                id); // caz in care nu gasim nod la pozitia dorita
                free(id_copy); free(id);
                return;
            }
            a = a->nextSibling;
        }
        id_token = strtok(NULL, ".");
    }
    free(id_copy);
    TParseState curr = PARSE_CONTENTS;
    void *stiva_falsa = InitS(sizeof(TArb));
    if(!stiva_falsa) { // creata pt a fi data ca argument functiei Interpret
        free(id); fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    char *name = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!name) {
        DistrS(stiva_falsa); free(id);
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    char *value = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!value) {
        free(name); DistrS(stiva_falsa); free(id);
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    TArb new_node = NULL;
    for(i = 1; i < strlen(token) - 1; ++i)
        curr = Interpret(curr, token[i], &new_node, stiva_falsa, name, value);
    // apelam functia interpret pt fiecare caracter pt a crea un nou nod
    free(name); free(value); DistrS(&stiva_falsa);
    if(a->firstChild) {
        int poz = 1;
        a = a->firstChild;
        for(; a->nextSibling; a = a->nextSibling)
            poz ++;
        a->nextSibling = new_node;
        new_node->info->id = calloc(strlen(id) + 5, sizeof(char));
        if(!new_node->info->id) {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(new_node->info->id, id);
        strcat(new_node->info->id, ".");
        char *num = itoa(poz + 1); // creem id-ul
        if(!num)
            return;
        strcat(new_node->info->id, num);
        free(num);
    }
    else {
        a->firstChild = new_node;
        new_node->info->id = calloc(strlen(id) + 5, sizeof(char));
        if(!new_node->info->id) {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(new_node->info->id, a->info->id);
        strcat(new_node->info->id, ".1");
    }
    free(id);
}

void overrideNode(TArb *node, char *style) /* dezaloca toate atributele de tip
style din nod si apeleaza functia createStyle */
{
    char *name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!name)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    if((*node)->info->style)
    {
        TAttr style = (*node)->info->style, auxstyle;
        while(style)
        {
            auxstyle = style;
            style = style->next;
            free(auxstyle->value);
            free(auxstyle->name);
            free(auxstyle);
        }
        (*node)->info->style = NULL;
    }
    createStyle(node, name, style);
    free(name);
}

void aux_appendNode(TArb *node, char *name, char *value) /* se parcurge lista
de atribute de tip style si se creaza un nou atribut cu stringurile primite ca
parametru */
{
    TAttr atr = (*node)->info->style, ant_atr = NULL;
    while(atr)
    {
        if(!strcmp(atr->name, name))
        {
            if(!strcmp(atr->value, value))
                return;
            strcpy(atr->value, value);
            return;
        }
        ant_atr = atr;
        atr = atr->next;
    }
    if(ant_atr) // nu este primul atribut
    {
        ant_atr->next = malloc(sizeof(TNodAttr));
        if(!ant_atr->next)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        atr = ant_atr->next;
        atr->next = NULL;
        atr->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!atr->name)
        {
            free(atr);
            ant_atr->next = NULL;
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        atr->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!atr->value)
        {
            free(atr->name);
            free(atr);
            ant_atr->next = NULL;
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(atr->name, name);
        strcpy(atr->value, value);
    }
    else // este primul atribut
    {
        (*node)->info->style = malloc(sizeof(TNodAttr));
        if(!(*node)->info->style)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        atr = (*node)->info->style;
        atr->next = NULL;
        atr->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!atr->name)
        {
            free(atr);
            (*node)->info->style = NULL;
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        atr->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!atr->value)
        {
            free(atr->name);
            free(atr);
            (*node)->info->style = NULL;
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(atr->name, name);
        strcpy(atr->value, value); 
    }
}

void appendNode(TArb *node, char *style) /* desparte string-ul style in perechi
(name,value) urmand sa fie apelata functia aux_appendNode pt fiecare pereche */
{
    char *name = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!name)
    {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *value = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!value)
    {
        free(name);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    unsigned int i;
    int state = 0;
    for(i = 0; i < strlen(style); ++i)
        if(!isspace(style[i]))
        {
            if(style[i] == ':')
                state = 1; // state = 1 => urmeaza value
            else if(style[i] == ';')
            {
                state = 0; // state = 0 => urmeaza name
                aux_appendNode(node, name, value);
                name[0] = '\0'; value[0] = '\0';
            }
            else
            {
                if(state == 0)
                    strncat(name, &style[i], 1);
                else
                    strncat(value, &style[i], 1);
            }
        }
    free(name); free(value);
}

int isAncestor(TArb arb, char *ancestor_name, char *id_child) /* functie
auxiliara care verifica daca nodul cu tag-ul "ancestor_name" este stramosul
nodului cu id-ul "id_child" (arb este radacina arborelui). */
{
    if(!strcmp(arb->info->type, ancestor_name))
        return 1;
    char *id_copy = calloc(strlen(id_child) + 1, sizeof(char));
    if(!id_copy)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return 0;
    }
    strcpy(id_copy, id_child);
    char *ant_token = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!ant_token)
    {
        free(id_copy);
        fprintf(stderr, "ERROR! Allocation\n");
        return 0;
    }
    char *token = strtok(id_copy, ".");
    strcpy(ant_token, token);
    token = strtok(NULL, ".");
    while(token)
    {
        int poz = atoi(ant_token), i;
        if(!arb->firstChild) // daca nu se ajunge la pozitia dorita
        {
            free(id_copy);
            free(ant_token);
            return 0;
        }
        arb = arb->firstChild;
        for(i = 1; i < poz; ++i)
        {
            if(!arb->nextSibling) // daca nu se ajunge la pozitia dorita
            {
                free(id_copy);
                free(ant_token);
                return 0;
            }
            arb = arb->nextSibling;
        }
        if(!strcmp(arb->info->type, ancestor_name)) // s-a gasit stramosul
        {
            free(id_copy);
            free(ant_token);
            return 1;
        }
        strcpy(ant_token, token);
        token = strtok(NULL, ".");
    }
    free(id_copy);
    free(ant_token);
    return 0;
}

void fixID(TArb arb) /* functie care repara ID-ul nodurilor dupa stergerile
recursive */
{
    if(arb->info->id == NULL && arb->firstChild != NULL)
    {
        TArb child = arb->firstChild;
        strcpy(child->info->id, "1");
        fixID(child);
        int poz = 2;
        while(child->nextSibling)
        {
            char *num = itoa(poz);
            if(!num)
                return;
            strcpy(child->nextSibling->info->id, num);
            free(num);
            fixID(child->nextSibling);
            child = child->nextSibling;
            poz++;
        }
    }
    else if(arb->firstChild != NULL)
    {
        TArb child = arb->firstChild;
        strcpy(child->info->id, arb->info->id);
        strcat(child->info->id, ".1");
        fixID(child);
        int poz = 2;
        while(child->nextSibling)
        {
            strcpy(child->nextSibling->info->id, arb->info->id);
            strcat(child->nextSibling->info->id, ".");
            char *num = itoa(poz);
            if(!num)
                return;
            strcat(child->nextSibling->info->id, num);
            free(num);
            fixID(child->nextSibling);
            child = child->nextSibling;
            poz++;
        }
    }
}

void findNode_ID(TArb arb, FILE *out, int task, char *id, char *style)
/* cauta un arbore dupa id si in functie de valoarea lui task (1 pentru
stergere recursiva, 2 pentru override si 3 pentru append) apeleaza una din
functiile definite mai sus. */
{
    TArb ant = NULL, origin = arb;
    char *id_copy = calloc(strlen(id) + 1, sizeof(char));
    if(!id_copy)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    strcpy(id_copy, id);
    char *id_token = strtok(id_copy, "."); /* se face o parcurgere optima dupa
    id */
    while(id_token)
    {
        int poz = atoi(id_token);
        if(!arb->firstChild) // daca nu se ajunge la pozitia dorita
        {
            if(task == 1)
                fprintf(out,
                "Delete recursively failed: no node found for selector #%s!\n",
                id);
            else if(task == 2)
                fprintf(out,
                "Override style failed: no node found for selector #%s\n",
                id);
            else
                fprintf(out,
                "Append to style failed: no node found for selector #%s!\n",
                id);
            free(id_copy);
            return;
        }
        ant = arb;
        arb = arb->firstChild;
        int j;
        for(j = 0; j < poz - 1; ++j)
        {
            if(!arb->nextSibling) // daca nu se ajunge la pozitia dorita
            {
                if(task == 1)
                fprintf(out,
                "Delete recursively failed: no node found for selector #%s!\n",
                id);
                else if(task == 2)
                    fprintf(out,
                    "Override style failed: no node found for selector #%s\n",
                    id);
                else
                    fprintf(out,
                    "Append to style failed: no node found for selector #%s!\n"
                    , id);
            free(id_copy);
            return;
            }
            ant = arb;
            arb = arb->nextSibling; 
        }
        id_token = strtok(NULL, ".");
    }
    free(id_copy);
    if(task == 1 && style == NULL)
    {
        if(ant->firstChild == arb)
            ant->firstChild = arb->nextSibling;
        else
            ant->nextSibling = arb->nextSibling;
        End(&arb);
        fixID(origin);
    }
    else if(task == 2)
        overrideNode(&arb, style);
    else
        appendNode(&arb, style);
}

void findNode_elem_ancestor(TArb arb, FILE *out, int task, char *ancestor_name,
char *tag_name, char *style)
/* cauta un arbore dupa nume si de numele stramosului sau; in functie de
valoarea lui task (1 pentru stergere recursiva, 2 pentru override si 3 pentru
append) apeleaza una din functiile definite mai sus. */
{
    int done = 0, deleted = 0;
    void *coada = InitQ(sizeof(TArb));
    if(!coada) {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    void *aux = malloc(sizeof(TArb));
    if(!aux) {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *copy_style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!copy_style) {
        free(aux);
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    if(style)
        strcpy(copy_style, style);
    IntrQ(coada, &arb); // parcurgere in latime cu o coada
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        TArb child = (*((TArb *) aux))->firstChild, ant = NULL;
        while(child)
        {
            deleted = 0;
            if(!strcmp(child->info->type, tag_name) &&
            isAncestor(arb, ancestor_name, child->info->id))
            {
                if(task == 1 && style == NULL)
                {
                    if(ant) // reface legaturi
                        ant->nextSibling = child->nextSibling;
                    else
                        (*((TArb *) aux))->firstChild = child->nextSibling;
                    End(&child);
                    deleted = 1; // daca am sters iterativ in aceasta iteratie
                    if(ant)
                        child = ant->nextSibling;
                    else
                        child = (*((TArb *) aux))->firstChild;
                    fixID(arb); /* reparam de fiecare data ID-urile pt a cauta
                    cu functia isAncestor */
                    done = 1; // s-a facut o operatie
                }
                else if(task == 2)
                {
                    overrideNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1; // s-a facut o operatie
                }
                else
                {
                    appendNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1; // s-a facut o operatie
                }
            }
            if(child != NULL && deleted == 0)
            {
                ant = child;
                child = child->nextSibling;
            }
        }
        TArb sibling = (*((TArb *) aux))->firstChild;
        while(sibling)
        {
            IntrQ(coada, &sibling);
            sibling = sibling->nextSibling;
        }
    }
    if(done == 0) // verificam daca s-a gasit macar un nod
    {
        if(task == 1)
            fprintf(out,
            "Delete recursively failed: no node found for selector %s %s!\n",
            ancestor_name, tag_name);
        else if(task == 2)
            fprintf(out,
            "Override style failed: no node found for selector %s %s!\n",
            ancestor_name, tag_name);
        else
            fprintf(out,
            "Append to style failed: no node found for selector %s %s!\n",
            ancestor_name, tag_name);
    }
    DistrQ(&coada);
    free(aux);
    free(copy_style);
}

void findNode_elem_parent(TArb arb, FILE *out, int task, char *parent_name,
char *tag_name, char *style)
/* cauta un arbore dupa nume si de numele parintelui sau; in functie de
valoarea lui task (1 pentru stergere recursiva, 2 pentru override si 3 pentru
append) apeleaza una din functiile definite mai sus. */
// asemanator functiei findNode_elem_ancestor
{
    int done = 0, deleted = 0;
    void *coada = InitQ(sizeof(TArb));
    if(!coada) {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    void *aux = malloc(sizeof(TArb));
    if(!aux) {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *copy_style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!copy_style) {
        free(aux);
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    if(style)
        strcpy(copy_style, style);
    IntrQ(coada, &arb);
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        TArb child = (*((TArb *) aux))->firstChild, ant = NULL;
        while(child)
        {
            deleted = 0;
            if(!strcmp(child->info->type, tag_name) &&
            !strcmp((*((TArb *) aux))->info->type, parent_name))
            {
                if(task == 1 && style == NULL)
                {
                    if(ant)
                        ant->nextSibling = child->nextSibling;
                    else
                        (*((TArb *) aux))->firstChild = child->nextSibling;
                    End(&child);
                    deleted = 1;
                    if(ant)
                        child = ant->nextSibling;
                    else
                        child = (*((TArb *) aux))->firstChild;
                    done = 1;
                }
                else if(task == 2)
                {
                    overrideNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1;
                }
                else
                {
                    appendNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1;
                }
            }
            if(child != NULL && deleted == 0)
            {
                ant = child;
                child = child->nextSibling;
            }
        }
        TArb sibling = (*((TArb *) aux))->firstChild;
        while(sibling)
        {
            IntrQ(coada, &sibling);
            sibling = sibling->nextSibling;
        }
    }
    if(done == 0)
    {
        if(task == 1)
            fprintf(out,
            "Delete recursively failed: no node found for selector %s>%s!\n",
            parent_name, tag_name);
        else if(task == 2)
            fprintf(out,
            "Override style failed: no node found for selector %s>%s!\n",
            parent_name, tag_name);
        else
            fprintf(out,
            "Append to style failed: no node found for selector %s>%s!\n",
            parent_name, tag_name);
    }
    DistrQ(&coada);
    free(aux);
    free(copy_style);
    if(task == 1)
        fixID(arb);
}

void findNode_elem_class(TArb arb, FILE *out, int task, char *tag_name,
char *class_name, char *style)
/* cauta un arbore dupa nume si de numele clasei lui sau; in functie de
valoarea lui task (1 pentru stergere recursiva, 2 pentru override si 3 pentru
append) apeleaza una din functiile definite mai sus. */
// asemanator functiei findNode_elem_ancestor
{
    int done = 0, deleted = 0;
    void *coada = InitQ(sizeof(TArb));
    if(!coada) {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    void *aux = malloc(sizeof(TArb));
    if(!aux) {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *copy_style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!copy_style) {
        free(aux);
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    if(style)
        strcpy(copy_style, style);
    IntrQ(coada, &arb);
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        TArb child = (*((TArb *) aux))->firstChild, ant = NULL;
        while(child)
        {
            deleted = 0;
            if(!strcmp(child->info->type, tag_name))
            {
                TAttr atr = child->info->otherAttributes;
                while(atr)
                {
                    if((!strcmp(atr->name, "class")) &&
                    (!strcmp(atr->value, class_name)))
                    {
                        if(task == 1 && style == NULL)
                        {
                            if(ant)
                                ant->nextSibling = child->nextSibling;
                            else
                                (*((TArb *) aux))->firstChild =
                                child->nextSibling;
                            End(&child);
                            deleted = 1;
                            if(ant)
                                child = ant->nextSibling;
                            else
                                child = (*((TArb *) aux))->firstChild;
                            done = 1;
                        }
                        else if(task == 2)
                        {
                            overrideNode(&child, copy_style);
                            strcpy(copy_style, style);
                            done = 1;
                        }
                        else
                        {
                            appendNode(&child, copy_style);
                            strcpy(copy_style, style);
                            done = 1;
                        }
                        break;
                    }
                    atr = atr->next;
                }
            }
            if(child != NULL && deleted == 0)
            {
                ant = child;
                child = child->nextSibling;
            }
        }
        TArb sibling = (*((TArb *) aux))->firstChild;
        while(sibling)
        {
            IntrQ(coada, &sibling);
            sibling = sibling->nextSibling;
        }
    }
    if(done == 0)
    {
        if(task == 1)
            fprintf(out,
            "Delete recursively failed: no node found for selector %s.%s!\n",
            tag_name, class_name);
        else if(task == 2)
            fprintf(out,
            "Override style failed: no node found for selector %s.%s!\n",
            tag_name, class_name);
        else
            fprintf(out,
            "Append to style failed: no node found for selector %s.%s!\n",
            tag_name, class_name);
    }
    DistrQ(&coada);
    free(aux);
    free(copy_style);
    if(task == 1)
        fixID(arb);
}

void findNode_elem(TArb arb, FILE *out, int task, char *name, char *style)
/* cauta un arbore dupa nume si, in functie de valoarea lui task (1 pentru
stergere recursiva, 2 pentru override si 3 pentru append) apeleaza una din
functiile definite mai sus. */
// asemanator functiei findNode_elem_ancestor
{
    int done = 0, deleted = 0;
    void *coada = InitQ(sizeof(TArb));
    if(!coada) {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    void *aux = malloc(sizeof(TArb));
    if(!aux) {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *copy_style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!copy_style) {
        free(aux);
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    if(style)
        strcpy(copy_style, style);
    IntrQ(coada, &arb);
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        TArb child = (*((TArb *) aux))->firstChild, ant = NULL;
        while(child)
        {
            deleted = 0;
            if(!strcmp(child->info->type, name))
            {
                if(task == 1 && style == NULL)
                {
                    if(ant)
                        ant->nextSibling = child->nextSibling;
                    else
                        (*((TArb *) aux))->firstChild = child->nextSibling;
                    End(&child);
                    deleted = 1;
                    if(ant)
                        child = ant->nextSibling;
                    else
                        child = (*((TArb *) aux))->firstChild;
                    done = 1;
                }
                else if(task == 2)
                {
                    overrideNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1;
                }
                else
                {
                    appendNode(&child, copy_style);
                    strcpy(copy_style, style);
                    done = 1;
                }
            }
            if(child != NULL && deleted == 0)
            {
                ant = child;
                child = child->nextSibling;
            }
        }
        TArb sibling = (*((TArb *) aux))->firstChild;
        while(sibling)
        {
            IntrQ(coada, &sibling);
            sibling = sibling->nextSibling;
        }
    }
    if(done == 0)
    {
        if(task == 1)
            fprintf(out,
            "Delete recursively failed: no node found for selector %s!\n",
            name);
        else if(task == 2)
            fprintf(out,
            "Override style failed: no node found for selector %s!\n",
            name);
        else
            fprintf(out,
            "Append to style failed: no node found for selector %s!\n",
            name);
    }
    DistrQ(&coada);
    free(aux);
    free(copy_style);
    if(task == 1)
        fixID(arb);
}

void findNode_class(TArb arb, FILE* out, int task, char *class_name,
char *style)
/* cauta un arbore dupa numele clasei si, in functie de valoarea lui task (1
pentru stergere recursiva, 2 pentru override si 3 pentru append) apeleaza una
din functiile definite mai sus. */
// asemanator functiei findNode_elem_ancestor
{
    int done = 0, deleted = 0;
    void *coada = InitQ(sizeof(TArb));
    if(!coada) {
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    void *aux = malloc(sizeof(TArb));
    if(!aux) {
        DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    char *copy_style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!copy_style) {
        free(aux); DistrQ(&coada);
        fprintf(stderr, "ERROR! Allocation!\n");
        return;
    }
    if(style)
        strcpy(copy_style, style);
    IntrQ(coada, &arb);
    while(!VIDAQ(coada))
    {
        ExtrQ(coada, aux);
        TArb child = (*((TArb *) aux))->firstChild, ant = NULL;
        while(child)
        {
            deleted = 0;
            TAttr atr = child->info->otherAttributes;
            while(atr)
            {
                if((!strcmp(atr->name, "class")) &&
                (!strcmp(atr->value, class_name))) {
                    if(task == 1 && style == NULL) {
                        if(ant)
                            ant->nextSibling = child->nextSibling;
                        else
                            (*((TArb *) aux))->firstChild = child->nextSibling;
                        End(&child);
                        deleted = 1;
                        if(ant)
                            child = ant->nextSibling;
                        else
                            child = (*((TArb *) aux))->firstChild;
                        done = 1;
                    }
                    else if(task == 2) {
                        overrideNode(&child, copy_style);
                        strcpy(copy_style, style);
                        done = 1;
                    }
                    else {
                        appendNode(&child, copy_style);
                        strcpy(copy_style, style);
                        done = 1;
                    }
                    break;
                }
                atr = atr->next;
            }
            if(child != NULL && deleted == 0)
            {
                ant = child;
                child = child->nextSibling;
            }
        }
        TArb sibling = (*((TArb *) aux))->firstChild;
        while(sibling)
        {
            IntrQ(coada, &sibling);
            sibling = sibling->nextSibling;
        }
    }
    if(done == 0)
    {
        if(task == 1)
            fprintf(out,
            "Delete recursively failed: no node found for selector .%s!\n",
            class_name);
        else if(task == 2)
            fprintf(out,
            "Override style failed: no node found for selector .%s!\n",
            class_name);
        else
            fprintf(out,
            "Append to style failed: no node found for selector .%s!\n",
            class_name);
    }
    DistrQ(&coada);
    free(aux);
    free(copy_style);
    if(task == 1)
        fixID(arb);
}

void findNode(TArb arb, char *buffer, FILE *out) /* functie apelata pentru
task-urile deleteRecursevely, overrideStyle si appendStyle. Verifica ce tip
de comanda este si retine in task, apoi obtine selectorul si apeleaza functia
specifica de gasire a nodurilor care respecta cerinta */
{
    char *token = strtok(buffer, "=\"");
    int task;
    if(token[0] == 'd') // adica delete
        task = 1;
    else if(token[0] == 'o') // adica override
        task = 2;
    else
        task = 3; // adica append
    token = strtok(NULL, "=\"");
    char *selector_name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
    if(!selector_name) { // numele selectorului
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    strcpy(selector_name, token);
    char *style = NULL; // va fi NULL daca task-ul este 1
    if(task != 1) { // inseamna ca exista style
        token = strtok(NULL, "=\"");
        token = strtok(NULL, "=\"");
        style = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!style) {
            free(selector_name);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(style, token);
    }
    if(selector_name[0] == '.') // .class_name adica dupa clasa
        findNode_class(arb, out, task, selector_name + 1, style);
    else if (selector_name[0] == '#') // #id adica dupa id
        findNode_ID(arb, out, task, selector_name + 1, style);   
    else {
        unsigned int i;
        for(i = 0; i < strlen(selector_name); ++i) {
            if(selector_name[i] == '.') { /* tag_name.class_name adica dupa
            nume si clasa */
                char *tag_name = calloc(MAX_LENGTH_TAG_ELEMENTS,
                sizeof(char));
                if(!tag_name) {
                    free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                char *class_name = malloc(MAX_LENGTH_TAG_ELEMENTS *
                sizeof(char));
                if(!class_name) {
                    free(tag_name); free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                strncpy(tag_name, selector_name, i);
                strcpy(class_name, selector_name + i + 1);
                findNode_elem_class(arb, out, task, tag_name, class_name,
                style);
                free(tag_name); free(class_name); break;
            }
            if(selector_name[i] == '>') { /* parent_name>tag_name adica dupa
            numele parintelui si al copilului */
                char *parent_name = calloc(MAX_LENGTH_TAG_ELEMENTS,
                sizeof(char));
                if(!parent_name) {
                    free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                char *tag_name = calloc(MAX_LENGTH_TAG_ELEMENTS,
                sizeof(char));
                if(!tag_name) {
                    free(parent_name); free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                strncpy(parent_name, selector_name, i);
                strcpy(tag_name, selector_name + i + 1);
                findNode_elem_parent(arb, out, task, parent_name, tag_name,
                style);
                free(parent_name); free(tag_name); break;
            }
            if(selector_name[i] == 32) { /* ancestor_name tag_name adica dupa
            numele stramosului si al copilului */
                char *ancestor_name = calloc(MAX_LENGTH_TAG_ELEMENTS,
                sizeof(char));
                if(!ancestor_name) {
                    free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                char *tag_name = malloc(MAX_LENGTH_TAG_ELEMENTS *
                sizeof(char));
                if(!tag_name) {
                    free(ancestor_name); free(style); free(selector_name);
                    fprintf(stderr, "ERROR! Allocation\n");
                    return;
                }
                strncpy(ancestor_name, selector_name, i);
                strcpy(tag_name, selector_name + i + 1);
                findNode_elem_ancestor(arb, out, task, ancestor_name, tag_name,
                style);
                free(ancestor_name); free(tag_name); break;
            }
        }
        if(i == strlen(selector_name)) // tag_name adica dupa numele tag-ului
            findNode_elem(arb, out, task, selector_name, style);
    }
    free(selector_name);
    if(style)
        free(style);
}