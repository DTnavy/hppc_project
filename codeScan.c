#include<stdio.h>
#include<stdlib.h>

typedef struct {
        char str;
        int row, col;
        int **arr;
} data;

int main() {
    FILE *f = fopen("data.txt","r");
    if ( !f ) {
        fprintf(stderr, "Error: Unable to open input file.\n");
        exit(EXIT_FAILURE);
    }
    int num;
    data mat;

    if ( fscanf(f,"%d %19s%d%d", &num, mat.str, &mat.row, &mat.col) != 4 ) {
        fprintf(stderr, "Error: wrong file format.\n");
        exit(EXIT_FAILURE);
    }
    printf("%d\n%s %d %d\n",num,mat.str,mat.row,mat.col);

    mat.arr = malloc(mat.row * sizeof *mat.arr);
    if ( !mat.arr ) {
        fprintf(stderr, "Error: malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    // now, I prefer one single allocation to keep data contiguous
    mat.arr[0] = malloc(mat.row * mat.col * sizeof *mat.arr[0]);
    if ( !mat.arr[0] ) {
        fprintf(stderr, "Error: malloc failed.\n");
        free(mat.arr);
        exit(EXIT_FAILURE);
    }
    int i, j;
    for ( i = 1; i < mat.row; ++i ) {
        mat.arr[i] = mat.arr[i - 1] + mat.col;
    }

    for ( i = 0; i < mat.row; ++i ) {
        for ( j = 0; j < mat.col; ++j ) {
            fscanf(f,"%d",&mat.arr[i][j]);
        }
    }

    for ( i = 0; i < mat.row; ++i ) {
        for ( j = 0; j < mat.col; ++j ) {
            printf("%d", mat.arr[i][j]);
        }
        printf("\n");
    }

    fclose(f);
    free(mat.arr[0]);
    free(mat.arr);
    return EXIT_SUCCESS;
}