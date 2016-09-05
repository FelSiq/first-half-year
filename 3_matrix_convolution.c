#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ENTER 10

char *getImageName(){
    //Esta funcao obtem o nome tanto do arquivo da imagem .PGM, quanto o nome do arquivo de mascara, se necessario.
    char *imgname = NULL;
    char c = 0;
    int counter = 0;

    while(c != ENTER){
        c = fgetc(stdin);
        imgname = (char *) realloc(imgname, sizeof(char)*(counter + 1));
        imgname[counter++] = c;
    }

    imgname[counter - 1] = '\0';

    return imgname;
}

float **matrixMemoryAlloc(int lin, int col){
    //Esta funcao generica realiza a alocacao Heap de uma matriz qualquer.
    float **matrix = malloc(sizeof(float *) * lin);
    int i;

    for(i = 0; i < lin; i++)
        matrix[i] = (float *) malloc(sizeof(float) * col);

    return matrix;
}

void matrixMemoryFree(float **matrix, int lin){
    //Esta funcao libera a memoria Heap alocada para os bits que compoe a imagem em si.
    int i;

    for(i = 0; i < lin; i++) free(matrix[i]);
    free(matrix);
}

float **getImageData(char *imgname, char *imgtype, int *imgwidth, int *imglenght, int *vmax){
    //Nesta funcao, todo o arquivo da imagem .PGM e lido e armazenado em diferentes secoes.
    float **imgdata;
    FILE *fimg;
    char aux = 0, *junk = NULL;
    int i, j;

    //Nas tres linhas seguintes, abre-se o arquivo da imagem.
    fimg = fopen(imgname, "r");
    free(imgname);//LIBERA-SE A MEM. HEAP DO NOME DE ARQUIVO DA IMAGEM.
    if (fimg == NULL) exit(0);

    //Nas tres linhas que seguem, obtem-se o 'Magic Number' da imagem.
    fread(imgtype, sizeof(char), 3, fimg);
    imgtype[2] = '\0';

    //Aqui verifica-se a presenca de possiveis comentarios na segunda linha da imagem.
    //Nota-se que tais comentarios iniciam-se com '#'.
    fscanf(fimg, "%c", &aux);
    if (aux == '\n' || aux == ' ') fscanf(fimg, "%c", &aux);//Trata-se de uma correcao para remover determinados comentarios.
    if (aux == '#') {
            junk = malloc(sizeof(char) * 75);   //Uma alternativa que arrumei para remover os comentarios da imagem e
            fgets(junk, 75, fimg);              //transferi-los para uma regiao de Heap temporaria e depois libera-la.
            free(junk);                         //Note que o tamanho maximo do comentario e 75 caracteres.
    }
    else fseek(fimg, 3, SEEK_SET);//Este corretor retorna a posicao correta de leitura, na ausencia de comentarios na imagem.

    //Agora obtemos, respectivamente, a altura (ref. as colunas da matriz),
    //a largura da imagem (ref. as linhas da matriz), e o Vmax, que representa o maior valor da escala cinza da imagem.
    fscanf(fimg, "%d", imglenght);
    fscanf(fimg, "%d", imgwidth);
    fscanf(fimg, "%d", vmax);

    //Finalmente, aloca-se mem. Heap e le-se a matriz da imagem.
    imgdata = matrixMemoryAlloc(*imgwidth, *imglenght);
    for (i = 0; i < *imgwidth; i++)
        for (j = 0; j < *imglenght; j++)
            fscanf(fimg, "%f", &imgdata[i][j]);

    fclose(fimg);
    return imgdata;
}

void printImageData(float **imgdata, char *imgtype, int vmax, int imgw, int imgl){
    //Nesta funcao e impresso o resultado final
    int i, j;

    printf("%s\n", imgtype); //Magic Number
    printf("%d %d\n%d\n", imgl, imgw, vmax); //Altura, Largura e Valor maximo de tom de cinza, respec.
    for(i = 0; i < imgw; i++){
        for(j = 0; j < imgl; j++)
            printf("%.0f ", imgdata[i][j]); //Matriz dos valores dos bits da imagem em si.
        printf("\n");
    }
}

void imageGetNegative(float **imgdata, int imgwidth, int imglenght, int vmax){
    //Esta funcao transforma a imagem lida em sua versao negativa.
    int i, j;

    for(i = 0; i < imgwidth; i++)
        for(j = 0; j < imglenght; j++)
            imgdata[i][j] = (vmax - imgdata[i][j]);
}

float **getFilterMask(char *maskname, int *order){
    //A funcao abaixo obtem os dados do arquivo de mascara de convolucao.
    float **mask;
    int i, j;
    FILE *fmask;

    fmask = fopen(maskname, "r");
    if (fmask == NULL) exit(1);

    fscanf(fmask, "%d", order);
    mask = matrixMemoryAlloc(*order, *order);

    for(i = 0; i < *order; i++)
        for(j = 0; j < *order; j++)
            fscanf(fmask, "%f", &mask[i][j]);

    fclose(fmask);
    return mask;
}

void matrixContentCopy(float **m1, float **m2, int lin, int col){
    /*Esta funcao generica foi criada para copiar os dados de uma matriz (m2) para uma outra matriz identica (m1).
    A usei para realizar a convolucao de matrizes, pois precisei de uma matriz suporte que guardasse os dados
    da imagem original.*/
    int i, j;

    for(i = 0; i < lin; i++)
        for(j = 0; j < col; j++)
            m1[i][j] = m2[i][j];
}

void imageGetSFilter(float **imgdata, char *maskname, int imgwidth, int imglength, int vmax){
    float **mask, **mcopy, result = 0;
    int gmax = 0, gmin = 0;
    int order, i, j, k, l, aux; //Ordem da matriz de mascara.

    mask = getFilterMask(maskname, &order);//Aqui e obtida a mascara para a convolucao de matrizes.
    free(maskname);//DESALOCA A MEM. HEAP DO NOME DO ARQUIVO DE MASCARA.

    mcopy = matrixMemoryAlloc(imgwidth, imglength);/*Tal matriz auxiliar sera necessaria para realizar a convolucao de matrizes,
    pois ela mantera os dados originais da imagem durante o processo, enquanto a matriz da imagem sera alterada elemento por elemento,
    o que interfere no processo de convolucao dos elementos seguintes.*/
    matrixContentCopy(mcopy, imgdata, imgwidth, imglength);//Nesta funcao, cria-se uma copia identica do conteudo da matriz da imagem para a matriz auxiliar.

    aux = order/2;//Tal variavel auxiliar foi criada apenas para simplificar as condicoes dos lacos seguintes.

    //Os dois primeiros lacos que se seguem servem para navegar pelos elementos da matriz da imagem.
    for(i = 0; i < imgwidth; i++){
        for(j = 0; j < imglength; j++){
            /*Os dois 'fors' seguintes realizam a convolucao das matrizes de mascara e auxiliar. Lembrando que a matriz
            auxiliar e uma copia identica da matriz da imagem, e nunca e alterada durante o processo.*/
            for(k = 0; k < order; k++)
                for(l = 0; l < order; l++)
                    if(i + aux >= k && j + aux >= l && i + aux - k < imgwidth && j + aux - l < imglength)
                        result += floor(mask[k][l] * mcopy[i + aux - k][j + aux - l]);

            /*No 'if-else' que segue, os valores gmin e gmax (que guardam, respectivamente, o menor e maior valor de g(x,y),
            sao obtidos e atualizados, ate os lacos terminem e os valores otimos sejam mantidos.*/
            if(i == 0 && j == 0){
                gmax = result;
                gmin = result;
            }else{
                if (gmax < result) gmax = result;
                if (gmin > result) gmin = result;
            }

            imgdata[i][j] = result;
            result = 0;//Reseta-se a variavel 'result' para o prox. elemento da matriz da imagem.
        }
    }

    //Os lacoes seguintes realizam a normalizacao dos elementos da matriz de imagem.
    //if(gmax > vmax) ??
        for(i = 0; i < imgwidth; i++)
            for(j = 0; j < imglength; j++)
                imgdata[i][j] = (vmax * ((imgdata[i][j] - gmin)/(gmax - gmin)));

    matrixMemoryFree(mask, order);//DESALOCA A MEM. HEAP DA MASCARA DE CONVOLUCAO.
    matrixMemoryFree(mcopy, imgwidth);//DESALOCA A MEM. HEAP DA MATRIZ SUPORTE.
}

int main(){
    int op, imgw = 0, imgl = 0, vmax = 0;
    char *filename, *imgtype = malloc(sizeof(char) * 3);
    float **imgdata;

    scanf(" %d", &op); //Aqui le-se a operacao a ser efetuada: Se 1, operar o Negativo. Se diferente de 1, operar o filtro especial.
    getchar();

    filename = getImageName(); //O nome da imagem e obtido aqui.
    imgdata = getImageData(filename, imgtype, &imgw, &imgl, &vmax);//Esta funcao obtem todos os dados dentro do arquivo da imagem, e os guarda dentro de diferentes secoes.

    if (op == 1) imageGetNegative(imgdata, imgw, imgl, vmax);
    else {
            //Poderia ter criado um 'if (op == 2)' para a opcao de filtro especial, mas achei desnecessario, pois este
            //programa opera com ou uma ou outra opcao, nunca nenhuma ou duas ou mais opcoes seguidas. Dai, o 'else' acima.
            filename = getImageName(); //O nome da mascara e obtido aqui.
            imageGetSFilter(imgdata, filename, imgw, imgl, vmax);//Tal funcao realiza todo o processo de filtragem especial com a convolucao de matrizes.
    }

    printImageData(imgdata, imgtype, vmax, imgw, imgl);//Imprime-se o resultado final, isto e, os valores da imagem pos-processada.

    free(imgtype);//Aqui o 'magic number' e liberado.
    matrixMemoryFree(imgdata, imgw);

    /*Nota: todos as outras regioes da memoria Heap alocadas para a execucao do programa,
    (nome da imagem, nome da mascara, matriz auxiliar de convolucao, etc) foram desalocadas
    anteriormente, logo que desnecessarias para operacoes futuras.*/

    return 0;
}
