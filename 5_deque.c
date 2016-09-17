#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ENTER 10

typedef enum{
    FALSE,
    TRUE
} boolean;

typedef struct Node{
    struct Node *prev, *next;
    char element;
} Node;

typedef struct Deque{
    Node *first, *last;
} Deque;

Deque *create(){
    Deque *d = malloc(sizeof(Deque));
    d->first = NULL;
    d->last = NULL;
    return d;
}

void clear(Deque *d){
    Node *aux = d->first;
    while(aux != NULL){
        if(d->first != d->last){
            aux = d->first->next;
            free(d->first);
            d->first = aux;
        }else{
            free(d->first);
            aux = NULL;
            d->first = NULL;
            d->last = NULL;
        }
    }
}

void destroy(Deque *d){
    clear(d);
    free(d);
}

int empty(Deque *d){
    if (d->first == NULL && d->last == NULL) return 1;
    /*Checar ambos os ponteiros acima e desnecessario, mas optei por faze-lo,
    pois engessa melhor o codigo e pode barrar possiveis erros.*/
    return 0;
}

int size(Deque *d){
    int size = 0;
    Node *aux = d->first;
    while(aux != NULL) {
        size++;
        aux = aux->next;
    }
    return size;
}

void push_front(Deque *d, char c){
    Node *newNode = malloc(sizeof(Node));

    newNode->prev = NULL;
    newNode->element = c;
    newNode->next = d->first;

    if (!empty(d)) d->first->prev = newNode;
    else d->last = newNode;

    d->first = newNode;
}

void push_back(Deque *d, char c){
    Node *newNode = malloc(sizeof(Node));

    newNode->next = NULL;
    newNode->element = c;
    newNode->prev = d->last;

    if (!empty(d)) d->last->next = newNode;
    else d->first = newNode;

    d->last = newNode;
}

void pop_front(Deque *d){
    Node *aux;
    if (!empty(d)){
        if (size(d) > 1){
            aux = d->first->next;
            d->first->next->prev = NULL;
            free(d->first);
            d->first = aux;
        }else clear(d);
    }
}

void pop_back(Deque *d){
    Node *aux;
    if (!empty(d)){
        if (size(d) > 1){
            aux = d->last->prev;
            d->last->prev->next = NULL;
            free(d->last);
            d->last = aux;
        }else clear(d);
    }
}

/*
A funcao de impressao comentada abaixo esta funcional, mas nao
atende a exigencia de uma impressao recursiva.

void print(Deque *d){
    int n = size(d);
    Node *aux = d->first;

    for(int i = 0; i < n; i++){
        printf("%c", aux->element);
        aux = aux->next;
    }
    printf("\n");
}*/

/*
Esta funcao comentada e outra forma de impressao recursiva que encontrei e que faz melhor uso das
funcoes do deque, porem apresenta uma performance ruim em relacao ao tempo de execucao, entao optei
por um caso mais simples, exibido imediatamente abaixo do comentario.

void print(Deque *d){
    char c;
    if(!empty(d)){
        c = front(d);
        printf("%c", c);
        pop_front(d, c);
        print(Deque *d);
        push_front(d, c);
    }
}

*/

//As duas funcoes abaixo em conjunto printam recursivamente o conteudo do deque.
void recursivePrint(Node *n){
    if(n != NULL){
        printf("%c", n->element);
        recursivePrint(n->next);
    }
}

void print(Deque *d){
    recursivePrint(d->first);
    printf("\n");
}

char front(Deque *d){
    if (d->first != NULL) return d->first->element;
    else return ' ';
}

char back(Deque *d){
    if (d->last != NULL) return d->last->element;
    else return ' ';
}

//Partindo daqui, todas as funcoes abaixo sao nao-obrigatorias.
//Esta funcao adicional completa um deque como se fosse uma string de caracteres.
void fill (Deque *d){
    char c = 0;
    while (c != ENTER){
        c = fgetc(stdin);
        if (c != ENTER)
            push_back(d, c);
    }
}

/*
A funcao comentada abaixo esta totalmente funcional, e utiliza de muitas das as funcoes do deque apresentadas acima.
Porem, no run.codes, a performance dela para os casos de teste 8, 9 e 10 sao piores do que o caso mais trivial,
que nao faz uso de um segundo deque, e que e apresentada logo abaixo em conjunto com a getString().

void getWinnerWord(Deque *d){
    Deque *s = create();
    int aux;
    char faux;

    fill(s);//Tal funcao preenche o novo deque s, semelhante a uma string de caracteres.
    aux = size(s); //Tal variavel auxiliar permite acessar a funcao size() menos vezes.

    for(int i = 0; i < aux; i++){
        faux = front(s); //Esta variavel auxiliar permite acessar menos vezes a funcao front() no loop.

        if(i == 0 || faux >= front(d)) push_front(d, faux);
        else push_back(d, faux);

        pop_front(s);
    }
    print(d);
    destroy(s);
}
*/

char *getString(int *counter){
    char *string = NULL, c = 0;

    while(c != ENTER){
        c = fgetc(stdin);
        string = (char *) realloc(string, sizeof(char) * (*counter + 1));
        string[(*counter)++] = c;
    }

    string[*counter - 1] = '\0';
    return string;
}

void getWinnerWord(Deque *d){
    int size = 0;
    char *string = getString(&size);

    for(int i = 0; i < size - 1; i++)
        if(i == 0 || string[i] >= front(d)) push_front(d, string[i]);
        else push_back(d, string[i]);

    print(d);
    free(string);
}

/*As duas funcoes abaixo resolvem o problema no. 2.
*/
boolean recursiveAnswer(Deque *d, Deque *s){
    char c;
    if (empty(s) && empty(d))
        return TRUE;

    if (front(d) == back(s)){
        c = front(d);
        pop_front(d);
        pop_back(s);

        if(recursiveAnswer(d, s))
            return TRUE;

        push_front(d, c);
        push_back(s, c);
    }

    if (back(d) == back(s)){
        c = back(d);
        pop_back(d);
        pop_back(s);

        if(recursiveAnswer(d, s))
            return TRUE;

        push_back(d, c);
        push_back(s, c);
    }

    return FALSE;
}

void computeAnswer(Deque *d){
    Deque *s = create();

    //Como ja dito, a funcao fill() preenche o deque semelhante a uma string de caracteres.
    fill(s);
    fill(d);

    if(recursiveAnswer(d, s)) printf("S\n");
    else printf("N\n");
    destroy(s);
}

int main(){
    Deque *d = create();
    int opt;

    scanf(" %d", &opt);
    getchar();

    switch(opt){
        case 1:
            getWinnerWord(d);
            break;
        case 2:
            computeAnswer(d);
            break;
    }

    destroy(d);
    return 0;
}
