/* BOLDIȘOR Dragoș-Alexandru - 312CB */

#include "interpret.h"

char *itoa(int x) // functie de transformare int in char*
{
    int aux = x, no = 0;
    x = 0;
    while(aux)
    {
        x *= 10;
        x += aux % 10;
        no++;
        aux /= 10;
    }
    char *number = calloc(no + 1, sizeof(char));
    if(!number)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return NULL;
    }
    while(x)
    {
        char c = (x % 10) + '0'; 
        strncat(number, &c, 1);
        x /= 10;
    }
    return number;
}

TArb TagNode(void *st_parinti) /* creaza tag nou si il leaga in arbore prin
intermediul unei stive de stramosi (varful fiind parintele noului nod) */
{
    TArb arb = (TArb)malloc(sizeof(TNodArb));
    if(!arb)
        return NULL;
    arb->firstChild = NULL;
    arb->nextSibling = NULL;
    arb->info = (TInfo)malloc(sizeof(TNodInfo));
    if(!arb->info)
    {
        free(arb);
        return NULL;
    }
    arb->info->contents = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!arb->info->contents)
    {
        free(arb->info);
        free(arb);
        return NULL;
    }
    arb->info->type = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!arb->info->type)
    {
        free(arb->info->contents);
        free(arb->info);
        free(arb);
        return NULL;
    }
    arb->info->isSelfClosing = 0;
    arb->info->otherAttributes = NULL;
    arb->info->style = NULL;
    void *parinte = malloc(sizeof(TArb));
    if(!parinte)
    {
        free(arb->info->type); free(arb->info->contents); free(arb->info);
        free(arb);
        fprintf(stderr, "ERROR! Allocation\n");
        return NULL;
    }
    if(VIDAS(st_parinti))
    {
        free(parinte);
        arb->info->id = NULL;
        return arb;
    }
    arb->info->id = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!arb->info->id)
    {
        free(parinte); free(arb->info->type); free(arb->info->contents);
        free(arb->info); free(arb);
        fprintf(stderr, "ERROR! Allocation\n");
        return NULL;
    }
    Pop(st_parinti, parinte); // cautare loc in lista de copii
    int poz, i;
    if( (*((TArb *) parinte))->firstChild == NULL )
    {
        (*((TArb *) parinte))->firstChild = arb; // legare copil de parinte
        poz = 1;
    }
    else
    {
        TArb aux;
        for(i = 0, aux = (*((TArb *) parinte))->firstChild;
        aux->nextSibling != NULL; ++i, aux = aux->nextSibling)
        { }
        aux->nextSibling = arb; // legare copil de ultimul frate
        poz = i + 2;
    }
    if( (*((TArb *) parinte))->info->id == NULL)
    {
        char *num = itoa(poz); // creare id pt copiii radacinii arborelui
        if(!num)
        {            
            free(parinte); free(arb->info->type); free(arb->info->contents);
            free(arb->info); free(arb);
            return NULL;
        }
        strcpy(arb->info->id, num);
        free(num);
    }
    else
    {
        strcpy(arb->info->id, (*((TArb *) parinte))->info->id );
        strncat(arb->info->id, ".", 1);
        char *num = itoa(poz); // creare id pt copiii unui nod
        if(!num)
        {            
            free(parinte); free(arb->info->type); free(arb->info->contents);
            free(arb->info); free(arb);
            return NULL;
        }
        strcat(arb->info->id, num);
        free(num);
    }
    Push(st_parinti, parinte); // punem parintele inapoi in stiva
    free(parinte);
    return arb;
}

void endTag(TArb *a, void *st_parinti) /* functie apelata in momentul in care
se incheie informatia dintr-un tag. Se va repara contents astfel incat sa nu
contina spatii la final si (*a) se face NULL, iar in cazul in care (*a) este
deja NULL (adica se iese din 2 sau mai multe tag-uri consecutiv), scoatem
primul parinte din stiva de stramosi. In momentul in care stiva este vida,
inseamna ca am ajuns la final. */
{
    if((*a))
    {
        if((*a)->info->contents[0] != '\0')
            if((*a)->info->contents[strlen((*a)->info->contents) - 1] == ' ')
                (*a)->info->contents[strlen((*a)->info->contents) - 1] = '\0';
    }
    if(VIDAS(st_parinti))
        return;
    void *aux = malloc(sizeof(TArb));
    if(!aux)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    if((*a) == NULL)
        Pop(st_parinti, aux);
    else if((*a)->info->isSelfClosing)
    {
        (*a) = NULL;
        Pop(st_parinti, aux);
    }
    else
        (*a) = NULL;
    if(VIDAS(st_parinti))
        *a = *((TArb *) aux);
    free(aux);
}

void insertAttr(TArb *a, char *name, char *value) /* insereaza un nou atribut
in (*a) (name este numele si value, valoarea). */
{
    TAttr curr = (*a)->info->otherAttributes;
    if(!curr) // daca este primul atribut
    {
        (*a)->info->otherAttributes = malloc(sizeof(TNodAttr));
        if(!(*a)->info->otherAttributes)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr = (*a)->info->otherAttributes;
        curr->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->name)
        {
            free((*a)->info->otherAttributes);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->value)
        {
            free(curr->name);
            free((*a)->info->otherAttributes);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(curr->name, name);
        strcpy(curr->value, value);
        curr->next = NULL;
    }
    else // daca mai exista atribute, il cautam pe ultimul
    {
        while(curr->next)
            curr = curr->next;
        curr->next = malloc(sizeof(TNodAttr));
        if(!curr->next)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->next->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->next->name)
        {
            free(curr->next);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->next->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->value)
        {
            free(curr->next->name);
            free(curr->next);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(curr->next->name, name);
        strcpy(curr->next->value, value);
        curr->next->next = NULL;   
    }
    name[0] = '\0'; value[0] = '\0';
}

void insertStyle(TArb *a, char *name, char *value) /* insereaza un nou atribut
de tip style in (*a) (name este numele si value, valoarea). */
{
    TAttr curr = (*a)->info->style;
    if(!curr) // daca este primul atribut
    {
        (*a)->info->style = malloc(sizeof(TNodAttr));
        if(!(*a)->info->style)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr = (*a)->info->style;
        curr->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->name)
        {
            free((*a)->info->style);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->value)
        {
            free(curr->name);
            free((*a)->info->style);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(curr->name, name);
        strcpy(curr->value, value);
        curr->next = NULL;
    }
    else // daca mai exista atribute, il cautam pe ultimul
    {
        while(curr->next)
            curr = curr->next;
        curr->next = malloc(sizeof(TNodAttr));
        if(!curr->next)
        {
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->next->name = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->next->name)
        {
            free(curr->next);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        curr->next->value = malloc(MAX_LENGTH_TAG_ELEMENTS * sizeof(char));
        if(!curr->value)
        {
            free(curr->next->name);
            free(curr->next);
            fprintf(stderr, "ERROR! Allocation\n");
            return;
        }
        strcpy(curr->next->name, name);
        strcpy(curr->next->value, value);
        curr->next->next = NULL;   
    }
}

void createStyle(TArb *a, char *name, char *value) /* face modificarile
necesare pentru a apela functia insertStyle. La inceput, name va fi style si
value va retine totalitatea atributelor de tip style care urmeaza sa fie
inserate. */
{
    char *buffer = calloc(MAX_LENGTH_TAG_ELEMENTS, sizeof(char));
    if(!buffer)
    {
        fprintf(stderr, "ERROR! Allocation\n");
        return;
    }
    strcpy(buffer, value); // salvare in buffer
    name[0] = '\0'; value[0] = '\0';
    unsigned int i;
    int state = 0; // adica este nume
    for(i = 0; i < strlen(buffer); ++i)
        if(!isspace(buffer[i]))
        {
            if(buffer[i] == ':')
                state = 1; // adica este valoare
            else if(buffer[i] == ';')
            {
                state = 0; // urmeaza un nume
                insertStyle(a, name, value); // inserare
                name[0] = '\0'; value[0] = '\0';
            }
            else
            {
                if(state == 0) // facem verificarea
                    strncat(name, &buffer[i], 1);
                else
                    strncat(value, &buffer[i], 1);
            }
        }
    free(buffer);
}

TParseState Interpret(TParseState currentState, char c, TArb *a,
void *st_parinti, char *name, char *value) /* prezinta argumentele initiale
(currentState - statusul curent in care se afla "automatul", c - caracterul
care urmeaza sa fie verificat, precum si arborele *a). De asemenea, am introdus
si alte argumente (stiva de stramosi, precum si stringurile name si value in
care se retin temporar informatiile unui atribut). */
{
    TParseState nextState;
    switch (currentState){
    case PARSE_CONTENTS:
        if (c == '<')
            nextState = PARSE_OPENING_BRACKET;
        else {
                nextState = PARSE_CONTENTS;
                if(c != '\n' && c != 13 && (*a) != NULL){ // caracter valid
                    if((*a)->info->contents[0] == '\0' && c == ' ')
                        break; //nu punem spatii inainte sa avem alte caractere
                    if((*a)->info->contents[0] != '\0')
                        if((*a)->info->contents
                        [strlen((*a)->info->contents) - 1] == ' ' && c == ' ')
                            break; // nu punem mai mult de un spatiu consecutiv
                    strncat((*a)->info->contents, &c, 1);
                }
        }
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c))
            nextState = PARSE_OPENING_BRACKET;
        else if (c == '>')
            nextState = PARSE_ERROR;
        else if (c == '/')
            nextState = PARSE_CLOSING_TAG;
        else {
                nextState = PARSE_TAG_TYPE;
                if((*a) != NULL && (*a)->info->isSelfClosing == 0)
                    Push(st_parinti, a); // introducem in stiva de stramosi
                *a = TagNode(st_parinti); // creem un nou nod
                if(!(*a)){
                    fprintf(stderr,"ERROR! Allocation\n");
                    return PARSE_ERROR;
                }
                strncat((*a)->info->type, &c, 1); /* introducem primul caracter
                in type */
        }
        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))
            nextState = PARSE_REST_OF_TAG;
        else if (c == '>')
            nextState = PARSE_CONTENTS;
        else {
            nextState = PARSE_TAG_TYPE;
            strncat((*a)->info->type, &c, 1); // introducere in type
        }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>') {
            nextState = PARSE_CONTENTS;
            endTag(a, st_parinti);
        }
        else
            nextState = PARSE_CLOSING_TAG;
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))
            nextState = PARSE_REST_OF_TAG;
        else if (c == '>')
            nextState = PARSE_CONTENTS;
        else if (c == '/')
            nextState = PARSE_SELF_CLOSING;
        else{
            nextState = PARSE_ATTRIBUTE_NAME;
            strncat(name, &c, 1); // introducere in nume atribut temporar
        }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')
            nextState = PARSE_ATTRIBUTE_EQ;
        else {
            nextState = PARSE_ATTRIBUTE_NAME;
            strncat(name, &c, 1); // introducere in nume atribut temporar
        }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')
            nextState = PARSE_ATTRIBUTE_VALUE;
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"') {
            nextState = PARSE_REST_OF_TAG;
            if(!strcmp(name, "style"))
                createStyle(a, name, value); // creare atribute de tip style
            else
                insertAttr(a, name, value); // creare atribute (otherAtributes)
        }
        else {
            nextState = PARSE_ATTRIBUTE_VALUE;
            strncat(value, &c, 1); // introducere in value atribut temporar
        }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>') {
            nextState = PARSE_CONTENTS;
            (*a)->info->isSelfClosing = 1; // tag-ul este self-closing
        }
        else
            nextState = PARSE_ERROR;
        break;
    default:
        break;
    }
    return nextState;
}