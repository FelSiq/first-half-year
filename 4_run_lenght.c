#include <stdlib.h>
#include <stdio.h>

#define ENTER 10
#define MINVALUE 3

char *getString(){
    char *filename = malloc(sizeof(char) * 7);
    int counter = 0;

    while (counter < 6)
        filename[counter++] = fgetc(stdin);
    filename[counter] = '\0';
    return filename;
}

void getParameters(char *filename, int *width, int *heigth, int *maxv, FILE **fp){
    *fp = fopen(filename, "r");
    if (*fp == NULL)
        exit(0);
    fseek(*fp, 3, SEEK_SET);
    fscanf(*fp, "%d%d%d", width, heigth, maxv);
}

int **allocMat(int col, int row){
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

void getContent(int **image, char *filename, int col, int row, FILE *fp){
    int junk;
    rewind(fp);

    fseek(fp, 3, SEEK_SET);
    fscanf(fp, "%d%d%d", &junk, &junk, &junk);

    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            fscanf(fp, "%d", &image[i][j]);
}

void compactImage(int **img, int col, int row, int maxv){
    int i, j, k, s;
    printf("P8\n%d %d\n%d\n", col, row, maxv); //Parametros iniciais

    for(i = 0; i < row; i++){
        for(j = 0; j < col - 1; j++){
            if (img[i][j] == img[i][j + 1]){
                for(k = 0; j + k < col;)
                    if(img[i][j] == img[i][j + k]) k++;
                    else break;
                if (k > MINVALUE){
                    printf("@ %d %d ", img[i][j], k);
                    j += k - 1;
                } else {
                    for(s = 0; s < k; s++)
                        printf("%d ", img[i][j + s]);
                    j += s - 1;
                }
            } else printf("%d ", img[i][j]);
        }
    if (j < col) printf("%d", img[i][col - 1]);
    printf("\n");
    }
}

int main(){
    int **image, width, height, maxv;
    char *filename;
    FILE *fp;

    filename = getString();
    getParameters(filename, &width, &height, &maxv, &fp);
    image = allocMat(width, height);
    getContent(image, filename, width, height, fp);
    fclose(fp);
    free(filename);
    compactImage(image, width, height, maxv);
    freeMat(image, height);
    return 0;
}
