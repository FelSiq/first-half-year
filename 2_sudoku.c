#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ENTER 10
#define ASCIIA 65
#define ASCII0 48

void matrixFreeChar(char **matrix, int row){
    int i;

    for(i = 0; i < row; i++) free(matrix[i]);
    free(matrix);

    return;
}

void matrixFreeInt(int **matrix, int row){
    int i;

    for(i = 0; i < row; i++) free(matrix[i]);
    free(matrix);

    return;
}

void cubeFree(char ***cube, int row){
    int i;

    for(i = 0; i < row; i++) matrixFreeChar(cube[i], row);
    free(cube);

    return;
}

char **allocMat(int c, int r){
    char **matrix;
    int i;

    matrix = (char **) malloc(sizeof(char *)*r);
    for(i = 0; i < r; i++) matrix[i] = (char *) calloc(c + 2, sizeof(char));

    return matrix;
}

char **readMat(char *path, int *c, int *r){
    char **matrix = NULL;
    FILE *fp = NULL;
    int i, j;

    fp = fopen(path, "rb");

    if (fp == NULL) exit(0);

    fseek(fp, 0, SEEK_END);
    i = ftell(fp);
    *c = sqrt(i);
    *r = sqrt(i);
    fseek(fp, 0, SEEK_SET);

    matrix = allocMat((*c), (*r));

    for(i = 0; i < (*r); i++)
        for(j = 0; j <= (*c); j++)
            fscanf(fp, "%c", &matrix[i][j]);

    /*ALTERNATIVA:
    for(i = 0; i < *r; i++)
        fread(matrix[i], sizeof(char), *c + 1, fp);*/

    matrix[i - 1][j - 1] = '\0';

    fclose(fp);
    return matrix;
}
/*Esta funcao contabiliza o numero de aparicoes de cada algarismo hex (0-F) na matriz root, e depois as organiza em ordem decrescente*/
void getAppareances(char **matrix, int **elemAppearance, int col, int row){
    int i, j, k, aux;

    for(i = 0; i < row; i++){
        elemAppearance[i][0] = 0;
        if(i <= 9) elemAppearance[i][1] = i + ASCII0;
        else elemAppearance[i][1] = i + ASCIIA - 10;
        elemAppearance[i][2] = i;
    }

    for(i = 0; i < row; i++)
        for(j = 0; j < col; j++)
            if(matrix[i][j] != '.'){
                if(matrix[i][j] <= ASCII0 + 9) elemAppearance[matrix[i][j] - ASCII0][0]++;
                else elemAppearance[matrix[i][j] - ASCIIA + 10][0]++;
            }

    for(i = 0; i < row - 1; i++)
        for(j = 0; j < row; j++)
            if(elemAppearance[i][0] >= elemAppearance[j][0])
                for(k = 0; k < 3; k++){
                    aux = elemAppearance[i][k];
                    elemAppearance[i][k] = elemAppearance[j][k];
                    elemAppearance[j][k] = aux;
                }

    return;
}

void printMat(char **mat, int c, int r){
    int i, j;
    for(i = 0; i < r; i++)
        for(j = 0; j <= c; j++){
            if (j == c) printf("\n");
        else
            printf("%c", mat[i][j]);
        }
    return;
}
/*Esta funcao marca com um 'X' (valor ASCII = 88) todas as casas, elemento por elemento hex (0-F)
em que o elemento NAO pode ser inserido de maneira alguma, isto e, colunas, linhas e regiao, alem de todas
as casas ocupadas desde a leitura do arquivo root.*/
char ***cubeSetup(char **matrix, int col, int row){
    int i, j, k, aux, aux2;
    char ***cube = calloc(row, sizeof(char **));
    char elem = '0';

    for(k = 0; k < row; k++)
        cube[k] = allocMat(col, row);

    for(i = 0; i < row; i++)
        for(j = 0; j < col; j++)
            if(matrix[i][j] != '.'){
                for(aux = 0; aux < row; aux++)
                    cube[aux][i][j] = 'X';
                }
            /*else{
                for(aux = 0; aux < row; aux++)
                    cube[aux][i][j] = '.';
                }*/

    for(k = 0; k < row; k++){
        if(k <= 9) elem = (k + ASCII0);
        else elem = (k + ASCIIA - 10);

        for (i = 0; i < row; i++)
            for(j = 0; j < col; j++){

                if (matrix[i][j] == elem){
                    for(aux = 0; aux < col; aux++)
                        cube[k][i][aux] = 'X';
                    for(aux = 0; aux < row; aux++)
                        cube[k][aux][j] = 'X';
                    for(aux = 0; aux < sqrt(row); aux++)
                        for(aux2 = 0; aux2 < sqrt(row); aux2++)
                            cube[k][(i/4)*4 + aux][(j/4)*4 + aux2] = 'X';
                }
            }

    }


    return cube;
}

void newEntryFill(char **matrix, int i, int j, int regx, int regy, char elem, int col, int row){
    int aux, aux2;

    for(aux = 0; aux < col; aux++)
        matrix[aux][j] = elem;
    for(aux = 0; aux < row; aux++)
        matrix[i][aux] = elem;

    for(aux = 0; aux < sqrt(row); aux++)
        for(aux2 = 0; aux2 < sqrt(col); aux2++)
            matrix[regx + aux][regy + aux2] = elem;
    return;
}

/*Funcao 'Search Only One Solution', preenche as casas obvias com o unico valor possivel.*/
int searchOOS(char **matrix, char ***cubeOfLines, int **elemAppearance, int col, int row){
    int i, j, k, s, counter = 0, auxi;
    char elem;
    int solved = 0;

    for(k = 0; k < row; k++){
        if (elemAppearance[k][0] < row){
            elem = elemAppearance[k][1]; //MATRIX ---> linetrack[data[k][2]]
            auxi = elemAppearance[k][2];

            for(i = 0; i < row; i++)
                for(j = 0; j < col; j++){
                    if (cubeOfLines[auxi][i][j] != 'X'){
                        for(s = 0; s < row; s++)
                            if(cubeOfLines[s][i][j] == 'X') counter++;
                        if(counter == row - 1){
                            matrix[i][j] = elem;
                            newEntryFill(cubeOfLines[auxi], i, j, (i/4)*4, (j/4)*4, 'X', col, row);
                            for(s = 0; s < row; s++) cubeOfLines[s][i][j] = 'X';
                        }
                    }
                counter = 0;
                }
            }
        else solved++;
        }

    return solved;
}

int main(){
    char *filename = malloc(sizeof(char)*6);
    char **matrix;
    char ***cubeOfLines;
    int **elemAppearance;
    int row, col, i, solved = 0;

    for(i = 0; i < 5; i++)
        scanf("%c", &filename[i]);
    filename[6] = '\0';

    matrix = readMat(filename, &col, &row);

    cubeOfLines = cubeSetup(matrix, col, row);
	elemAppearance = (int **) malloc(sizeof(int *) * row);
	for(i = 0; i < row; i++) elemAppearance[i] = (int *) calloc(3, sizeof(int));

    while(solved < row){
            getAppareances(matrix, elemAppearance, col, row);
            solved = searchOOS(matrix, cubeOfLines, elemAppearance, col, row);
    }
    printMat(matrix, col, row);

    free(filename);
    cubeFree(cubeOfLines, row);
    matrixFreeChar(matrix, row);
	matrixFreeInt(elemAppearance, row);
    return 0;
}
