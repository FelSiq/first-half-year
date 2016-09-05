#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define STDSIZE 47
#define REGIONSIDE 10
#define REGIONNUMBER 15
#define STDLIN 50
#define STDCOL 30

typedef enum{
    FALSE,
    TRUE
} boolean;

char *getString(){
    char *string = NULL;
    int counter = 0;
    char c = 0;

    while(c != 'm'){
        c = fgetc(stdin);
        string = (char *) realloc(string, sizeof(char) * (counter + 1));
        string[counter++] = c;
    }

    string[counter] = '\0';

    return string;
}

int **allocMat(int row, int col){
    int **mat = malloc(sizeof(int *) * row);
    for(int i = 0; i < row; i++)
        mat[i] = malloc(sizeof(int) * col);
    return mat;
}

void freeMat(int **mat, int row){
    for(int i = 0; i < row; i++)
        free(mat[i]);
    free(mat);
}

void getContent(int **mat, char *filename, FILE *fp, int row, int col){
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            fscanf(fp, "%d", &mat[i][j]);
}

void printMat(int **mat, int row, int col){
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
}

FILE *getDimensions(char *filename, int *row, int *col, int *nexpected){
    int junk;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) exit(0);

    fseek(fp, 3, SEEK_SET);
    fscanf(fp, "%d%d%d", col, row, &junk);

    (*nexpected) = ceil(*col * 1.0/STDSIZE * 1.0);

    return fp;
}

void cleanMat(int **mat, int row, int col){
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            mat[i][j] = 0;
}

boolean recognizeDigit(int **image){
    int **vector = allocMat(2, REGIONNUMBER), sum = 0;
    cleanMat(vector, 2, REGIONNUMBER);

    for(int s = 0; s < 5; s++)
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < REGIONSIDE; j++)
                for(int k = 0; k < REGIONSIDE; k++)
                    if (image[j + s*REGIONSIDE][k + i*REGIONSIDE] == 1)
                        vector[1][s*3 + i]++;
            if (vector[1][s*3 + i] >= (pow(REGIONSIDE, 2)/2))
                vector[0][s*3 + i] = 1;
        }

    //Esta secao gera um codigo unico para cada digito, a fim de reconhece-lo.
    for(int i = 0; i < REGIONNUMBER; i++)
        sum += ((pow((vector[0][i]), 2))*pow(i + 1, 2));

    switch(sum){
        case 1030: printf("0"); break;
        case 410: printf("1"); break;
        case 934: printf("2"); break;
        case 978: printf("3"); break;
        case 625: printf("4"); break;
        case 958: printf("5"); break;
        case 1058: printf("6"); break;
        case 500: printf("7"); break;
        case 1094: printf("8"); break;
        case 994: printf("9"); break;
        default:
            freeMat(vector, 2);
            return FALSE;
    }

    freeMat(vector, 2);
    return TRUE;
}

void analyzeImage(int **img, int nexpected, int row, int col){
    int **digit = allocMat(STDLIN, STDCOL), vdisplacement = 0, hdisplacement = 0, i = 0, j;

    while(i < nexpected){
        cleanMat(digit, STDLIN, STDCOL);
        for(j = 0; j + vdisplacement < row && j < STDLIN; j++)
            memcpy(digit[j], &img[vdisplacement + j][hdisplacement], sizeof(int) * STDCOL);

        if (recognizeDigit(digit)) {
                vdisplacement = 0;
                hdisplacement += STDCOL;
                i++;
        } else {
            if(j + vdisplacement < row)
                vdisplacement += REGIONSIDE/2;
            else{
                vdisplacement = 0;
                hdisplacement += REGIONSIDE/2;
            }
        }
    }
    freeMat(digit, STDLIN);
}

int main(){
    char *filename;
    int **image, row, col, nexpected;
    FILE *fp;

    filename = getString();
    fp = getDimensions(filename, &row, &col, &nexpected);
    image = allocMat(row, col);
    getContent(image, filename, fp, row, col);
    free(filename);
    fclose(fp);
    analyzeImage(image, nexpected, row, col);
    freeMat(image, row);

    return 0;
}
