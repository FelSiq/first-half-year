#include <stdlib.h>
#include <stdio.h>

#define ENDOFLINE -1

/*Esta versao, diferente da anterior, inclui ponteiros duplos*/

int* getElements(int n)
    {

    //Esta funcao obtem os 'n' termos que serao armazenados.

    int *elem = NULL;
    int i, counter = 0, aux = 0;

    for(i = 0; i < n; i++)
        {
        scanf("%d", &aux);
        elem = (int *) realloc(elem, sizeof(int)*(counter + 1));
        elem[counter++] = aux;
        }

    return elem;
    }

int* getLine(int* elem, int k, int i, int n)
    {
    int counter = 0, aux, r = 0;
    int *auxVector = NULL; //Este e um vetor auxiliar que ira segurar todos os elementos pertencentes a 'linha' atual.

    auxVector = (int *) calloc(1, sizeof(int)); //Esta alocacao inicial impede um Segmentation fault, caso nenhum elemento esteja contido na linha atual.

    while (counter < n)
        {
        aux = (elem[counter] % k);
        if (aux == i) //Aqui e verificado se o elemento atual pertence a 'linha' atual.
            {
            auxVector[r++] = elem[counter];
            auxVector = (int *) realloc(auxVector, sizeof(int)*(r + 1));
            }

        counter++;
        }

    auxVector[r] = ENDOFLINE;

    return auxVector;
    }

void createTable(int k, int n, int *elem)
    {
    int i, p = 0;
    int **line = NULL;

    line = (int **) malloc(sizeof(int *)*(1 + k));

    for(i = 0; i < k; i++)//Este e o laco externo, usado para gerar a tabela, linha por linha.
        {
        printf("%d:", i); //Aqui e gerado o indice da linha atual, de 0 ate a k-esima.
        line[i] = getLine(elem, k, i, n); //Esta e a funcao complementar, que apresenta o laco interno para gerar a tabela.

        while (line[i][p] != ENDOFLINE) printf(" %d", line[i][p++]); //Imprime todos os valores relacionados a linha atual.

        p = 0; //O contador de 'colunas' e resetado para a proxima linha.

        printf("\n");
        }

    for(i = 0; i < k; i++) free(line[i]); //Aqui cada 'linha' e liberada separadamente.


    free(line);

    }

int main ()
    {
    int k, n;
    int *elem = NULL; //Este sera o ponteiro responsavel pelo endereco dos

    scanf("%d%d", &k, &n); //'k' e a chave, 'n' o numero de inteiros que serao armazenados

    elem = getElements(n); //Esta funcao obtem todos os 'n' termos inteiros, e retorna um endereco onde estao armazenados
    createTable(k, n, elem); //Esta funcao e usada em conjunto com a funcao getLine para criar a tabela.


    free(elem);

    return 0;
    }
